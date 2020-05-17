#include "usb_disk_mode.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>

// this
#include "flash.h"
#include "globals.h"
#include "teletype.h"

// libavr32
#include "font.h"
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


static uint8_t grid_state = 0;
static uint16_t grid_count = 0;
static uint8_t grid_num = 0;

static void grid_usb_write(scene_state_t *scene);
static void grid_usb_read(scene_state_t *scene, char c);


void tele_usb_disk() {
    char input_buffer[32];
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

        // WRITE SCENES
        char filename[13];
        strcpy(filename, "tt00s.txt");

        print_dbg("\r\nwriting scenes");
        strcpy(input_buffer, "WRITE");
        region_fill(&line[0], 0);
        font_string_region_clip_tab(&line[0], input_buffer, 2, 0, 0xa, 0);
        region_draw(&line[0]);

        for (int i = 0; i < SCENE_SLOTS; i++) {
            scene_state_t scene;
            ss_init(&scene);

            char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
            memset(text, 0, SCENE_TEXT_LINES * SCENE_TEXT_CHARS);

            strcat(input_buffer, ".");
            region_fill(&line[0], 0);
            font_string_region_clip_tab(&line[0], input_buffer, 2, 0, 0xa, 0);
            region_draw(&line[0]);

            flash_read(i, &scene, &text, 1, 1);

            if (!nav_file_create((FS_STRING)filename)) {
                if (fs_g_status != FS_ERR_FILE_EXIST) {
                    if (fs_g_status == FS_LUN_WP) {
                        // Test can be done only on no write protected
                        // device
                        continue;
                    }
                    lun_state |= (1 << lun);  // LUN test is done.
                    print_dbg("\r\nfail");
                    continue;
                }
            }
            if (!file_open(FOPEN_MODE_W)) {
                if (fs_g_status == FS_LUN_WP) {
                    // Test can be done only on no write protected
                    // device
                    continue;
                }
                lun_state |= (1 << lun);  // LUN test is done.
                print_dbg("\r\nfail");
                continue;
            }

            char blank = 0;
            for (int l = 0; l < SCENE_TEXT_LINES; l++) {
                if (strlen(text[l])) {
                    file_write_buf((uint8_t *)text[l], strlen(text[l]));
                    file_putc('\n');
                    blank = 0;
                }
                else if (!blank) {
                    file_putc('\n');
                    blank = 1;
                }
            }

            char input[36];
            for (int s = 0; s < 10; s++) {
                file_putc('\n');
                file_putc('\n');
                file_putc('#');
                if (s == 8)
                    file_putc('M');
                else if (s == 9)
                    file_putc('I');
                else
                    file_putc(s + 49);

                for (int l = 0; l < ss_get_script_len(&scene, s); l++) {
                    file_putc('\n');
                    print_command(ss_get_script_command(&scene, s, l), input);
                    file_write_buf((uint8_t *)input, strlen(input));
                }
            }

            file_putc('\n');
            file_putc('\n');
            file_putc('#');
            file_putc('P');
            file_putc('\n');

            for (int b = 0; b < 4; b++) {
                itoa(ss_get_pattern_len(&scene, b), input, 10);
                file_write_buf((uint8_t *)input, strlen(input));
                if (b == 3)
                    file_putc('\n');
                else
                    file_putc('\t');
            }

            for (int b = 0; b < 4; b++) {
                itoa(ss_get_pattern_wrap(&scene, b), input, 10);
                file_write_buf((uint8_t *)input, strlen(input));
                if (b == 3)
                    file_putc('\n');
                else
                    file_putc('\t');
            }

            for (int b = 0; b < 4; b++) {
                itoa(ss_get_pattern_start(&scene, b), input, 10);
                file_write_buf((uint8_t *)input, strlen(input));
                if (b == 3)
                    file_putc('\n');
                else
                    file_putc('\t');
            }

            for (int b = 0; b < 4; b++) {
                itoa(ss_get_pattern_end(&scene, b), input, 10);
                file_write_buf((uint8_t *)input, strlen(input));
                if (b == 3)
                    file_putc('\n');
                else
                    file_putc('\t');
            }

            file_putc('\n');

            for (int l = 0; l < 64; l++) {
                for (int b = 0; b < 4; b++) {
                    itoa(ss_get_pattern_val(&scene, b, l), input, 10);
                    file_write_buf((uint8_t *)input, strlen(input));
                    if (b == 3)
                        file_putc('\n');
                    else
                        file_putc('\t');
                }
            }

            grid_usb_write(&scene);

            file_close();
            lun_state |= (1 << lun);  // LUN test is done.

            if (filename[3] == '9') {
                filename[3] = '0';
                filename[2]++;
            }
            else
                filename[3]++;

            print_dbg(".");
        }

        nav_filelist_reset();


        // READ SCENES
        strcpy(filename, "tt00.txt");
        print_dbg("\r\nreading scenes...");

        strcpy(input_buffer, "READ");
        region_fill(&line[1], 0);
        font_string_region_clip_tab(&line[1], input_buffer, 2, 0, 0xa, 0);
        region_draw(&line[1]);

        for (int i = 0; i < SCENE_SLOTS; i++) {
            scene_state_t scene;
            ss_init(&scene);
            char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
            memset(text, 0, SCENE_TEXT_LINES * SCENE_TEXT_CHARS);

            strcat(input_buffer, ".");
            region_fill(&line[1], 0);
            font_string_region_clip_tab(&line[1], input_buffer, 2, 0, 0xa, 0);
            region_draw(&line[1]);
            if (nav_filelist_findname(filename, 0)) {
                print_dbg("\r\nfound: ");
                print_dbg(filename);
                if (!file_open(FOPEN_MODE_R))
                    print_dbg("\r\ncan't open");
                else {
                    char c;
                    uint8_t l = 0;
                    uint8_t p = 0;
                    int8_t s = 99;
                    uint8_t b = 0;
                    uint16_t num = 0;
                    int8_t neg = 1;

                    char input[32];
                    memset(input, 0, sizeof(input));

                    while (!file_eof() && s != -1) {
                        c = toupper(file_getc());
                        // print_dbg_char(c);

                        if (c == '#') {
                            if (!file_eof()) {
                                c = toupper(file_getc());
                                // print_dbg_char(c);

                                if (c == 'M')
                                    s = 8;
                                else if (c == 'I')
                                    s = 9;
                                else if (c == 'P')
                                    s = 10;
                                else if (c == 'G') {
                                    grid_state = grid_num = grid_count = 0;
                                    s = 11;
                                }
                                else {
                                    s = c - 49;
                                    if (s < 0 || s > 7) s = -1;
                                }

                                l = 0;
                                p = 0;

                                if (!file_eof()) c = toupper(file_getc());
                            }
                            else
                                s = -1;

                            // print_dbg("\r\nsection: ");
                            // print_dbg_ulong(s);
                        }
                        // SCENE TEXT
                        else if (s == 99) {
                            if (c == '\n') {
                                l++;
                                p = 0;
                            }
                            else {
                                if (l < SCENE_TEXT_LINES &&
                                    p < SCENE_TEXT_CHARS) {
                                    text[l][p] = c;
                                    p++;
                                }
                            }
                        }
                        // SCRIPTS
                        else if (s >= 0 && s <= 9) {
                            if (c == '\n') {
                                if (p && l < SCRIPT_MAX_COMMANDS) {
                                    tele_command_t temp;
                                    temp.comment = false;
                                    error_t status;
                                    char error_msg[TELE_ERROR_MSG_LENGTH];
                                    status = parse(input, &temp, error_msg);

                                    if (status == E_OK) {
                                        status = validate(&temp, error_msg);

                                        if (status == E_OK) {
                                            ss_overwrite_script_command(
                                                &scene, s, l, &temp);
                                            l++;
                                        }
                                        else {
                                            print_dbg("\r\nvalidate: ");
                                            print_dbg(tele_error(status));
                                            print_dbg(" >> ");
                                            print_dbg("\r\nINPUT: ");
                                            print_dbg(input);
                                        }
                                    }
                                    else {
                                        print_dbg("\r\nERROR: ");
                                        print_dbg(tele_error(status));
                                        print_dbg(" >> ");
                                        print_dbg("\r\nINPUT: ");
                                        print_dbg(input);
                                    }

                                    memset(input, 0, sizeof(input));
                                    p = 0;
                                }
                            }
                            else {
                                if (p < 32) input[p] = c;
                                p++;
                            }
                        }
                        // PATTERNS
                        // tele_patterns[]. l wrap start end v[64]
                        else if (s == 10) {
                            if (c == '\n' || c == '\t') {
                                if (b < 4) {
                                    if (l > 3) {
                                        ss_set_pattern_val(&scene, b, l - 4,
                                                           neg * num);
                                        // print_dbg("\r\nset: ");
                                        // print_dbg_ulong(b);
                                        // print_dbg(" ");
                                        // print_dbg_ulong(l-4);
                                        // print_dbg(" ");
                                        // print_dbg_ulong(num);
                                    }
                                    else if (l == 0) {
                                        ss_set_pattern_len(&scene, b, num);
                                    }
                                    else if (l == 1) {
                                        ss_set_pattern_wrap(&scene, b, num);
                                    }
                                    else if (l == 2) {
                                        ss_set_pattern_start(&scene, b, num);
                                    }
                                    else if (l == 3) {
                                        ss_set_pattern_end(&scene, b, num);
                                    }
                                }

                                b++;
                                num = 0;
                                neg = 1;

                                if (c == '\n') {
                                    if (p) l++;
                                    if (l > 68) s = -1;
                                    b = 0;
                                    p = 0;
                                }
                            }
                            else {
                                if (c == '-')
                                    neg = -1;
                                else if (c >= '0' && c <= '9') {
                                    num = num * 10 + (c - 48);
                                    // print_dbg("\r\nnum: ");
                                    // print_dbg_ulong(num);
                                }
                                p++;
                            }
                        }
                        // GRID
                        else if (s == 11) {
                            grid_usb_read(&scene, c);
                        }
                    }


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

    nav_exit();
}

char fvalue[36];
static void grid_usb_write(scene_state_t *scene) {
    file_putc('\n');
    file_putc('#');
    file_putc('G');
    file_putc('\n');
    for (uint16_t i = 0; i < GRID_BUTTON_COUNT; i++) {
        file_putc('0' + scene->grid.button[i].state);
        if ((i & 15) == 15) file_putc('\n');
    }
    file_putc('\n');
    for (uint16_t i = 0; i < GRID_FADER_COUNT; i++) {
        itoa(scene->grid.fader[i].value, fvalue, 10);
        file_write_buf((uint8_t *)fvalue, strlen(fvalue));
        file_putc((i & 15) == 15 ? '\n' : '\t');
    }
}

static void grid_usb_read(scene_state_t *scene, char c) {
    if (grid_state == 0) {
        if (c >= '0' && c <= '9') {
            scene->grid.button[grid_count].state = c != '0';
            if (++grid_count >= GRID_BUTTON_COUNT) {
                grid_count = 0;
                grid_state = 1;
                if (!file_eof()) file_getc();
                if (!file_eof()) file_getc();  // eat \n\n
            }
        }
    }
    else if (grid_state == 1) {
        if (c >= '0' && c <= '9') { grid_num = grid_num * 10 + c - '0'; }
        else if (c == '\t' || c == '\n') {
            if (grid_count < GRID_FADER_COUNT) {
                scene->grid.fader[grid_count].value = grid_num;
                grid_num = 0;
                grid_count++;
            }
        }
    }
}