#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>

#include "globals.h"
#include "line_editor.h"
#include "teletype.h"

#define SCENE_SLOTS 32

u8 is_flash_fresh(void);
void flash_prepare(void);
void flash_read(uint8_t preset_no, scene_state_t *scene,
                char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS],
                uint8_t init_grid, uint8_t init_i2c_op_address);
void flash_write(uint8_t preset_no, scene_state_t *scene,
                 char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS]);
uint8_t flash_last_saved_scene(void);
void flash_update_last_saved_scene(uint8_t preset_no);
const char *flash_scene_text(uint8_t preset_no, size_t line);
tele_mode_t flash_last_mode(void);
void flash_update_last_mode(tele_mode_t mode);
void flash_update_cal(cal_data_t *);
void flash_get_cal(cal_data_t *);
void flash_update_cal(cal_data_t *);
void flash_get_cal(cal_data_t *);
void flash_update_device_config(device_config_t *);
void flash_get_device_config(device_config_t *);

#endif
