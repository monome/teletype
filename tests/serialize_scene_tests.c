#include "serialize_scene_tests.h"

#include <stdlib.h>
#include <string.h>

#include "greatest/greatest.h"
#include "log.h"
#include "ops/op_enum.h"
#include "scene_serialization.h"
#include "serializer.h"
#include "state.h"
#include "teletype.h"

void test_file_write_buffer(void* self_data, uint8_t* buffer, uint16_t size) {
    fwrite(buffer, 1, size, (FILE*)self_data);
}
void test_file_write_char(void* self_data, uint8_t c) {
    fputc(c, (FILE*)self_data);
}
void test_print_dbg(const char* c) {
    printf("%s\n", c);
}

uint16_t test_file_read_char(void* self_data) {
    return (uint16_t)fgetc((FILE*)self_data);
}
bool test_file_eof(void* self_data) {
    return feof((FILE*)self_data) != 0;
}

typedef struct {
    char* buffer;
    unsigned int length;
    unsigned int position;
} stringsource;

void test_string_write_buffer(void* self_data, uint8_t* buffer, uint16_t size) {
    stringsource* ss = (stringsource*)self_data;
    strncpy(ss->buffer + ss->position, (char*)buffer, size);
    ss->position += size;
    ss->length += size;
}
void test_string_write_char(void* self_data, uint8_t c) {
    stringsource* ss = (stringsource*)self_data;
    ss->buffer[ss->position] = c;
    ss->position += 1;
    ss->length += 1;
}
uint16_t test_string_read_char(void* self_data) {
    stringsource* ss = (stringsource*)self_data;
    if (ss->position < ss->length) {
        char r = ss->buffer[ss->position];
        ss->position += 1;
        return r;
    }
    else { return -1; }
}
bool test_string_eof(void* self_data) {
    stringsource* ss = (stringsource*)self_data;
    return (ss->position >= ss->length);
}

tt_serializer_t test_file_writer, test_string_writer;
tt_deserializer_t test_file_reader, test_string_reader;

void init_serializers() {
    test_file_writer.write_buffer = &test_file_write_buffer;
    test_file_writer.write_char = &test_file_write_char;
    test_file_writer.print_dbg = &test_print_dbg;

    test_file_reader.read_char = &test_file_read_char;
    test_file_reader.eof = &test_file_eof;
    test_file_reader.print_dbg = &test_print_dbg;

    test_string_writer.write_buffer = &test_string_write_buffer;
    test_string_writer.write_char = &test_string_write_char;
    test_string_writer.print_dbg = &test_print_dbg;

    test_string_reader.read_char = &test_string_read_char;
    test_string_reader.eof = &test_string_eof;
    test_string_reader.print_dbg = &test_print_dbg;
}

void deserialize_fragment(char* fragment, scene_state_t* scene,
                          char (*text)[SCENE_TEXT_LINES][SCENE_TEXT_CHARS]) {
    stringsource ss;
    ss.buffer = fragment;
    ss.length = strlen(fragment);
    ss.position = 0;

    test_string_reader.data = (void*)&ss;

    deserialize_scene(&test_string_reader, scene, text);
}

int compare_files(char* filename, FILE* a, FILE* b) {
    fseek(a, 0, 0);
    fseek(b, 0, 0);

    int line = 1;

    while (!feof(a) && !feof(b)) {
        char ca = fgetc(a);
        char cb = fgetc(b);
        if (ca != cb) {
            lprintf("At %s line %d, expected '%c', got '%c'", filename, line,
                    ca, cb);
            return -1;
        }
        if (ca == '\n') { line++; }
    }

    if (feof(a) != feof(b)) { return -1; }

    return 0;
}

TEST test_round_trip_file(char* filename, char* tempfile) {
    scene_state_t scene;
    ss_init(&scene);

    char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
    memset(text, 0, SCENE_TEXT_LINES * SCENE_TEXT_CHARS);

    for (int i = 0; i < 9; i++) {
        FILE* infile = fopen(filename, "rb");
        ASSERT(infile != 0);
        test_file_reader.data = (void*)infile;

        deserialize_scene(&test_file_reader, &scene, &text);

        FILE* outfile = fopen(tempfile, "w+b");
        ASSERT(outfile != 0);
        test_file_writer.data = (void*)outfile;

        serialize_scene(&test_file_writer, &scene, &text);

        CHECK_CALL(compare_files(filename, infile, outfile));
    }

    PASS();
}

TEST test_deserialize_fragment_script_basic() {
    scene_state_t scene;
    ss_init(&scene);

    char text[SCENE_TEXT_LINES][SCENE_TEXT_CHARS];
    memset(text, 0, SCENE_TEXT_LINES * SCENE_TEXT_CHARS);

    deserialize_fragment("#1\nTR.P 4\n\n", &scene, &text);
    // confirm we parsed a command with OP and NUMBER
    ASSERT(scene.scripts[0].c[0].length == 2);
    ASSERT(scene.scripts[0].c[0].data[0].tag == OP);
    ASSERT(scene.scripts[0].c[0].data[0].value == E_OP_TR_P);
    ASSERT(scene.scripts[0].c[0].data[1].tag == NUMBER);
    ASSERT(scene.scripts[0].c[0].data[1].value == 4);
    // confirm only one command in script
    ASSERT(scene.scripts[0].c[1].length == 0);
    // confirm rest of scripts are empty
    for (int i = 1; i < 11; i++) { ASSERT(scene.scripts[i].c[0].length == 0); }
    // confirm no text
    ASSERT(text[0][0] == 0);

    PASS();
}

SUITE(serialize_scene_suite) {
    log_init();
    init_serializers();
    RUN_TESTp(test_round_trip_file, "../presets/tt00.txt",
              "./test_output/tt00.txt");
    RUN_TESTp(test_round_trip_file, "../presets/tt01.txt",
              "./test_output/tt01.txt");
    RUN_TESTp(test_round_trip_file, "../presets/tt02.txt",
              "./test_output/tt02.txt");
    RUN_TESTp(test_round_trip_file, "../presets/tt03.txt",
              "./test_output/tt03.txt");
    RUN_TESTp(test_round_trip_file, "../presets/tt04.txt",
              "./test_output/tt04.txt");
    RUN_TESTp(test_round_trip_file, "../presets/tt05.txt",
              "./test_output/tt05.txt");
    RUN_TESTp(test_round_trip_file, "../presets/tt06.txt",
              "./test_output/tt06.txt");
    RUN_TESTp(test_round_trip_file, "../presets/tt07.txt",
              "./test_output/tt07.txt");
    RUN_TESTp(test_round_trip_file, "../presets/tt08.txt",
              "./test_output/tt08.txt");
    RUN_TEST(test_deserialize_fragment_script_basic);
    log_print();
}
