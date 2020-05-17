#include "help_mode.h"

// this
#include "globals.h"
#include "keyboard_helper.h"

// libavr32
#include "font.h"
#include "region.h"

// asf
#include "conf_usb_host.h"  // needed in order to include "usb_protocol_hid.h"
#include "usb_protocol_hid.h"


////////////////////////////////////////////////////////////////////////////////
// Help text ///////////////////////////////////////////////////////////////////

#define HELP_PAGES 14

#define HELP1_LENGTH 68
const char* help1[HELP1_LENGTH] = { "1/14 HELP",
                                    "[ ] NAVIGATE HELP PAGES",
                                    "UP/DOWN TO SCROLL",
                                    "CTRL-F  SEARCH FORWARD",
                                    "CTRL-R  SEARCH REVERSE",
                                    " ",
                                    "TAB|EDIT/LIVE/PATTERN",
                                    "PRT SC|JUMP TO LIVE",
                                    "NUM LOCK|JUMP TO PATTERN",
                                    "F1-F10|EXECUTE SCRIPT",
                                    "ALT-F1-F10|EDIT SCRIPT",
                                    "CTRL-F1-F8|MUTE SCRIPT",
                                    "CTRL-F9|STOP/START METRO",
                                    "ESC|SCENE",
                                    "ALT-ESC|WRITE",
                                    " ",
                                    "ALT-X|CUT",
                                    "ALT-C|COPY",
                                    "ALT-V|PASTE",
                                    " ",
                                    "// LIVE",
                                    "ENTER|EXECUTE",
                                    "UP|PREVIOUS",
                                    "SH-BSP|CLEAR",
                                    "CTRL-L/R|JUMP WORDS",
                                    "~|TOGGLE VARS",
                                    "ALT-G|GRID VISUALIZER",
                                    "ALT-ARROWS|MOVE IN GRID",
                                    "ALT-SH-ARRS|SELECT AREA",
                                    "ALT-SPACE|PRESS IN GRID",
                                    "ALT-PRTSC|INSERT X Y W H",
                                    "ALT-/|CHANGE GRID PAGE",
                                    "ALT-\\|TOGGLE CONTROL VIEW",
                                    " ",
                                    "// EDIT",
                                    "[ ]|PREV, NEXT SCRIPT",
                                    "ENTER|ADD/OVERWRITE",
                                    "CTRL-Z|UNDO",
                                    "SH-ENTER|INSERT",
                                    "SH-BSP|CLEAR",
                                    "SH-UP/DOWN|SELECT LINES",
                                    "ALT-SLASH|DISABLE LINE(S)",
                                    "ALT-DELETE|DELETE LINE(S)",
                                    "ALT-UP/DN|MOVE LINE(S)",
                                    " ",
                                    "// PATTERN",
                                    "ARROWS|NAVIGATE",
                                    "ALT-ARROWS|JUMP",
                                    "0-9|NUMERIC ENTRY",
                                    "-|FLIP SIGN",
                                    "SPACE|TOGGLE 0/1",
                                    "ENTER|COMMIT CHANGE",
                                    "[ ]|NUDGE UP, DOWN",
                                    "ALT-[ ]|SEMITONE UP, DOWN",
                                    "CTRL-[ ]|FIFTH UP, DOWN",
                                    "SH-[ ]|OCTAVE UP, DOWN",
                                    "ALT-<0-9>|NUM SEMITONES UP",
                                    "// 0 = 10 and 1 = 11",
                                    "SH-ALT-<0-9>|NUM SEMITONES DOWN",
                                    "// 0 = 10 and 1 = 11",
                                    "SH-ALT-V|INSERT PASTE",
                                    "SH-BSP|DELETE",
                                    "SH-ENTER|DUPE INSERT",
                                    "SH-L|SET LENGTH",
                                    "SH-S|SET START",
                                    "SH-E|SET END",
                                    "ALT-L,S,E|JUMP",
                                    "SHIFT-2|SHOW/HIDE TURTLE" };

#define HELP2_LENGTH 14
const char* help2[HELP2_LENGTH] = { "2/14 VARIABLES",
                                    " ",
                                    "X, Y, Z|GENERAL PURPOSE",
                                    "T|USE FOR TIME",
                                    "A, B, C, D|ASSIGNED TO 1-4",
                                    " ",
                                    "// SPECIAL VARIABLES",
                                    "I|USED BY LOOP",
                                    "O|INCREMENTS ON READ",
                                    "DRUNK|INC BY -1, 0, +1",
                                    "Q|SHIFT REGISTER",
                                    "Q.N|SET Q LENGTH",
                                    "Q.AVG|AVERAGE OF ALL Q",
                                    "J, K|UNIQUE PER SCRIPT" };

#define HELP3_LENGTH 60
const char* help3[HELP3_LENGTH] = { "3/14 PARAMETERS",
                                    " ",
                                    "TR A-D|SET TR VALUE (0,1)",
                                    "TR.TIME A-D|TR PULSE TIME",
                                    "CV 1-4|CV TARGET VALUE",
                                    "CV.SLEW 1-4|CV SLEW TIME (MS)",
                                    "CV.SET 1-4|SET CV (NO SLEW)",
                                    "CV.OFF 1-4|ADD CV OFFSET",
                                    " ",
                                    "IN|GET IN JACK VAL",
                                    "IN.SCALE X Y",
                                    "    SCALE IN RANGE",
                                    "IN.CAL.MIN",
                                    "    CALIBRATE IN MIN",
                                    "IN.CAL.MAX",
                                    "    CALIBRATE IN MAX",
                                    "IN.CAL.RESET",
                                    "    RESET IN CALIBRATION",
                                    " ",
                                    "PARAM|GET KNOB VAL",
                                    "PARAM.SCALE X Y",
                                    "    SCALE KNOB RANGE",
                                    "PARAM.CAL.MIN",
                                    "    CALIBRATE KNOB MIN",
                                    "PARAM.CAL.MAX",
                                    "    CALIBRATE KNOB MAX",
                                    "PARAM.CAL.RESET",
                                    "    RESET KNOB CALIBRATION",
                                    " ",
                                    "M|METRO TIME (MS)",
                                    "M.ACT|ENABLE METRO (0/1)",
                                    "M.RESET|HARD RESET TIMER",
                                    " ",
                                    "TIME|TIMER COUNT (MS)",
                                    "TIME.ACT|ENABLE TIMER (0/1)",
                                    " ",
                                    "SCRIPT A|GET/RUN SCRIPT",
                                    "SCRIPT.POL",
                                    "   GET/SET ACTIVE SCRIPT EDGES",
                                    "   1 RISING, 2 FALLING, 3 BOTH",
                                    "SCENE|GET/SET SCENE #",
                                    "SCENE.G|SET SCENE, EXCL GRID",
                                    "LAST N|GET SCRIPT LAST RUN",
                                    " ",
                                    "// 16n FADERBANK OPS",
                                    "FADER X",
                                    "FB X",
                                    "   GET FADER X VAL",
                                    "FADER.SCALE X Y Z",
                                    "FB.S X Y Z",
                                    "   SCALE FADER X RANGE",
                                    "FADER.CAL.MIN X",
                                    "FB.C.MIN X",
                                    "   CALIBRATE FADER X MIN"
                                    "FADER.CAL.MAX X",
                                    "FB.C.MAX X",
                                    "   CALIBRATE FADER X MAX",
                                    "FADER.CAL.RESET X",
                                    "FB.C.R X",
                                    "   RESET FADER X CALIBRATION"};

#define HELP4_LENGTH 73
const char* help4[HELP4_LENGTH] = { "4/14 DATA AND TABLES",
                                    " ",
                                    "ALL PARAMS HAVE 16B RANGE",
                                    "-32768 TO 32767",
                                    " ",
                                    "// LOOKUP TABLES",
                                    "V 0-10|VOLT LOOKUP",
                                    "VV 0-1000|V WITH 2 DECIMALS",
                                    "BPM 2-MAX|MS PER BPM",
                                    "EXP X|EXPO LOOKUP",
                                    "N 0-127|CONVERT TO 1V/8VE",
                                    "// NOTE OPERATIONS",
                                    "SCALE NOTE OP - N.S",
                                    "N.S R S D",
                                    "    R = ROOT (0-127)",
                                    "    S = SCALE (0-8) *",
                                    "    D = DEGREE (0-6) *",
                                    "    * S & D WILL WRAP",
                                    "    NEGATIVE INDEXING OK",
                                    "   SCALES",
                                    "   0 = MAJOR",
                                    "   1 = NATURAL MINOR",
                                    "   2 = HARMONIC MINOR",
                                    "   3 = MELODIC MINOR",
                                    "   4 = DORIAN",
                                    "   5 = PHRYGIAN",
                                    "   6 = LYDIAN",
                                    "   7 = MYXOLIDIAN",
                                    "   8 = LOCRIAN",
                                    "   RETURNS N TABLE VALUE",
                                    "  ",
                                    "CHORD NOTE OP - N.C",
                                    "N.C R C D",
                                    "    R = ROOT (0-127)",
                                    "    C = CHORD (0-12) *",
                                    "    D = DEGREE (0-3) *",
                                    "    * C & D WILL WRAP",
                                    "    NEGATIVE INDEXING OK",
                                    "   CHORDS",
                                    "   0 = MAJ 7",
                                    "   1 = MIN 7",
                                    "   2 = DOM 7",
                                    "   3 = DIM 7",
                                    "   4 = AUG 7",
                                    "   5 = DOM 7b5",
                                    "   6 = MIN 7b5",
                                    "   7 = MAJ 7#5",
                                    "   8 = MIN MAJ 7",
                                    "   9 = DIM MAJ 7",
                                    "   10 = MAJ 6",
                                    "   11 = MIN 6",
                                    "   12 = 7 SUS 4",
                                    "   RETURNS N TABLE VALUE",
                                    "   ",
                                    "CHORD SCALE NOTE OP - N.CS",
                                    "N.CS R S D C",
                                    "     R = ROOT (0-127)",
                                    "     S = SCALE (0-8) *",
                                    "     D = SCALE DEGREE (0-6) *",
                                    "     C = CHORD DEGREE (0-3) *",
                                    "     * S, D, C WILL WRAP",
                                    "     NEGATIVE INDEXING OK",
                                    "  SCALES - SEE CHORDS ABOVE",
                                    "  MAJ = 0, 1, 1, 0, 2, 1, 6",
                                    "  NMI = 1, 6, 0, 1, 1, 0, 2",
                                    "  HMI = 8, 6, 7, 1, 2, 0, 3",
                                    "  MMI = 8, 1, 7, 2, 2, 6, 6",
                                    "  DOR = 1, 1, 0, 2, 1, 6, 0",
                                    "  PHR = 1, 0, 2, 1, 6, 0, 1",
                                    "  LYD = 0, 2, 1, 6, 0, 1, 1",
                                    "  LOC = 6, 0, 1, 1, 0, 2, 1",
                                    "  MYX = 2, 1, 6, 0, 1, 1, 0",
                                    "  RETURNS N TABLE VALUE",
};

#define HELP5_LENGTH 83
const char* help5[HELP5_LENGTH] = { "5/14 OPERATORS",
                                    " ",
                                    "RAND A|RANDOM 0 - A",
                                    "RRAND A B|RANDOM A - B",
                                    "TOSS|RANDOM 0 OR 1",
                                    " ",
                                    "CHAOS|CHAOTIC NUMBER",
                                    "CHAOS.R|CHAOS PARAMETER",
                                    "CHAOS.ALG|CHAOS ALGORITHM",
                                    " ",
                                    "AVG A B|AVERAGE OF INS",
                                    "MIN A B|LESSER OF INS",
                                    "MAX A B|GREATER OF INS",
                                    "ABS X|ABSOLUTE VALUE",
                                    "SGN X|SIGN FUNCTION",
                                    " ",
                                    "ADD A B|A + B",
                                    "SUB A B|A - B",
                                    "MUL A B|A * B",
                                    "DIV A B|A / B",
                                    "MOD A B|REMAINDER OF A / B",
                                    " ",
                                    "// LOGIC COMPARISONS",
                                    "RETURNS 1 (TRUE) OR 0 (FALSE)",
                                    "EQ A B|A EQUALS B",
                                    "NE A B|A NOT EQUAL TO B",
                                    "GT A B|A GREATER THAN B",
                                    "LT A B|A LESS THAN B",
                                    "EZ A|A EQUALS 0",
                                    "NZ A|A NOT EQUAL TO 0",
                                    "? A B C|TERNARY IF",
                                    " ",
                                    "RSH A B|BITSHIFT A RIGHT B",
                                    "LSH A B|BITSHIFT A LEFT B",
                                    "RROT A B|BITROTATE A >> B",
                                    "LROT A B|BITROTATE A << B",
                                    "| A B|BITWISE A OR B",
                                    "& A B|BITWISE A AND B",
                                    "^ A B|BITWISE A XOR B",
                                    "~ A|BITWISE NOT A",
                                    "BSET A B|SET BIT B OF A",
                                    "BGET A B|GET BIT B OF A",
                                    "BCLR A B|CLEAR BIT B OF A",
                                    " ",
                                    "LIM A B C|CLAMP A WITHIN B-C",
                                    "WRAP A B C|WRAP A AROUND B-C",
                                    "SCALE A B X Y I",
                                    "    SCALE I FROM A..B TO X..Y",
                                    "QT A B|QUANTIZE A TO B*X",
                                    " ",
                                    "// EUCLIDEAN OPERATOR",
                                    "ER F L I",
                                    "   F = FILL (1-32)",
                                    "   L = LENGTH (1-32)",
                                    "   I = STEP (ANY)",
                                    "   RETURNS 0 OR 1",
                                    " ",
                                    "// NUMERIC REPEATER",
                                    "NR P M F S",
                                    "   P = PRIME (0-31) *",
                                    "   M = & MASK (0-3) *",
                                    "   F = FACTOR (0-16) *",
                                    "   S = STEP (0-15) *",
                                    "   * P,M,F,S WILL WRAP",
                                    "   NEGATIVE INDEXING OK",
                                    "   MASKS",
                                    "   0 = NO MASK",
                                    "   1 = 0x0F0F",
                                    "   2 = 0xF003",
                                    "   3 = 0x1F0",
                                    "   RETURNS 0 OR 1",
                                    " ",
                                    "// SEED OPERATORS",
                                    "GET/SET RNG SEED",
                                    "SEED",
                                    "RAND.SEED|R, RAND, RRAND",
                                    "TOSS.SEED|TOSS",
                                    "PROB.SEED|PROB",
                                    "DRUNK.SEED|DRUNK",
                                    "P.SEED|PATTERN",
                                    "// SPECIAL OPERATORS",
                                    "TR.TOG X|FLIP STATE OF TR X",
                                    "TR.PULSE X|PULSE TR X" };

#define HELP6_LENGTH 38
const char* help6[HELP6_LENGTH] = { "6/14 PRE :",
                                    " ",
                                    "EACH PRE NEEDS A : FOLLOWED",
                                    "BY A COMMAND TO OPERATE ON",
                                    " ",
                                    "PROB A: | PROBABALITY 0-100",
                                    " ",
                                    "DEL A: |DELAY BY A (MS)",
                                    "DEL.CLR|KILL ALL DELAYS",
                                    "DEL.X A B: |A DELAYS",
                                    "  A NUMBER OF DELAYS",
                                    "  B TIME(MS) BETWEEN DELAYS",
                                    "DEL.R A B: |A-1 DELAYS",
                                    "  SAME AS DEL.X BUT",
                                    "  EXECUTE ONCE IMMEDIATELY",
                                    "  THEN A-1 TIMES",
                                    " ",
                                    "S: |PUSH ONTO STACK",
                                    "S.CLR|CLEAR S",
                                    "S.ALL|EXECUTE ALL S",
                                    "S.POP|EXECUTE NEWEST S",
                                    "S.L|QUERY LENGTH OF S",
                                    " ",
                                    "IF A: |EXECUTE IF NON-ZERO",
                                    "ELIF A: |AFTER IF WHEN TRUE",
                                    "ELSE: |AFTER FAILED IF",
                                    " ",
                                    "L A B: |ITERATE FROM A-B",
                                    "NB: I IS UPDATED EACH TIME",
                                    " ",
                                    "W X:|ITERATE WHILE X",
                                    " ",
                                    "EVERY X:|EXECUTE EACH X",
                                    "SKIP X:|EXECUTE EACH BUT X",
                                    "OTHER:|EXECUTE OTHERWISE",
                                    "SYNC X|SYNC TO STEP X",
                                    " ",
                                    "BREAK|STOP EXECUTION" };

#define HELP7_LENGTH 43
const char* help7[HELP7_LENGTH] = { "7/14 PATTERNS",
                                    " ",
                                    "// DIRECT ACCESS",
                                    "P A|GET VAL AT INDEX A",
                                    "P A B|SET VAL AT A TO B",
                                    "P.N A|SELECT BANK A",
                                    "PN A B|GET BANK A, IDX B",
                                    "PN A B C|BANK A, IDX B TO C",
                                    " ",
                                    "// CHANGES LENGTH",
                                    "P.INS A B|INSERT B AT IDX A",
                                    "P.RM A|DELETE ENTRY AT A",
                                    "P.PUSH A|ADD A TO END",
                                    "P.POP|RETURN & REM LAST",
                                    " ",
                                    "// ACTIVE RANGE",
                                    "P.L A|GET/SET LENGTH",
                                    "P.WRAP A|ENABLE WRAPPING",
                                    "P.START A|GET/SET START",
                                    "P.END A|GET/SET END",
                                    " ",
                                    "// READ HEAD",
                                    "P.I A|GET/SET POSITION",
                                    "P.HERE A|GET/SET VAL AT P.I",
                                    "P.NEXT A|GET/SET NEXT POS",
                                    "P.PREV A|GET/SET PREV POS",
                                    "P.RND A|GET RANDOM VALUE",
                                    "P.MIN A|GET MIN VALUE",
                                    "P.MAX A|GET MAX VALUE",
                                    " ",
                                    "// INCREMENT/DECREMENT",
                                    "P.+ A B|INC VALUE A BY B",
                                    "P.- A B|DEC VALUE A BY B",
                                    "P.+W A B C D",
                                    " |INC AND WRAP TO C..D",
                                    "P.-W A B C D",
                                    " |DEC AND WRAP TO C..D",
                                    " ",
                                    "// CHANGE WHOLE PATTERN",
                                    "P.SHUF|SHUFFLE",
                                    "P.REV|REVERSE",
                                    "P.ROT|ROTATE (NEG OK)",
                                    "P.MAP:|APPLY FUNC" };

#define HELP8_LENGTH 135
const char* help8[HELP8_LENGTH] = { "8/14 GRID",
                                    " ",
                                    "G.RST|RESET EVERYTHING",
                                    "G.CLR|CLEAR ALL LEDS",
                                    "G.DIM level|SET DIM LEVEL",
                                    "G.ROTATE 1/0",
                                    " |ROTATE GRID",
                                    "G.KEY x y action",
                                    " |EMULATE KEY PRESS",
                                    " ",
                                    "GROUPS",
                                    "G.GRP id|CURRENT GROUP",
                                    "G.GRP.EN id|ENABLE GROUP",
                                    "G.GRP.RST id|RESET GROUP",
                                    "G.GRP.SW id|SWITCH TO GROUP",
                                    "G.GRP.SC id|ASSIGN SCRIPT",
                                    "G.GRPI|GET LATEST GROUP",
                                    " ",
                                    "LEDS / RECTANGLES",
                                    "-1 DIM",
                                    "-2 BRIGHTEN",
                                    "-3 CLEAR",
                                    "G.LED x y l|DRAW DOT",
                                    "G.LED.C x y|CLEAR DOT",
                                    "G.REC x y w h fill border",
                                    "G.RCT x1 y1 x2 y2 fill border",
                                    "    DRAW RECTANGLE",
                                    " ",
                                    "BUTTONS",
                                    "G.BTN id x y w h ",
                                    "    latch level script",
                                    "G.GBT grp id x y w h",
                                    "    latch level script",
                                    "G.BTX id x y w h",
                                    "    lt lvl scr cols rows",
                                    "G.GBX grp id x y w h lt",
                                    "    lt lvl scr cols rows",
                                    " ",
                                    "G.BTN.EN id",
                                    "    ENABLE BUTTON",
                                    "G.BTN.X id|G.BTN.X id x",
                                    "    GET OR SET X COORDINATE",
                                    "G.BTN.Y id|G.BTN.Y id y",
                                    "    GET OR SET Y COORDINATE",
                                    "G.BTN.V id|G.BTN.V id val",
                                    "    GET OR SET VALUE",
                                    "G.BTN.L id G.BTN.L id level",
                                    "    GET OR SET LEVEL",
                                    "G.BTNI",
                                    "    ID OF LAST PRESSED",
                                    "G.BTNX|G.BTNX x",
                                    "    GET/SET X OF LAST PR",
                                    "G.BTNY|G.BTNY y",
                                    "    GET/SET Y OF LAST PR",
                                    "G.BTNV|G.BTNV value",
                                    "    GET/SET VALUE OF LAST PR",
                                    "G.BTNL G.BTNL level",
                                    "    GET/SET LEVEL OF LAST PRESSED",
                                    "G.BTN.SW id",
                                    "    SWITCH BUTTON",
                                    "G.BTN.PR id action",
                                    "    EMULATE BUTTON PRESS",
                                    "G.GBTN.V group value",
                                    "    SET VALUE FOR ALL IN GROUP",
                                    "G.GBTN.L grp odd_lvl even_lvl",
                                    "    SET LEVEL FOR ALL IN GROUP",
                                    "G.GBTN.C group",
                                    "    GET COUNT OF ALL PRESSED",
                                    "G.GBTN.I group index",
                                    "    GET IDS OF PRESSED BY INX",
                                    "G.GBTN.W group",
                                    "    GET PRESSED WIDTH",
                                    "G.GBTN.H group",
                                    "    GET PRESSED HEIGHT",
                                    "G.GBTN.X1 group",
                                    "    GET LEFTMOST PRESSED X",
                                    "G.GBTN.X2 group",
                                    "    GET RIGHTMOST PRESSED X",
                                    "G.GBTN.Y1 group",
                                    "    GET HIGHEST PRESSED",
                                    "G.GBTN.Y2 group",
                                    "    GET LOWEST PRESSED",
                                    " ",
                                    "FADERS",
                                    "0 COARSE HORIZ BAR",
                                    "1 COARSE VERT BAR",
                                    "2 COARSE HORIZ DOT",
                                    "3 COARSE VERT DOT",
                                    "4 FINE HORIZ BAR",
                                    "5 FINE VERT BAR",
                                    "6 FINE HORIZ DOT",
                                    "7 FINE VERT DOT",
                                    "G.FDR id x y w h",
                                    "    type level script",
                                    "G.GFD group id x y w h",
                                    "    type level script",
                                    "G.FDX id x y w h",
                                    "    type lvl scr cols rows",
                                    "G.GFX group id x y w h",
                                    "    type lvl scr cols rows",
                                    " ",
                                    "G.FDR.EN id|G.FDR.EN id 1/0",
                                    "    ENABLE FADER",
                                    "G.FDR.X id|G.FDR.X id x",
                                    "    GET OR SET X COORDINATE",
                                    "G.FDR.Y id|G.FDR.Y id y",
                                    "    GET OR SET Y COORDINATE",
                                    "G.FDR.V id|G.FDR.V id value",
                                    "    GET OR SET VALUE",
                                    "G.FDR.N id|G.FDR.N id value",
                                    "    GET OR SET VALUE (UNITS)",
                                    "G.FDR.L id|G.FDR.L id level",
                                    "    GET OR SET LEVEL",
                                    "G.FDRI",
                                    "    ID OF LAST PRESSED",
                                    "G.FDRX|G.FDRX x",
                                    "    GET/SET X OF LAST PRESSED",
                                    "G.FDRY|G.FDRY y",
                                    "    GET/SET Y OF LAST PRESSED",
                                    "G.FDRV|G.FDRV value",
                                    "    GET/SET VALUE OF LAST PR",
                                    "G.FDRN|G.FDRN value",
                                    "    AS ABOVE IN LED UNITS",
                                    "G.FDRL G.FDRL level",
                                    "    GET/SET LEVEL OF LAST PR",
                                    "G.FDR.PR id value",
                                    "    EMULATE FADER PRESS",
                                    "G.GFDR.V group value",
                                    "    SET VALUE FOR ALL IN GROUP",
                                    "G.GFDR.N group value",
                                    "    AS ABOVE IN LED UNITS",
                                    "G.GFDR.L grp odd_lvl even_lvl",
                                    "    SET LEVEL FOR ALL IN GROUP",
                                    "G.GFDR.RN group min max",
                                    "    SET FADER RANGE FOR .V" };
#define HELP9_LENGTH 17
const char* help9[HELP9_LENGTH] = { "9/14 TURTLE",
                                    " ",
                                    "// CRAWLS TRACKER DATA",
                                    "@|GET/SET DATA",
                                    "@X/@Y|GET/SET POSITION",
                                    "@F X1 Y1 X2 Y2",
                                    " |SET FENCE",
                                    "  OR @FX1/@FY1/@FX2/@FY2",
                                    "@BUMP 1|STOP AT FENCE",
                                    "@WRAP 1|WRAP AT FENCE",
                                    "@BOUNCE 1|BOUNCE OFF FENCE",
                                    "@MOVE X Y|MOVE RELATIVE",
                                    "@DIR 0-360|GET/SET DIRECTION",
                                    "@SPEED|GET/SET CENTICELLS",
                                    "@STEP|MOVE AT SPEED/DIR",
                                    "@SCRIPT N|GET/SET EDGE SCRIPT",
                                    "@SHOW 1/0|DISPLAY < ON TRACKER" };

#define HELP10_LENGTH 39
const char* help10[HELP10_LENGTH] = { "10/14 GENERIC I2C",
                                    " ",
                                    "IIA",
                                    "    GET I2C ADDRESS",
                                    "IIA ADDRESS",
                                    "    SET I2C ADDRESS",
                                      " ",
                                    "IIS CMD",
                                    "    SEND COMMAND",
                                    "IIS1 CMD VAL",
                                    "IIS2 CMD VAL1 VAL2",
                                    "IIS3 CMD VAL1 VAL2 VAL3",
                                    "    SEND COMMAND WITH PARAMS",
                                    "IISB1 CMD VAL",
                                    "IISB2 CMD VAL1 VAL2",
                                    "IISB3 CMD VAL1 VAL2 VAL3",
                                    "    SEND COMMAND BYTE PARAMS",
                                      " ",
                                    "IIQ CMD",
                                    "    SEND QUERY",
                                    "IIQ1 CMD VAL",
                                    "IIQ2 CMD VAL1 VAL2",
                                    "IIQ3 CMD VAL1 VAL2 VAL3",
                                    "    SEND QUERY WITH PARAMS",
                                    "IIQB1 CMD VAL",
                                    "IIQB2 CMD VAL1 VAL2",
                                    "IIQB3 CMD VAL1 VAL2 VAL3",
                                    "    SEND QUERY BYTE PARAMS",
                                      " ",
                                    "IIB CMD",
                                    "    SEND BYTE QUERY",
                                    "IIB1 CMD VAL",
                                    "IIB2 CMD VAL1 VAL2",
                                    "IIB3 CMD VAL1 VAL2 VAL3",
                                    "    BYTE QUERY WITH PARAMS",
                                    "IIBB1 CMD VAL",
                                    "IIBB2 CMD VAL1 VAL2",
                                    "IIBB3 CMD VAL1 VAL2 VAL3",
                                    "    BYTE QUERY BYTE PARAMS" };

#define HELP11_LENGTH 36
const char* help11[HELP11_LENGTH] = { "11/14 TELEX INPUT",
                                      " ",
                                      "TI.PARAM X|(TI.PRM)",
                                      "    GET KNOB VALUE",
                                      "TI.PARAM.QT X",
                                      "    KNOB VALUE QUANTIZED",
                                      "TI.PARAM.N X",
                                      "    KNOB AS SCALE NOTE",
                                      "TI.PARAM.SCALE X",
                                      "    SELECT KNOB QUANT SCALE",
                                      "TI.PARAM.MAP X Y Z",
                                      "    MAP KNOB VALUE TO Y..Z",
                                      "TI.IN X",
                                      "    GET INPUT VAL",
                                      "TI.IN.QT X",
                                      "    IN VALUE QUANTIZED",
                                      "TI.IN.N X",
                                      "    IN AS SCALE NOTE",
                                      "TI.IN.SCALE X",
                                      "    SELECT IN QUANT SCALE",
                                      "TI.IN.MAP X Y Z",
                                      "    MAP IN VALUE TO Y..Z",
                                      "TI.PARAM.INIT X",
                                      "    RESET KNOB X",
                                      "TI.IN.INIT X",
                                      "    RESET IN X",
                                      "TI.INIT D",
                                      "    RESET DEVICE D",
                                      "TI.PARAM.CALIB X Y",
                                      "    CALIBRATE KNOB X",
                                      "TI.IN.CALIB X Y",
                                      "    CALIBRATE IN X",
                                      "TI.STORE D",
                                      "    STORE CALIB FOR DEVICE D",
                                      "TI.RESET D",
                                      "    RESET CALIB FOR DEVICE D" };

#define HELP12_LENGTH 164
const char* help12[HELP12_LENGTH] = { "12/14 TELEX OUTPUT",
                                      " ",
                                      "TO.TR X Y",
                                      "    SET TR VALUE (0/1)",
                                      "TO.TR.TOG X",
                                      "    TOGGLE TR",
                                      "TO.TR.P X",
                                      "    PULSE TR",
                                      "TO.TR.P.DIV X Y",
                                      "    SET TR X CLOCK DIV TO Y",
                                      "TO.TR.P.MUTE X Y",
                                      "    MUTE TR X (0/1)",
                                      "TO.TR.TIME X Y",
                                      "    SET TR PULSE TIME (MS)",
                                      "TO.TR.TIME.S X Y",
                                      "    SET TR PULSE TIME (SEC)",
                                      "TO.TR.TIME.M",
                                      "    SET TR PULSE TIME (MIN)",
                                      "TO.TR.WIDTH X Y",
                                      "    SET TR DUTY (%)",
                                      "TO.TR.POL X Y",
                                      "    SET TR POLARITY",
                                      "TO.TR.M.ACT X Y",
                                      "    ENABLE METRO (0/1)",
                                      "TO.TR.M X Y",
                                      "    SET METRO RATE (MS)",
                                      "TO.TR.M.S X Y",
                                      "    SET METRO RATE (SEC)",
                                      "TO.TR.M.M X Y",
                                      "    SET METRO RATE (MIN)",
                                      "TO.TR.M.BPM X Y",
                                      "    SET METRO RATE (BPM)",
                                      "TO.TR.M.COUNT X Y",
                                      "    SET METRO REPEAT",
                                      "TO.TR.M.MUL X Y",
                                      "    SET METRO MULT",
                                      "TO.TR.M.SYNC X",
                                      "    SYNC METRO",
                                      "TO.M.ACT D Y",
                                      "    ENABLE DEVICE METRO (0/1)",
                                      "TO.M D Y",
                                      "    SET DEVICE METRO RATE (MS)",
                                      "TO.M.S D Y",
                                      "    AS ABOVE IN SECONDS",
                                      "TO.M.M D Y",
                                      "    AS ABOVE IN MINUTES",
                                      "TO.M.BPM D Y",
                                      "    AS ABOVE IN BPM",
                                      "TO.M.COUNT D Y",
                                      "    SET DEVICE METRO REPEAT",
                                      "TO.M.SYNC D",
                                      "   SYNC DEVICE METRO",
                                      "TO.CV X Y",
                                      "    SET CV WITH SLEW",
                                      "TO.CV.SLEW X Y",
                                      "    SET CV SLEW (MS)",
                                      "TO.CV.SLEW.S X Y",
                                      "    SET CV SLEW (SEC)",
                                      "TO.CV.SLEW.M",
                                      "    SET CV SLEW (MIN)",
                                      "TO.CV.SET X Y",
                                      "    SET CV WITH NO SLEW",
                                      "TO.CV.OFF X Y",
                                      "    SET CV OFFSET",
                                      "TO.CV.QT X Y",
                                      "    SET CV QUANTIZED",
                                      "TO.CV.QT.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.CV.N X Y",
                                      "    SET CV TO SCALE NOTE",
                                      "TO.CV.N.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.CV.LOG X Y",
                                      "    SET CV LOG MODE",
                                      "TO.OSC X Y",
                                      "    SET CV OSC FREQ",
                                      "TO.OSC.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.OSC.QT X Y",
                                      "    SET CV OSC QUANTIZED",
                                      "TO.OSC.QT.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.OSC.N X Y",
                                      "    SET CV OSC TO SCALE NOTE",
                                      "TO.OSC.N.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.OSC.FQ X Y",
                                      "    SET CV OSC (HZ)",
                                      "TO.OSC.LFO X Y",
                                      "    SET CV LFO RATE (MILLIHZ)",
                                      "TO.OSC.LFO.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.OSC.CYC X Y",
                                      "    SET CV OSC CYCLE (MS)",
                                      "TO.OSC.CYC.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.OSC.CYC.S X Y",
                                      "    SET CV OSC CYCLE (SEC)",
                                      "TO.OSC.CYC.S.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.OSC.CYC.M X Y",
                                      "    SET CV OSC CYCLE (MIN)",
                                      "TO.OSC.CYC.M.SET X Y",
                                      "    AS ABOVE NO SLEW",
                                      "TO.OSC.SCALE X Y",
                                      "    CV X QT SCALE Y",
                                      "TO.OSC.WAVE X Y",
                                      "    SELECT CV OSC WAVEFORM",
                                      "TO.OSC.RECT X Y",
                                      "    SET OSC RECTIFY (-2..2)",
                                      "TO.OSC.WIDTH X Y",
                                      "    SET OSC PULSEWIDTH",
                                      "TO.OSC.SYNC X",
                                      "    RESET OSC PHASE",
                                      "TO.OSC.PHASE X Y",
                                      "    SET OSC PHASE",
                                      "TO.OSC.SLEW X Y",
                                      "    SET OSC SLEW (MS)",
                                      "TO.OSC.SLEW.S X Y",
                                      "    SET OSC SLEW (SEC)",
                                      "TO.OSC.SLEW.M X Y",
                                      "    SET OSC SLEW (MIN)",
                                      "TO.OSC.CTR X Y",
                                      "    SET OSC CENTER",
                                      "TO.ENV.ACT X Y",
                                      "    ENABLE ENV (1/0)",
                                      "TO.ENV X Y",
                                      "    SET ENV GATE (1/0)",
                                      "TO.ENV.TRIG X Y",
                                      "    TRIGGER ENV",
                                      "TO.ENV.ATT X Y",
                                      "    SET ENV ATTACK (MS)",
                                      "TO.ENV.ATT.S X Y",
                                      "    SET ENV ATTACK (SEC)",
                                      "TO.ENV.ATT.M X Y",
                                      "    SET ENV ATTACK (MIN)",
                                      "TO.ENV.DEC X Y",
                                      "    SET ENV DECAY (MS)",
                                      "TO.ENV.DEC.S X Y",
                                      "    SET ENV DECAY (SEC)",
                                      "TO.ENV.DEC.M X Y",
                                      "    SET ENV DECAY (MIN)",
                                      "TO.ENV.EOR X N",
                                      "    PULSE TR N AT END OF RISE",
                                      "    TR ON SAME DEVICE",
                                      "TO.ENV.EOC X N",
                                      "    AS ABOVE FOR END OF CYCLE",
                                      "TO.ENV.LOOP X Y",
                                      "    LOOP ENV Y TIMES",
                                      "    0 TO LOOP INDEFINITELY",
                                      "TO.TR.INIT X",
                                      "    RESET TR",
                                      "TO.CV.INIT X",
                                      "    RESET CV",
                                      "TO.INIT D",
                                      "    RESET DEVICE",
                                      "TO.KILL D",
                                      "    CANCEL DEVICE TR PULSES",
                                      "    AND CV SLEW",
                                      "TO.CV.CALIB X",
                                      "    SAVE TO.CV.OFF X",
                                      "    AS CALIB VALUE",
                                      "TO.CV.RESET X",
                                      "    RESET CV CALIB" };

#define HELP13_LENGTH 140
const char* help13[HELP13_LENGTH] = { "13/14 ANSIBLE",
                                      " ",
                                      "ANS.G.LED X Y",
                                      "    READ GRID LED STATE",
                                      "    AT POS X, Y",
                                      "ANS.G X Y / ANS.G X Y Z",
                                      "    GET/SET GRID KEY STATE",
                                      "    AT POS X, Y, ON/OFF Z",
                                      "ANS.G.P X Y",
                                      "    SIMULATE GRID KEY PRESS",
                                      "    AT POS X, Y",
                                      "ANS.A.LED N X",
                                      "    READ ARC LED STATE",
                                      "    RING N, LED X CW",
                                      "ANS.A N D",
                                      "    SEND ARC ENCODER EVENT",
                                      "    RING N, DELTA D",
                                      "ANS.APP / ANS.APP X",
                                      "    GET/SET ACTIVE APP",
                                      "KR.PRE / KR.PRE X",
                                      "    RETURN/LOAD CURRENT PRESET",
                                      "KR.PERIOD / KR.PERIOD X",
                                      "    GET/SET INTERNAL PERIOD",
                                      "KR.PAT / KR.PAT X",
                                      "    GET/SET CURRENT PATTERN",
                                      "KR.SCALE / KR.SCALE X",
                                      "    GET/SET CURRENT SCALE",
                                      "KR.POS X Y / KR.POS X Y Z",
                                      "    GET/SET POSITION Z",
                                      "    FOR TRACK X PARAM Y",
                                      "KR.L.ST X Y",
                                      "    GET LOOP START FOR",
                                      "    TRACK X, PARAM Y",
                                      "KR.L.ST X Y Z",
                                      "    LOOP START FOR",
                                      "    TRACK X, PARAM Y TO Z",
                                      "KR.L.LEN X Y",
                                      "    GET LENGTH OF",
                                      "    TRACK X, PARAM Y",
                                      "KR.L.LEN X Y Z",
                                      "    SET LENGTH OF",
                                      "    TRACK X, PARAM Y TO Z",
                                      "KR.RES X Y",
                                      "    RESET POSITION",
                                      "    FOR TRACK X, PARAM Y",
                                      "KR.CV X",
                                      "    GET CV FOR CHANNEL X",
                                      "KR.MUTE X / KR.MUTE X Y",
                                      "    GET/SET MUTE STATE",
                                      "    FOR CHANNEL X",
                                      "KR.TRMUTE X",
                                      "    TOGGLE MUTE STATE",
                                      "    FOR CHANNEL X",
                                      "KR.CLK X",
                                      "    ADVANCE THE CLOCK",
                                      "    FOR CHANNEL X",
                                      "    (MUST BE ENABLED!)",
                                      "KR.CUE / KR.CUE X",
                                      "    GET/SET CUED PATTERN X",
                                      "KR.DIR / KR.DIR X",
                                      "    GET/SET STEP DIRECTION X",
                                      "KR.DUR X",
                                      "    GET CHANNEL X DURATION",
                                      "KR.PG / KR.PG X",
                                      "    GET/SET ACTIVE PAGE",
                                      "ME.PRE / ME.PRE X",
                                      "    RETURN/LOAD CURRENT PRESET",
                                      "ME.SCALE / ME.SCALE X",
                                      "    GET/SET CURRENT SCALE",
                                      "ME.PERIOD / ME.PERIOD X",
                                      "    GET/SET CLOCK PERIOD",
                                      "ME.STOP X",
                                      "    STOP CHANNEL X (0 = ALL)",
                                      "ME.RES X",
                                      "    RESET CHANNEL X (0 = ALL)",
                                      "    ALSO USED AS START",
                                      "ME.CV X",
                                      "    GET CV FOR CHANNEL X",
                                      "LV.PRE / LV.PRE X",
                                      "    RETURN/LOAD CURRENT PRESET",
                                      "LV.RES X",
                                      "    RESET (0 ON NEXT CLK,1 NOW)",
                                      "LV.POS / LV.POS X",
                                      "    GET/SET CURRENT POSITION",
                                      "LV.L.ST / LV.L.ST X",
                                      "    GET/SET LOOP START",
                                      "LV.L.LEN / LV.L.LEN X",
                                      "    GET/SET LOOP LENGTH",
                                      "LV.L.DIR / LV.L.DIR X",
                                      "    GET/SET LOOP DIRECTION",
                                      "LV.CV X",
                                      "    GET CV FOR CHANNEL X",
                                      "CY.PRE / CY.PRE X",
                                      "    RETURN/LOAD CURRENT PRESET",
                                      "CY.RES X",
                                      "    RESET CHANNEL X (0 = ALL)",
                                      "CY.POS X / CY.POS X Y",
                                      "    GET/SET CHANNEL POSITION",
                                      "    X = 0 TO SET ALL",
                                      "    POSITION BETWEEN 0-255",
                                      "CY.REV X",
                                      "    REVERSE CHANNEL X (0 = ALL)",
                                      "MID.SLEW T",
                                      "    SET PITCH SLEW TIME",
                                      "    TO T IN MS",
                                      "MID.SHIFT X",
                                      "    SHIFT PITCH BY TT PITCH X",
                                      "    (E.G. N 6, V -1)",
                                      "ARP.HLD X",
                                      "    0 DISABLES KEY HOLD",
                                      "    OTHER VALUES ENABLE",
                                      "ARP.STY X",
                                      "    SET ARP STYLE (0-7)",
                                      "ARP.GT V G",
                                      "    SET GATE LENGTH FOR VOICE V",
                                      "    TO G (0-127, SYNCED TO CLK)",
                                      "ARP.SLEW V T",
                                      "    SET SLEW TIME FOR VOICE V",
                                      "    TO T IN MS",
                                      "ARP.RPT V N S",
                                      "    SET VOICE PATTERN REPEAT",
                                      "    FOR VOICE V TO N TIMES",
                                      "    SHIFTED BY S SEMITONES",
                                      "ARP.DIV V D",
                                      "    SET VOICE CLOCK DIVISOR",
                                      "    FOR VOICE V TO D (1-32)",
                                      "ARP.FIL V F",
                                      "    SET VOICE EUCLIDEAN FILL",
                                      "    1 FOR STRAIGHT CLOCK (1-32)",
                                      "ARP.ROT V R",
                                      "    SET VOICE EUCLIDEAN",
                                      "    ROTATION (-32, 32)",
                                      "ARP.ER V F D R",
                                      "    SET ALL EUCLIDEAN RHYTHM",
                                      "ARP.RES V",
                                      "    RESET VOICE CLOCK/PATTERN",
                                      "    ON NEXT CLOCK TICK",
                                      "ARP.SHIFT V X",
                                      "    SHIFT VOICE CV BY TT PITCH",
                                      "    (E.G. N 6, V -1)" };

#define HELP14_LENGTH 53
const char* help14[HELP14_LENGTH] = { "14/14 JUST FRIENDS & W/",
                                      " ",
                                      "JF.TR X Y",
                                      "    TRIGGER CHANNEL X (0 = ALL)",
                                      "    WITH STATE Y (1 ON, 0 OFF)",
                                      "JF.RMODE X",
                                      "    SET RUN STATE OF JF",
                                      "    (0 = OFF, NON-ZERO = ON)",
                                      "JF.RUN X",
                                      "    SEND X TO RUN INPUT",
                                      "    (V -5 TO V 5)",
                                      "JF.SHIFT X",
                                      "    TRANSPOSE JF BY X",
                                      "JF.VTR X Y",
                                      "    LIKE JF.TR WITH VOLUME CTR",
                                      "    (V 0 TO V 5)",
                                      "JF.TUNE X Y Z",
                                      "    ADJUST TUNING OF CHANNEL X",
                                      "    SET NUMERATOR TO Y",
                                      "    SET DENOMINATOR TO Z",
                                      "JF.MODE X",
                                      "    NON-0 FOR ALTERNATE MODES",
                                      "JF.VOX X Y Z",
                                      "    CREATE NOTE AT CHANNEL X",
                                      "    Y = PITCH, Z = VELOCITY",
                                      "JF.NOTE X Y",
                                      "    ALLOCATED NOTE SEQUENCING",
                                      "    X = PITCH, Y = VELOCITY",
                                      "JF.GOD X",
                                      "    REDEFINE C3 TO GOD NOTE",
                                      "    (0: A=440HZ, 1: A=432HZ)",
                                      "JF.TICK X",
                                      "    SET TIMEBASE OF GEODE",
                                      "    1-48 TICKS PER MEASURE",
                                      "    49-255 BEATS PER MINUTE",
                                      "JF.QT X",
                                      "    SET QUANTIZATION",
                                      "    0 = NO QUANTIZATION",
                                      "    1-32 SETS SUBDIVISION",
                                      "WS.PLAY X",
                                      "    SET PLAYBACK STATE AND DIR",
                                      "    0 = STOP, 1 = FWD, -1 = REV",
                                      "WS.REC X",
                                      "    SET RECORDING MODE",
                                      "    0 PLAYBACK ONLY, 1 OVERDUB",
                                      "    -1 OVERWRITE",
                                      "WS.CUE X",
                                      "    MOVE TO CUEPOINT (RELATIVE)",
                                      "    0 RETRIGGER CURRENT CUE",
                                      "    1 JUMP TO NEXT CUE",
                                      "    -1 JUMP TO PREV CUE",
                                      "WS.LOOP X",
                                      "    SET LOOP STATE ON/OFF" };


////////////////////////////////////////////////////////////////////////////////
// Help mode ///////////////////////////////////////////////////////////////////

const char** help_pages[HELP_PAGES] = { help1,  help2,  help3, help4, help5,
                                        help6,  help7,  help8, help9, help10,
                                        help11, help12, help13, help14 };
const uint8_t help_length[HELP_PAGES] = {
    HELP1_LENGTH,  HELP2_LENGTH,  HELP3_LENGTH, HELP4_LENGTH, HELP5_LENGTH,
    HELP6_LENGTH,  HELP7_LENGTH,  HELP8_LENGTH, HELP9_LENGTH, HELP10_LENGTH,
    HELP11_LENGTH, HELP12_LENGTH, HELP13_LENGTH, HELP14_LENGTH
};

static uint8_t page_no;
static uint8_t offset;
static line_editor_t le;

typedef struct {
    int line;
    int ch;
} search_state_t;
typedef enum {
    SEARCH_MODE_NONE,
    SEARCH_MODE_FWD,
    SEARCH_MODE_REV,
} search_mode_t;
typedef enum {
    SEARCH_RESULT_NONE,
    SEARCH_RESULT_HIT,
    SEARCH_RESULT_MISS,
} search_result_t;
static search_mode_t search_mode;
static search_state_t search_state;
static search_result_t search_result;
static int prev_hit;

static bool dirty;

static bool text_search_forward(search_state_t* state, const char* needle,
                                const char** haystack, int haystack_len);
static bool text_search_reverse(search_state_t* state, const char* needle,
                                const char** haystack, int haystack_len);


void set_help_mode() {
    search_mode = SEARCH_MODE_NONE;
    search_result = SEARCH_RESULT_NONE;
    dirty = true;
}

bool text_search_forward(search_state_t* state, const char* needle,
                         const char** haystack, int haystack_len) {
    const int needle_len = strlen(needle);
    for (; state->line < haystack_len; state->line++) {
        const int haystack_line_len = strlen(haystack[state->line]);
        for (state->ch = 0; state->ch < haystack_line_len - needle_len;
             state->ch++) {
            if (!strncmp(needle, haystack[state->line] + state->ch, needle_len))
                return true;
        }
    }
    return false;
}

bool text_search_reverse(search_state_t* state, const char* needle,
                         const char** haystack, int haystack_len) {
    const int needle_len = strlen(needle);
    for (; state->line >= 0; state->line--) {
        const int haystack_line_len = strlen(haystack[state->line]);
        for (state->ch = haystack_line_len - needle_len; state->ch >= 0;
             state->ch--) {
            if (!strncmp(needle, haystack[state->line] + state->ch, needle_len))
                return true;
        }
    }
    return false;
}

void process_help_keys(uint8_t k, uint8_t m, bool is_held_key) {
    // <down> or C-n: line down
    if (match_no_mod(m, k, HID_DOWN) || match_ctrl(m, k, HID_N)) {
        if (offset < help_length[page_no] - (search_mode ? 7 : 8)) {
            offset++;
            dirty = true;
        }
    }
    // <up> or C-p: line up
    else if (match_no_mod(m, k, HID_UP) || match_ctrl(m, k, HID_P)) {
        if (offset) {
            offset--;
            dirty = true;
        }
    }
    // C-f or C-s: search forward
    else if (match_ctrl(m, k, HID_F) || match_ctrl(m, k, HID_S)) {
        search_result = SEARCH_RESULT_NONE;
        if (search_mode == SEARCH_MODE_FWD) { search_mode = SEARCH_MODE_NONE; }
        else {
            search_mode = SEARCH_MODE_FWD;
        }
        dirty = true;
    }
    // C-r: search in reverse
    else if (match_ctrl(m, k, HID_R)) {
        search_result = SEARCH_RESULT_NONE;
        if (search_mode == SEARCH_MODE_REV) { search_mode = SEARCH_MODE_NONE; }
        else {
            search_mode = SEARCH_MODE_REV;
        }
        dirty = true;
    }
    else if (search_mode != SEARCH_MODE_NONE) {
        if (match_no_mod(m, k, HID_ENTER)) {
            char* needle = line_editor_get(&le);
            if (!strlen(needle)) return;
            search_state.line = offset;

            switch (search_mode) {
                case SEARCH_MODE_FWD: {
                    if (search_result == SEARCH_RESULT_HIT) {
                        prev_hit = search_state.line;
                        if (search_state.line < help_length[page_no]) {
                            search_state.line++;
                        }
                        else {
                            search_result = SEARCH_RESULT_MISS;
                            dirty = true;
                            return;
                        }
                    }
                    for (int p = page_no; p < HELP_PAGES; p++) {
                        if (text_search_forward(&search_state, needle,
                                                help_pages[p],
                                                help_length[p])) {
                            search_result = SEARCH_RESULT_HIT;
                            page_no = p;
                            offset = search_state.line;
                            dirty = true;
                            return;
                        }
                        search_state.line = 0;
                    }
                    search_state.line = prev_hit;
                    search_result = SEARCH_RESULT_MISS;
                    dirty = true;
                    return;
                }
                case SEARCH_MODE_REV: {
                    if (search_result == SEARCH_RESULT_HIT) {
                        prev_hit = search_state.line;
                        if (search_state.line > 0) { search_state.line--; }
                        else {
                            search_result = SEARCH_RESULT_MISS;
                            dirty = true;
                            return;
                        }
                    }
                    for (int p = page_no; p >= 0; p--) {
                        if (text_search_reverse(&search_state, needle,
                                                help_pages[p],
                                                help_length[p])) {
                            search_result = SEARCH_RESULT_HIT;
                            page_no = p;
                            offset = search_state.line;
                            dirty = true;
                            return;
                        }
                        if (p > 0) { search_state.line = help_length[p - 1]; }
                    }
                    search_state.line = prev_hit;
                    search_result = SEARCH_RESULT_MISS;
                    dirty = true;
                    return;
                }
                default: break;
            }
        }
        else {
            search_result = SEARCH_RESULT_NONE;
            dirty = line_editor_process_keys(&le, k, m, is_held_key);
        }
    }
    else {
        // <left> or [: previous page
        if (match_no_mod(m, k, HID_LEFT) ||
            match_no_mod(m, k, HID_OPEN_BRACKET)) {
            if (page_no) {
                offset = 0;
                page_no--;
                dirty = true;
            }
        }
        // <right> or ]: next page
        else if (match_no_mod(m, k, HID_RIGHT) ||
                 match_no_mod(m, k, HID_CLOSE_BRACKET)) {
            if (page_no < HELP_PAGES - 1) {
                offset = 0;
                page_no++;
                dirty = true;
            }
        }
    }
}

uint8_t screen_refresh_help() {
    if (!dirty) { return 0; }

    uint8_t help_line_ct = 8;
    if (search_mode != SEARCH_MODE_NONE) help_line_ct--;
    if (search_result == SEARCH_RESULT_MISS) help_line_ct--;

    // clamp value of page_no
    if (page_no >= HELP_PAGES) page_no = HELP_PAGES - 1;

    // clamp value of offset
    if (offset >= help_length[page_no] - help_line_ct)
        offset = help_length[page_no] - help_line_ct;

    const char** text = help_pages[page_no];

    for (uint8_t y = 0; y < help_line_ct; y++) {
        if (search_result == SEARCH_RESULT_HIT &&
            (y + offset) == search_state.line) {
            region_fill(&line[y], 2);
            font_string_region_clip_tab(&line[y], text[y + offset], 2, 0, 0xa,
                                        2);
        }
        else {
            region_fill(&line[y], 0);
            font_string_region_clip_tab(&line[y], text[y + offset], 2, 0, 0xa,
                                        0);
        }
    }

    if (search_result == SEARCH_RESULT_MISS) {
        char s[36];
        strcpy(s, line_editor_get(&le));
        strcat(s, ": NOT FOUND");
        region_fill(&line[6], 0);
        font_string_region_clip(&line[6], s, 0, 0, 0x4, 0);
    }

    switch (search_mode) {
        case SEARCH_MODE_FWD: line_editor_draw(&le, '>', &line[7]); break;
        case SEARCH_MODE_REV: line_editor_draw(&le, '<', &line[7]); break;
        default: break;
    }

    dirty = false;
    return 0xFF;
};
