#include "flash.h"

#include <string.h>

// asf
#include "flashc.h"
#include "gpio.h"
#include "init_teletype.h"
#include "print_funcs.h"

// this
#include "teletype.h"

#define FIRSTRUN_KEY 0x22
#define BUTTON_STATE_SIZE (GRID_BUTTON_COUNT >> 3)

typedef struct {
    uint8_t button_states[BUTTON_STATE_SIZE];
    uint8_t fader_states[GRID_FADER_COUNT];
} grid_data_t;
static grid_data_t grid_data;

// NVRAM data structure located in the flash array.
typedef const struct {
    scene_script_t scripts[SCRIPT_COUNT - 1];  // Exclude TEMP script
    scene_pattern_t patterns[PATTERN_COUNT];
    grid_data_t grid_data;
    char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
} nvram_scene_t;

typedef const struct {
    nvram_scene_t scenes[SCENE_SLOTS];
    uint8_t last_scene;
    tele_mode_t last_mode;
    uint8_t fresh;
    cal_data_t cal;
    device_config_t device_config;
} nvram_data_t;


static __attribute__((__section__(".flash_nvram"))) nvram_data_t f;

static void pack_grid(scene_state_t *scene);
static void unpack_grid(scene_state_t *scene);

u8 is_flash_fresh() {
    return f.fresh != FIRSTRUN_KEY;
}

void flash_prepare() {
    // if it's not empty return
    if (f.fresh != FIRSTRUN_KEY) {
        int confirm = 1;
        uint32_t counter = 0;
        int toggle = 0;
#define TIMEOUT 50000
        while (confirm == 1 && (++counter < TIMEOUT)) {
            confirm = gpio_get_pin_value(NMI);
            if ((counter % 1000) == 0) {
                if (++toggle % 2)
                    gpio_set_pin_low(B11);
                else
                    gpio_set_pin_high(B11);
            }
            print_dbg_ulong(confirm);
        }
        gpio_set_pin_low(B11);
        if (counter >= TIMEOUT) return;

        print_dbg("\r\n:::: first run, clearing flash");
        print_dbg("\r\nflash size: ");
        print_dbg_ulong(sizeof(f));

        // blank scene to write to flash
        scene_state_t scene;
        ss_init(&scene);

        char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
        memset(text, 0, SCENE_TEXT_LINES * SCENE_TEXT_CHARS);

        for (uint8_t i = 0; i < SCENE_SLOTS; i++) {
            flash_write(i, &scene, &text);
        }

        cal_data_t cal = { 0,
                           16383,
                           0,
                           16383,
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                           { 16383, 16383, 16383, 16383, 16383, 16383, 16383,
                             16383, 16383, 16383, 16383, 16383, 16383, 16383,
                             16383, 16383 } };
        flashc_memcpy((void *)&f.cal, &cal, sizeof(cal), true);
        device_config_t device_config = {.flip = 0 };
        flashc_memcpy((void *)&f.device_config, &device_config,
                      sizeof(device_config), true);
        flash_update_last_saved_scene(0);
        flash_update_last_mode(M_LIVE);
        flashc_memset8((void *)&f.fresh, FIRSTRUN_KEY, 1, true);
    }
}

void flash_write(uint8_t preset_no, scene_state_t *scene,
                 char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS]) {
    if (preset_no >= SCENE_SLOTS) return;
    flashc_memcpy((void *)&f.scenes[preset_no].scripts, ss_scripts_ptr(scene),
                  // Exclude TEMP script from flash storage by subtracting one
                  ss_scripts_size() - sizeof(scene_script_t), true);
    flashc_memcpy((void *)&f.scenes[preset_no].patterns, ss_patterns_ptr(scene),
                  ss_patterns_size(), true);
    pack_grid(scene);
    flashc_memcpy((void *)&f.scenes[preset_no].grid_data, &grid_data,
                  sizeof(grid_data_t), true);
    flashc_memcpy((void *)&f.scenes[preset_no].text, text,
                  SCENE_TEXT_LINES * SCENE_TEXT_CHARS, true);
}

void flash_read(uint8_t preset_no, scene_state_t *scene,
                char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS],
                uint8_t init_pattern, uint8_t init_grid,
                uint8_t init_i2c_op_address) {
    memcpy(ss_scripts_ptr(scene), &f.scenes[preset_no].scripts,
           // Exclude size of TEMP script as above
           ss_scripts_size() - sizeof(scene_script_t));
    if (init_pattern) {
        memcpy(ss_patterns_ptr(scene), &f.scenes[preset_no].patterns,
               ss_patterns_size());
    }
    if (init_grid) {
        memcpy(&grid_data, &f.scenes[preset_no].grid_data, sizeof(grid_data_t));
        unpack_grid(scene);
    }
    memcpy(text, &f.scenes[preset_no].text,
           SCENE_TEXT_LINES * SCENE_TEXT_CHARS);
    // need to reset timestamps
    uint32_t ticks = get_ticks();
    for (size_t i = 0; i < TEMP_SCRIPT; i++)
        scene->scripts[i].last_time = ticks;
    scene->variables.time = 0;

    if (init_i2c_op_address) scene->i2c_op_address = -1;
    ss_midi_init(scene);
}

uint8_t flash_last_saved_scene() {
    return f.last_scene;
}

void flash_update_last_saved_scene(uint8_t preset_no) {
    if (preset_no >= SCENE_SLOTS) return;
    flashc_memset8((void *)&f.last_scene, preset_no, 1, true);
}

const char *flash_scene_text(uint8_t preset_no, size_t line) {
    return f.scenes[preset_no].text[line];
}

tele_mode_t flash_last_mode() {
    return f.last_mode;
}

void flash_update_last_mode(tele_mode_t mode) {
    // flashc_memset8((void *)&f.last_mode, mode, sizeof(tele_mode_t), true);
}

void flash_update_cal(cal_data_t *cal) {
    flashc_memcpy((void *)&f.cal, cal, sizeof(cal_data_t), true);
}

void flash_get_cal(cal_data_t *cal) {
    *cal = f.cal;
}

void flash_update_device_config(device_config_t *device_config) {
    flashc_memcpy((void *)&f.device_config, device_config,
                  sizeof(device_config_t), true);
}

void flash_get_device_config(device_config_t *device_config) {
    *device_config = f.device_config;
}

static void pack_grid(scene_state_t *scene) {
    uint8_t byte = 0;
    uint8_t byte_count = 0;
    for (uint16_t i = 0; i < GRID_BUTTON_COUNT; i++) {
        byte |= (scene->grid.button[i].state != 0) << (i & 7);
        if ((i & 7) == 7) {
            grid_data.button_states[byte_count] = byte;
            byte = 0;
            if (++byte_count >= BUTTON_STATE_SIZE) break;
        }
    }
    for (uint16_t i = 0; i < GRID_FADER_COUNT; i++)
        grid_data.fader_states[i] = scene->grid.fader[i].value;
}

static void unpack_grid(scene_state_t *scene) {
    for (uint16_t i = 0; i < GRID_BUTTON_COUNT; i++) {
        scene->grid.button[i].state =
            0 != (grid_data.button_states[i >> 3] & (1 << (i & 7)));
    }
    for (uint16_t i = 0; i < GRID_FADER_COUNT; i++)
        scene->grid.fader[i].value = grid_data.fader_states[i];
}
