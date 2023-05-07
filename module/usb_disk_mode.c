#include "usb_disk_mode.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include "flash.h"
#include "globals.h"
#include "scene_serialization.h"

// libavr32
#include "adc.h"
#include "font.h"
#include "interrupts.h"
#include "region.h"
#include "util.h"

// asf
#include "delay.h"
#include "fat.h"
#include "file.h"
#include "fs_com.h"
#include "navigation.h"
#include "print_funcs.h"
#include "uhi_msc.h"
#include "uhi_msc_mem.h"
#include "usb_protocol_msc.h"

// Local declarations
void draw_usb_menu_item(uint8_t item_num, const char* text);
bool tele_usb_disk_write_operation(uint8_t* plun_state, uint8_t* plun);
void tele_usb_disk_read_operation(void);


// Local functions to implement the usb filesystem serialization contract
void tele_usb_putc(void* self_data, uint8_t c);
void tele_usb_write_buf(void* self_data, uint8_t* buffer, uint16_t size);
uint16_t tele_usb_getc(void* self_data);
bool tele_usb_eof(void* self_data);

void tele_usb_putc(void* self_data, uint8_t c) {
    file_putc(c);
}

void tele_usb_write_buf(void* self_data, uint8_t* buffer, uint16_t size) {
    file_write_buf(buffer, size);
}

uint16_t tele_usb_getc(void* self_data) {
    return file_getc();
}

bool tele_usb_eof(void* self_data) {
    return file_eof() != 0;
}

// *very* basic USB operations menu


typedef enum {
    USB_MENU_COMMAND_EXIT = 0,
    USB_MENU_COMMAND_BOTH = 1,
    USB_MENU_COMMAND_READ = 2,
    USB_MENU_COMMAND_WRITE = 3,
} usb_menu_command_t;

usb_menu_command_t usb_menu_command;

void draw_usb_menu_item(uint8_t item_num, const char* text) {
    uint8_t line_num = 7 - item_num;
    uint8_t fg = usb_menu_command == item_num ? 0 : 0xa;
    uint8_t bg = usb_menu_command == item_num ? 0xa : 0;
    region_fill(&line[line_num], bg);
    font_string_region_clip_tab(&line[line_num], text, 2, 0, fg, bg);
    region_draw(&line[line_num]);
}

void handler_usb_PollADC(int32_t data) {
    uint16_t adc[4];
    adc_convert(&adc);
    uint8_t cursor = adc[1] >> 9;
    uint8_t deadzone = cursor & 1;
    cursor >>= 1;
    if (!deadzone || abs(cursor - usb_menu_command) > 1) {
        usb_menu_command = cursor;
    }
}

void handler_usb_Front(int32_t data) {

    // disable timers
    u8 flags = irqs_pause();

    if (usb_menu_command != USB_MENU_COMMAND_EXIT) {
        tele_usb_disk();
    }

    // renable teletype
    set_mode(M_LIVE);
    assign_main_event_handlers();
    irqs_resume(flags);
}

void handler_usb_ScreenRefresh(int32_t data) {
    draw_usb_menu_item(3, "WRITE TO USB");
    draw_usb_menu_item(2, "READ FROM USB");
    draw_usb_menu_item(1, "DO BOTH");
    draw_usb_menu_item(0, "EXIT");
}


// usb disk mode entry point
void tele_usb_disk() {

    print_dbg("\r\nusb");
    uint8_t lun_state = 0;

    for (uint8_t lun = 0; (lun < uhi_msc_mem_get_lun()) && (lun < 8); lun++) {
        // print_dbg("\r\nlun: ");
        // print_dbg_ulong(lun);

        // Mount drive
        nav_drive_set(lun);
        if (!nav_partition_mount()) {
            if (fs_g_status == FS_ERR_HW_NO_PRESENT) {
                // The test can not be done, if LUN is not present
                lun_state &= ~(1 << lun);  // LUN test reseted
                continue;
            }
            lun_state |= (1 << lun);  // LUN test is done.
            print_dbg("\r\nfail");
            // ui_test_finish(false); // Test fail
            continue;
        }
        // Check if LUN has been already tested
        if (lun_state & (1 << lun)) { continue; }

        if (usb_menu_command == USB_MENU_COMMAND_WRITE ||
            usb_menu_command == USB_MENU_COMMAND_BOTH) {
            if (!tele_usb_disk_write_operation(&lun_state, &lun)) { continue; }
        }
        if (usb_menu_command == USB_MENU_COMMAND_READ ||
            usb_menu_command == USB_MENU_COMMAND_BOTH) {
            tele_usb_disk_read_operation();
        }

        nav_exit();
    }
}

bool tele_usb_disk_write_operation(uint8_t* plun_state, uint8_t* plun) {
    // WRITE SCENES
    print_dbg("\r\nwriting scenes");

    char filename[13];
    strcpy(filename, "tt00s.txt");

    char text_buffer[40];
    strcpy(text_buffer, "WRITE");
    region_fill(&line[0], 0);
    font_string_region_clip_tab(&line[0], text_buffer, 2, 0, 0xa, 0);
    region_draw(&line[0]);

    for (int i = 0; i < SCENE_SLOTS; i++) {
        scene_state_t scene;
        ss_init(&scene);

        char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
        memset(text, 0, SCENE_TEXT_LINES * SCENE_TEXT_CHARS);

        strcat(text_buffer, ".");  // strcat is dangerous, make sure the
                                   // buffer is large enough!
        region_fill(&line[0], 0);
        font_string_region_clip_tab(&line[0], text_buffer, 2, 0, 0xa, 0);
        region_draw(&line[0]);

        flash_read(i, &scene, &text, 1, 1, 1);

        if (!nav_file_create((FS_STRING)filename)) {
            if (fs_g_status != FS_ERR_FILE_EXIST) {
                if (fs_g_status == FS_LUN_WP) {
                    // Test can be done only on no write protected
                    // device
                    return false;
                }
                *plun_state |= (1 << *plun);  // LUN test is done.
                print_dbg("\r\nfail");
                return false;
            }
        }

        if (!file_open(FOPEN_MODE_W)) {
            if (fs_g_status == FS_LUN_WP) {
                // Test can be done only on no write protected
                // device
                return false;
            }
            *plun_state |= (1 << *plun);  // LUN test is done.
            print_dbg("\r\nfail");
            return false;
        }

        tt_serializer_t tele_usb_writer;
        tele_usb_writer.write_char = &tele_usb_putc;
        tele_usb_writer.write_buffer = &tele_usb_write_buf;
        tele_usb_writer.print_dbg = &print_dbg;
        tele_usb_writer.data =
            NULL;  // asf disk i/o holds state, no handles needed
        serialize_scene(&tele_usb_writer, &scene, &text);

        file_close();
        *plun_state |= (1 << *plun);  // LUN test is done.

        if (filename[3] == '9') {
            filename[3] = '0';
            filename[2]++;
        }
        else
            filename[3]++;

        print_dbg(".");
    }

    nav_filelist_reset();
    return true;
}

void tele_usb_disk_read_operation() {
    // READ SCENES
    print_dbg("\r\nreading scenes...");

    char filename[13];
    strcpy(filename, "tt00.txt");

    char text_buffer[40];
    strcpy(text_buffer, "READ");
    region_fill(&line[1], 0);
    font_string_region_clip_tab(&line[1], text_buffer, 2, 0, 0xa, 0);
    region_draw(&line[1]);

    for (int i = 0; i < SCENE_SLOTS; i++) {
        scene_state_t scene;
        ss_init(&scene);
        char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
        memset(text, 0, SCENE_TEXT_LINES * SCENE_TEXT_CHARS);

        strcat(text_buffer, ".");  // strcat is dangerous, make sure the
                                   // buffer is large enough!
        region_fill(&line[1], 0);
        font_string_region_clip_tab(&line[1], text_buffer, 2, 0, 0xa, 0);
        region_draw(&line[1]);
        if (nav_filelist_findname(filename, 0)) {
            print_dbg("\r\nfound: ");
            print_dbg(filename);
            if (!file_open(FOPEN_MODE_R))
                print_dbg("\r\ncan't open");
            else {
                tt_deserializer_t tele_usb_reader;
                tele_usb_reader.read_char = &tele_usb_getc;
                tele_usb_reader.eof = &tele_usb_eof;
                tele_usb_reader.print_dbg = &print_dbg;
                tele_usb_reader.data =
                    NULL;  // asf disk i/o holds state, no handles needed
                deserialize_scene(&tele_usb_reader, &scene, &text);

                file_close();
                flash_write(i, &scene, &text);
            }
        }

        nav_filelist_reset();

        if (filename[3] == '9') {
            filename[3] = '0';
            filename[2]++;
        }
        else
            filename[3]++;
    }
}
