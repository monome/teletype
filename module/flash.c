#include "flash.h"

#include <string.h>

// asf
#include "flashc.h"
#include "print_funcs.h"

// this
#include "teletype.h"

#define FIRSTRUN_KEY 0x22

// NVRAM data structure located in the flash array.
typedef const struct {
    scene_script_t scripts[SCRIPT_COUNT - 1];
    scene_pattern_t patterns[PATTERN_COUNT];
    char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
} nvram_scene_t;

typedef const struct {
    nvram_scene_t scenes[SCENE_SLOTS];
    uint8_t last_scene;
    uint8_t fresh;
} nvram_data_t;


static __attribute__((__section__(".flash_nvram"))) nvram_data_t f;

void flash_prepare() {
    // if it's not empty return
    if (f.fresh == FIRSTRUN_KEY) return;

    print_dbg("\r\n:::: first run, clearing flash");
    print_dbg("\r\nflash size: ");
    print_dbg_ulong(sizeof(f));

    // blank scene to write to flash
    scene_state_t scene;
    ss_init(&scene);

    char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
    memset(text, 0, SCENE_TEXT_LINES * SCENE_TEXT_CHARS);

    for (uint8_t i = 0; i < SCENE_SLOTS; i++) { flash_write(i, &scene, &text); }
    flash_update_last_saved_scene(0);
    flashc_memset8((void *)&f.fresh, FIRSTRUN_KEY, 1, true);
}

void flash_write(uint8_t preset_no, scene_state_t *scene,
                 char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS]) {
    flashc_memcpy((void *)&f.scenes[preset_no].scripts, ss_scripts_ptr(scene),
                  ss_scripts_size() - sizeof(scene_script_t), true);
    flashc_memcpy((void *)&f.scenes[preset_no].patterns, ss_patterns_ptr(scene),
                  ss_patterns_size(), true);
    flashc_memcpy((void *)&f.scenes[preset_no].text, text,
                  SCENE_TEXT_LINES * SCENE_TEXT_CHARS, true);
}

void flash_read(uint8_t preset_no, scene_state_t *scene,
                char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS]) {
    memcpy(ss_scripts_ptr(scene), &f.scenes[preset_no].scripts,
           ss_scripts_size() - sizeof(scene_script_t));
    memcpy(ss_patterns_ptr(scene), &f.scenes[preset_no].patterns,
           ss_patterns_size());
    memcpy(text, &f.scenes[preset_no].text,
           SCENE_TEXT_LINES * SCENE_TEXT_CHARS);
}

uint8_t flash_last_saved_scene() {
    return f.last_scene;
}

void flash_update_last_saved_scene(uint8_t preset_no) {
    flashc_memset8((void *)&f.last_scene, preset_no, 1, true);
}

const char *flash_scene_text(uint8_t preset_no, size_t line) {
    return f.scenes[preset_no].text[line];
}
