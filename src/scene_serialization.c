#include "scene_serialization.h"

#include "teletype.h"
#include "util.h"

uint8_t grid_state = 0;
uint16_t grid_count = 0;
uint8_t grid_num = 0;

// internal test functions to make sure serializer struct is filled out
bool check_serializer(tt_serializer_t* stream);
bool check_deserializer(tt_deserializer_t* stream);

// internal helper functions for grid data serialization
void serialize_grid(tt_serializer_t* stream, scene_state_t* scene);
void deserialize_grid(tt_deserializer_t* stream, scene_state_t* scene, char c);


void serialize_scene(tt_serializer_t* stream, scene_state_t* scene,
                     char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS]) {
    if (!check_serializer(stream)) { return; }

    char blank = 0;
    for (int l = 0; l < SCENE_TEXT_LINES; l++) {
        size_t line_length = strlen((*text)[l]);
        if (line_length > 0) {
            stream->write_buffer(stream->data, (uint8_t*)((*text)[l]),
                                 line_length);
            stream->write_char(stream->data, '\n');
            blank = 0;
        }
        else if (!blank) {
            stream->write_char(stream->data, '\n');
            blank = 1;
        }
    }

    char input[36];
    for (int s = 0; s < 10; s++) {
        stream->write_char(stream->data, '\n');
        stream->write_char(stream->data, '\n');
        stream->write_char(stream->data, '#');
        if (s == 8)
            stream->write_char(stream->data, 'M');
        else if (s == 9)
            stream->write_char(stream->data, 'I');
        else
            stream->write_char(stream->data, s + 49);

        for (int l = 0; l < ss_get_script_len(scene, s); l++) {
            stream->write_char(stream->data, '\n');
            print_command(ss_get_script_command(scene, s, l), input);
            stream->write_buffer(stream->data, (uint8_t*)input, strlen(input));
        }
    }

    stream->write_char(stream->data, '\n');
    stream->write_char(stream->data, '\n');
    stream->write_char(stream->data, '#');
    stream->write_char(stream->data, 'P');
    stream->write_char(stream->data, '\n');

    for (int b = 0; b < 4; b++) {
        itoa(ss_get_pattern_len(scene, b), input, 10);
        stream->write_buffer(stream->data, (uint8_t*)input, strlen(input));
        if (b == 3)
            stream->write_char(stream->data, '\n');
        else
            stream->write_char(stream->data, '\t');
    }

    for (int b = 0; b < 4; b++) {
        itoa(ss_get_pattern_wrap(scene, b), input, 10);
        stream->write_buffer(stream->data, (uint8_t*)input, strlen(input));
        if (b == 3)
            stream->write_char(stream->data, '\n');
        else
            stream->write_char(stream->data, '\t');
    }

    for (int b = 0; b < 4; b++) {
        itoa(ss_get_pattern_start(scene, b), input, 10);
        stream->write_buffer(stream->data, (uint8_t*)input, strlen(input));
        if (b == 3)
            stream->write_char(stream->data, '\n');
        else
            stream->write_char(stream->data, '\t');
    }

    for (int b = 0; b < 4; b++) {
        itoa(ss_get_pattern_end(scene, b), input, 10);
        stream->write_buffer(stream->data, (uint8_t*)input, strlen(input));
        if (b == 3)
            stream->write_char(stream->data, '\n');
        else
            stream->write_char(stream->data, '\t');
    }

    stream->write_char(stream->data, '\n');

    for (int l = 0; l < 64; l++) {
        for (int b = 0; b < 4; b++) {
            itoa(ss_get_pattern_val(scene, b, l), input, 10);
            stream->write_buffer(stream->data, (uint8_t*)input, strlen(input));
            if (b == 3)
                stream->write_char(stream->data, '\n');
            else
                stream->write_char(stream->data, '\t');
        }
    }

    serialize_grid(stream, scene);
}

void deserialize_scene(tt_deserializer_t* stream, scene_state_t* scene,
                       char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS]) {
    if (!check_deserializer(stream)) { return; }

    char c;
    uint8_t l = 0;
    uint8_t p = 0;
    int8_t s = 99;
    uint8_t b = 0;
    int16_t num = 0;
    int16_t neg = 1;

    char input[32];
    memset(input, 0, sizeof(input));

    while (!stream->eof(stream->data) && s != -1) {
        c = toupper(stream->read_char(stream->data));
        // stream->print_dbg_char(c);

        if (c == '#') {
            if (!stream->eof(stream->data)) {
                c = toupper(stream->read_char(stream->data));
                // stream->print_dbg_char(c);

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

                if (!stream->eof(stream->data))
                    c = toupper(stream->read_char(stream->data));
            }
            else
                s = -1;

            // stream->print_dbg("\r\nsection: ");
            // stream->print_dbg_ulong(s);
        }
        // SCENE TEXT
        else if (s == 99) {
            if (c == '\n') {
                l++;
                p = 0;
            }
            else {
                if (l < SCENE_TEXT_LINES && p < SCENE_TEXT_CHARS) {
                    (*text)[l][p] = c;
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
                            ss_overwrite_script_command(scene, s, l, &temp);
                            l++;
                        }
                        else {
                            stream->print_dbg("\r\nvalidate: ");
                            stream->print_dbg(tele_error(status));
                            stream->print_dbg(" >> ");
                            stream->print_dbg("\r\nINPUT: ");
                            stream->print_dbg(input);
                        }
                    }
                    else {
                        stream->print_dbg("\r\nERROR: ");
                        stream->print_dbg(tele_error(status));
                        stream->print_dbg(" >> ");
                        stream->print_dbg("\r\nINPUT: ");
                        stream->print_dbg(input);
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
                        ss_set_pattern_val(scene, b, l - 4, neg * num);
                        // stream->print_dbg("\r\nset: ");
                        // stream->print_dbg_ulong(b);
                        // stream->print_dbg(" ");
                        // stream->print_dbg_ulong(l-4);
                        // stream->print_dbg(" ");
                        // stream->print_dbg_ulong(num);
                    }
                    else if (l == 0) { ss_set_pattern_len(scene, b, num); }
                    else if (l == 1) { ss_set_pattern_wrap(scene, b, num); }
                    else if (l == 2) { ss_set_pattern_start(scene, b, num); }
                    else if (l == 3) { ss_set_pattern_end(scene, b, num); }
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
                    // stream->print_dbg("\r\nnum: ");
                    // stream->print_dbg_ulong(num);
                }
                p++;
            }
        }
        // GRID
        else if (s == 11) { deserialize_grid(stream, scene, c); }
    }
}

bool check_serializer(tt_serializer_t* stream) {
    return (stream && stream->write_char && stream->write_buffer &&
            stream->print_dbg);
}

bool check_deserializer(tt_deserializer_t* stream) {
    return (stream && stream->read_char && stream->eof && stream->print_dbg);
}

void serialize_grid(tt_serializer_t* stream, scene_state_t* scene) {
    if (!check_serializer(stream)) { return; }

    char fvalue[36];

    stream->write_char(stream->data, '\n');
    stream->write_char(stream->data, '#');
    stream->write_char(stream->data, 'G');
    stream->write_char(stream->data, '\n');
    for (uint16_t i = 0; i < GRID_BUTTON_COUNT; i++) {
        stream->write_char(stream->data, '0' + scene->grid.button[i].state);
        if ((i & 15) == 15) stream->write_char(stream->data, '\n');
    }
    stream->write_char(stream->data, '\n');
    for (uint16_t i = 0; i < GRID_FADER_COUNT; i++) {
        itoa(scene->grid.fader[i].value, fvalue, 10);
        stream->write_buffer(stream->data, (uint8_t*)fvalue, strlen(fvalue));
        stream->write_char(stream->data, (i & 15) == 15 ? '\n' : '\t');
    }
}

void deserialize_grid(tt_deserializer_t* stream, scene_state_t* scene, char c) {
    if (!check_deserializer(stream)) { return; }

    if (grid_state == 0) {
        if (c >= '0' && c <= '9') {
            scene->grid.button[grid_count].state = c != '0';
            if (++grid_count >= GRID_BUTTON_COUNT) {
                grid_count = 0;
                grid_state = 1;
                if (!stream->eof(stream->data)) stream->read_char(stream->data);
                if (!stream->eof(stream->data))
                    stream->read_char(stream->data);  // eat \n\n
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
