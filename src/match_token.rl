#include "match_token.h"
#
#include <ctype.h>   // isdigit
#include <stdlib.h>  // rand, strtol

#include <stdio.h>

#include "ops/op.h"
#include "ops/op_enum.h"

%%{
    machine match_token; # declare our ragel machine

    number = '-'? digit+;

    main := |*
        # NUMBERS
        number        => { MATCH_NUMBER() };

        # OPS
        # variables
        "A"           => { MATCH_OP(E_OP_A); };
        "B"           => { MATCH_OP(E_OP_B); };
        "C"           => { MATCH_OP(E_OP_C); };
        "D"           => { MATCH_OP(E_OP_D); };
        "DRUNK"       => { MATCH_OP(E_OP_DRUNK); };
        "DRUNK.MAX"   => { MATCH_OP(E_OP_DRUNK_MAX); };
        "DRUNK.MIN"   => { MATCH_OP(E_OP_DRUNK_MIN); };
        "DRUNK.WRAP"  => { MATCH_OP(E_OP_DRUNK_WRAP); };
        "FLIP"        => { MATCH_OP(E_OP_FLIP); };
        "I"           => { MATCH_OP(E_OP_I); };
        "O"           => { MATCH_OP(E_OP_O); };
        "O.INC"       => { MATCH_OP(E_OP_O_INC); };
        "O.MAX"       => { MATCH_OP(E_OP_O_MAX); };
        "O.MIN"       => { MATCH_OP(E_OP_O_MIN); };
        "O.WRAP"      => { MATCH_OP(E_OP_O_WRAP); };
        "T"           => { MATCH_OP(E_OP_T); };
        "TIME"        => { MATCH_OP(E_OP_TIME); };
        "TIME.ACT"    => { MATCH_OP(E_OP_TIME_ACT); };
        "LAST"        => { MATCH_OP(E_OP_LAST); };
        "X"           => { MATCH_OP(E_OP_X); };
        "Y"           => { MATCH_OP(E_OP_Y); };
        "Z"           => { MATCH_OP(E_OP_Z); };
        "J"           => { MATCH_OP(E_OP_J); };
        "K"           => { MATCH_OP(E_OP_K); };

        # init
        "INIT"            => { MATCH_OP(E_OP_INIT); };
        "INIT.SCENE"      => { MATCH_OP(E_OP_INIT_SCENE); };
        "INIT.SCRIPT"     => { MATCH_OP(E_OP_INIT_SCRIPT); };
        "INIT.SCRIPT.ALL" => { MATCH_OP(E_OP_INIT_SCRIPT_ALL); };
        "INIT.P"          => { MATCH_OP(E_OP_INIT_P); };
        "INIT.P.ALL"      => { MATCH_OP(E_OP_INIT_P_ALL); };
        "INIT.CV"         => { MATCH_OP(E_OP_INIT_CV); };
        "INIT.CV.ALL"     => { MATCH_OP(E_OP_INIT_CV_ALL); };
        "INIT.TR"         => { MATCH_OP(E_OP_INIT_TR); };
        "INIT.TR.ALL"     => { MATCH_OP(E_OP_INIT_TR_ALL); };
        "INIT.DATA"       => { MATCH_OP(E_OP_INIT_DATA); };
        "INIT.TIME"       => { MATCH_OP(E_OP_INIT_TIME); };

        # turtle
        "@"           => { MATCH_OP(E_OP_TURTLE); };
        "@X"          => { MATCH_OP(E_OP_TURTLE_X); };
        "@Y"          => { MATCH_OP(E_OP_TURTLE_Y); };
        "@MOVE"       => { MATCH_OP(E_OP_TURTLE_MOVE); };
        "@F"          => { MATCH_OP(E_OP_TURTLE_F); };
        "@FX1"        => { MATCH_OP(E_OP_TURTLE_FX1); };
        "@FY1"        => { MATCH_OP(E_OP_TURTLE_FY1); };
        "@FX2"        => { MATCH_OP(E_OP_TURTLE_FX2); };
        "@FY2"        => { MATCH_OP(E_OP_TURTLE_FY2); };
        "@SPEED"      => { MATCH_OP(E_OP_TURTLE_SPEED); };
        "@DIR"        => { MATCH_OP(E_OP_TURTLE_DIR); };
        "@STEP"       => { MATCH_OP(E_OP_TURTLE_STEP); };
        "@BUMP"       => { MATCH_OP(E_OP_TURTLE_BUMP); };
        "@WRAP"       => { MATCH_OP(E_OP_TURTLE_WRAP); };
        "@BOUNCE"     => { MATCH_OP(E_OP_TURTLE_BOUNCE); };
        "@SCRIPT"     => { MATCH_OP(E_OP_TURTLE_SCRIPT); };
        "@SHOW"       => { MATCH_OP(E_OP_TURTLE_SHOW); };

        # metronome
        "M"           => { MATCH_OP(E_OP_M); };
        "M!"          => { MATCH_OP(E_OP_M_SYM_EXCLAMATION); };
        "M.ACT"       => { MATCH_OP(E_OP_M_ACT); };
        "M.RESET"     => { MATCH_OP(E_OP_M_RESET); };

        # patterns
        "P.N"         => { MATCH_OP(E_OP_P_N); };
        "P"           => { MATCH_OP(E_OP_P); };
        "PN"          => { MATCH_OP(E_OP_PN); };
        "P.L"         => { MATCH_OP(E_OP_P_L); };
        "PN.L"        => { MATCH_OP(E_OP_PN_L); };
        "P.WRAP"      => { MATCH_OP(E_OP_P_WRAP); };
        "PN.WRAP"     => { MATCH_OP(E_OP_PN_WRAP); };
        "P.START"     => { MATCH_OP(E_OP_P_START); };
        "PN.START"    => { MATCH_OP(E_OP_PN_START); };
        "P.END"       => { MATCH_OP(E_OP_P_END); };
        "PN.END"      => { MATCH_OP(E_OP_PN_END); };
        "P.I"         => { MATCH_OP(E_OP_P_I); };
        "PN.I"        => { MATCH_OP(E_OP_PN_I); };
        "P.HERE"      => { MATCH_OP(E_OP_P_HERE); };
        "PN.HERE"     => { MATCH_OP(E_OP_PN_HERE); };
        "P.NEXT"      => { MATCH_OP(E_OP_P_NEXT); };
        "PN.NEXT"     => { MATCH_OP(E_OP_PN_NEXT); };
        "P.PREV"      => { MATCH_OP(E_OP_P_PREV); };
        "PN.PREV"     => { MATCH_OP(E_OP_PN_PREV); };
        "P.INS"       => { MATCH_OP(E_OP_P_INS); };
        "PN.INS"      => { MATCH_OP(E_OP_PN_INS); };
        "P.RM"        => { MATCH_OP(E_OP_P_RM); };
        "PN.RM"       => { MATCH_OP(E_OP_PN_RM); };
        "P.PUSH"      => { MATCH_OP(E_OP_P_PUSH); };
        "PN.PUSH"     => { MATCH_OP(E_OP_PN_PUSH); };
        "P.POP"       => { MATCH_OP(E_OP_P_POP); };
        "PN.POP"      => { MATCH_OP(E_OP_PN_POP); };
        "P.MIN"       => { MATCH_OP(E_OP_P_MIN); };
        "PN.MIN"      => { MATCH_OP(E_OP_PN_MIN); };
        "P.MAX"       => { MATCH_OP(E_OP_P_MAX); };
        "PN.MAX"      => { MATCH_OP(E_OP_PN_MAX); };
        "P.RND"       => { MATCH_OP(E_OP_P_RND); };
        "PN.RND"      => { MATCH_OP(E_OP_PN_RND); };
        "P.+"         => { MATCH_OP(E_OP_P_ADD); };
        "PN.+"        => { MATCH_OP(E_OP_PN_ADD); };
        "P.-"         => { MATCH_OP(E_OP_P_SUB); };
        "PN.-"        => { MATCH_OP(E_OP_PN_SUB); };
        "P.+W"        => { MATCH_OP(E_OP_P_ADDW); };
        "PN.+W"       => { MATCH_OP(E_OP_PN_ADDW); };
        "P.-W"        => { MATCH_OP(E_OP_P_SUBW); };
        "PN.-W"       => { MATCH_OP(E_OP_PN_SUBW); };

        # queue
        "Q"           => { MATCH_OP(E_OP_Q); };
        "Q.AVG"       => { MATCH_OP(E_OP_Q_AVG); };
        "Q.N"         => { MATCH_OP(E_OP_Q_N); };

        # hardware
        "CV"          => { MATCH_OP(E_OP_CV); };
        "CV.OFF"      => { MATCH_OP(E_OP_CV_OFF); };
        "CV.SLEW"     => { MATCH_OP(E_OP_CV_SLEW); };
        "IN"          => { MATCH_OP(E_OP_IN); };
        "IN.SCALE"    => { MATCH_OP(E_OP_IN_SCALE); };
        "IN.CAL.MIN"  => { MATCH_OP(E_OP_IN_CAL_MIN); };
        "IN.CAL.MAX"  => { MATCH_OP(E_OP_IN_CAL_MAX); };
        "IN.CAL.RESET" => { MATCH_OP(E_OP_IN_CAL_RESET); };
        "PARAM"       => { MATCH_OP(E_OP_PARAM); };
        "PARAM.SCALE" => { MATCH_OP(E_OP_PARAM_SCALE); };
        "PARAM.CAL.MIN"  => { MATCH_OP(E_OP_PARAM_CAL_MIN); };
        "PARAM.CAL.MAX"  => { MATCH_OP(E_OP_PARAM_CAL_MAX); };
        "PARAM.CAL.RESET" => { MATCH_OP(E_OP_PARAM_CAL_RESET); };
        "PRM"         => { MATCH_OP(E_OP_PRM); };
        "TR"          => { MATCH_OP(E_OP_TR); };
        "TR.POL"      => { MATCH_OP(E_OP_TR_POL); };
        "TR.TIME"     => { MATCH_OP(E_OP_TR_TIME); };
        "TR.TOG"      => { MATCH_OP(E_OP_TR_TOG); };
        "TR.PULSE"    => { MATCH_OP(E_OP_TR_PULSE); };
        "TR.P"        => { MATCH_OP(E_OP_TR_P); };
        "CV.SET"      => { MATCH_OP(E_OP_CV_SET); };
        "MUTE"        => { MATCH_OP(E_OP_MUTE); };
        "STATE"       => { MATCH_OP(E_OP_STATE); };
        "DEVICE.FLIP" => { MATCH_OP(E_OP_DEVICE_FLIP); };

        # maths
        "ADD"         => { MATCH_OP(E_OP_ADD); };
        "SUB"         => { MATCH_OP(E_OP_SUB); };
        "MUL"         => { MATCH_OP(E_OP_MUL); };
        "DIV"         => { MATCH_OP(E_OP_DIV); };
        "MOD"         => { MATCH_OP(E_OP_MOD); };
        "RAND"        => { MATCH_OP(E_OP_RAND); };
        "RND"         => { MATCH_OP(E_OP_RND); };
        "RRAND"       => { MATCH_OP(E_OP_RRAND); };
        "RRND"        => { MATCH_OP(E_OP_RRND); };
        "R"           => { MATCH_OP(E_OP_R); };
        "R.MIN"       => { MATCH_OP(E_OP_R_MIN); };
        "R.MAX"       => { MATCH_OP(E_OP_R_MAX); };
        "TOSS"        => { MATCH_OP(E_OP_TOSS); };
        "MIN"         => { MATCH_OP(E_OP_MIN); };
        "MAX"         => { MATCH_OP(E_OP_MAX); };
        "LIM"         => { MATCH_OP(E_OP_LIM); };
        "WRAP"        => { MATCH_OP(E_OP_WRAP); };
        "WRP"         => { MATCH_OP(E_OP_WRP); };
        "QT"          => { MATCH_OP(E_OP_QT); };
        "AVG"         => { MATCH_OP(E_OP_AVG); };
        "EQ"          => { MATCH_OP(E_OP_EQ); };
        "NE"          => { MATCH_OP(E_OP_NE); };
        "LT"          => { MATCH_OP(E_OP_LT); };
        "GT"          => { MATCH_OP(E_OP_GT); };
        "LTE"         => { MATCH_OP(E_OP_LTE); };
        "GTE"         => { MATCH_OP(E_OP_GTE); };
        "NZ"          => { MATCH_OP(E_OP_NZ); };
        "EZ"          => { MATCH_OP(E_OP_EZ); };
        "RSH"         => { MATCH_OP(E_OP_RSH); };
        "LSH"         => { MATCH_OP(E_OP_LSH); };
        "RROT"        => { MATCH_OP(E_OP_RROT); };
        "LROT"        => { MATCH_OP(E_OP_LROT); };
        "EXP"         => { MATCH_OP(E_OP_EXP); };
        "ABS"         => { MATCH_OP(E_OP_ABS); };
        "SGN"         => { MATCH_OP(E_OP_SGN); };
        "AND"         => { MATCH_OP(E_OP_AND); };
        "OR"          => { MATCH_OP(E_OP_OR); };
        "JI"          => { MATCH_OP(E_OP_JI); };
        "SCALE"       => { MATCH_OP(E_OP_SCALE); };
        "SCL"         => { MATCH_OP(E_OP_SCL); };
        "N"           => { MATCH_OP(E_OP_N); };
        "V"           => { MATCH_OP(E_OP_V); };
        "VV"          => { MATCH_OP(E_OP_VV); };
        "ER"          => { MATCH_OP(E_OP_ER); };
        "BPM"         => { MATCH_OP(E_OP_BPM);; };
        "|"           => { MATCH_OP(E_OP_BIT_OR);; };
        "&"           => { MATCH_OP(E_OP_BIT_AND);; };
        "~"           => { MATCH_OP(E_OP_BIT_NOT);; };
        "^"           => { MATCH_OP(E_OP_BIT_XOR);; };
        "BSET"        => { MATCH_OP(E_OP_BSET);; };
        "BGET"        => { MATCH_OP(E_OP_BGET);; };
        "BCLR"        => { MATCH_OP(E_OP_BCLR);; };
        "XOR"         => { MATCH_OP(E_OP_XOR); };
        "CHAOS"       => { MATCH_OP(E_OP_CHAOS); };
        "CHAOS.R"     => { MATCH_OP(E_OP_CHAOS_R); };
        "CHAOS.ALG"   => { MATCH_OP(E_OP_CHAOS_ALG); };
        "?"           => { MATCH_OP(E_OP_TIF); };
        "+"           => { MATCH_OP(E_OP_SYM_PLUS); };
        "-"           => { MATCH_OP(E_OP_SYM_DASH); };
        "*"           => { MATCH_OP(E_OP_SYM_STAR); };
        "/"           => { MATCH_OP(E_OP_SYM_FORWARD_SLASH); };
        "%"           => { MATCH_OP(E_OP_SYM_PERCENTAGE); };
        "=="          => { MATCH_OP(E_OP_SYM_EQUAL_x2); };
        "!="          => { MATCH_OP(E_OP_SYM_EXCLAMATION_EQUAL); };
        "<"           => { MATCH_OP(E_OP_SYM_LEFT_ANGLED); };
        ">"           => { MATCH_OP(E_OP_SYM_RIGHT_ANGLED); };
        "<="          => { MATCH_OP(E_OP_SYM_LEFT_ANGLED_EQUAL); };
        ">="          => { MATCH_OP(E_OP_SYM_RIGHT_ANGLED_EQUAL); };
        "!"           => { MATCH_OP(E_OP_SYM_EXCLAMATION); };
        "<<"          => { MATCH_OP(E_OP_SYM_LEFT_ANGLED_x2); };
        ">>"          => { MATCH_OP(E_OP_SYM_RIGHT_ANGLED_x2); };
        "<<<"         => { MATCH_OP(E_OP_SYM_LEFT_ANGLED_x3); };
        ">>>"         => { MATCH_OP(E_OP_SYM_RIGHT_ANGLED_x3); };
        "&&"          => { MATCH_OP(E_OP_SYM_AMPERSAND_x2); };
        "||"          => { MATCH_OP(E_OP_SYM_PIPE_x2); };

        # stack
        "S.ALL"       => { MATCH_OP(E_OP_S_ALL); };
        "S.POP"       => { MATCH_OP(E_OP_S_POP); };
        "S.CLR"       => { MATCH_OP(E_OP_S_CLR); };
        "S.L"         => { MATCH_OP(E_OP_S_L); };

        # controlflow
        "SCRIPT"      => { MATCH_OP(E_OP_SCRIPT); };
        "$"           => { MATCH_OP(E_OP_SYM_DOLLAR); };
        "SCRIPT.POL"  => { MATCH_OP(E_OP_SCRIPT_POL); };
        "$.POL"       => { MATCH_OP(E_OP_SYM_DOLLAR_POL); };
        "KILL"        => { MATCH_OP(E_OP_KILL); };
        "SCENE"       => { MATCH_OP(E_OP_SCENE); };
        "SCENE.G"     => { MATCH_OP(E_OP_SCENE_G); };
        "BREAK"       => { MATCH_OP(E_OP_BREAK); };
        "BRK"         => { MATCH_OP(E_OP_BRK); };
        "SYNC"        => { MATCH_OP(E_OP_SYNC); };

        # delay
        "DEL.CLR"     => { MATCH_OP(E_OP_DEL_CLR); };

        # whitewhale
        "WW.PRESET"   => { MATCH_OP(E_OP_WW_PRESET); };
        "WW.POS"      => { MATCH_OP(E_OP_WW_POS); };
        "WW.SYNC"     => { MATCH_OP(E_OP_WW_SYNC); };
        "WW.START"    => { MATCH_OP(E_OP_WW_START); };
        "WW.END"      => { MATCH_OP(E_OP_WW_END); };
        "WW.PMODE"    => { MATCH_OP(E_OP_WW_PMODE); };
        "WW.PATTERN"  => { MATCH_OP(E_OP_WW_PATTERN); };
        "WW.QPATTERN" => { MATCH_OP(E_OP_WW_QPATTERN); };
        "WW.MUTE1"    => { MATCH_OP(E_OP_WW_MUTE1); };
        "WW.MUTE2"    => { MATCH_OP(E_OP_WW_MUTE2); };
        "WW.MUTE3"    => { MATCH_OP(E_OP_WW_MUTE3); };
        "WW.MUTE4"    => { MATCH_OP(E_OP_WW_MUTE4); };
        "WW.MUTEA"    => { MATCH_OP(E_OP_WW_MUTEA); };
        "WW.MUTEB"    => { MATCH_OP(E_OP_WW_MUTEB); };

        # meadowphysics
        "MP.PRESET"   => { MATCH_OP(E_OP_MP_PRESET); };
        "MP.RESET"    => { MATCH_OP(E_OP_MP_RESET); };
        "MP.STOP"     => { MATCH_OP(E_OP_MP_STOP); };

        # earthsea
        "ES.PRESET"   => { MATCH_OP(E_OP_ES_PRESET); };
        "ES.MODE"     => { MATCH_OP(E_OP_ES_MODE); };
        "ES.CLOCK"    => { MATCH_OP(E_OP_ES_CLOCK); };
        "ES.RESET"    => { MATCH_OP(E_OP_ES_RESET); };
        "ES.PATTERN"  => { MATCH_OP(E_OP_ES_PATTERN); };
        "ES.TRANS"    => { MATCH_OP(E_OP_ES_TRANS); };
        "ES.STOP"     => { MATCH_OP(E_OP_ES_STOP); };
        "ES.TRIPLE"   => { MATCH_OP(E_OP_ES_TRIPLE); };
        "ES.MAGIC"    => { MATCH_OP(E_OP_ES_MAGIC); };

        # orca
        "OR.TRK"      => { MATCH_OP(E_OP_OR_TRK); };
        "OR.CLK"      => { MATCH_OP(E_OP_OR_CLK); };
        "OR.DIV"      => { MATCH_OP(E_OP_OR_DIV); };
        "OR.PHASE"    => { MATCH_OP(E_OP_OR_PHASE); };
        "OR.RST"      => { MATCH_OP(E_OP_OR_RST); };
        "OR.WGT"      => { MATCH_OP(E_OP_OR_WGT); };
        "OR.MUTE"     => { MATCH_OP(E_OP_OR_MUTE); };
        "OR.SCALE"    => { MATCH_OP(E_OP_OR_SCALE); };
        "OR.BANK"     => { MATCH_OP(E_OP_OR_BANK); };
        "OR.PRESET"   => { MATCH_OP(E_OP_OR_PRESET); };
        "OR.RELOAD"   => { MATCH_OP(E_OP_OR_RELOAD); };
        "OR.ROTS"     => { MATCH_OP(E_OP_OR_ROTS); };
        "OR.ROTW"     => { MATCH_OP(E_OP_OR_ROTW); };
        "OR.GRST"     => { MATCH_OP(E_OP_OR_GRST); };
        "OR.CVA"      => { MATCH_OP(E_OP_OR_CVA); };
        "OR.CVB"      => { MATCH_OP(E_OP_OR_CVB); };

        # ansible
        "ANS.G.LED"   => { MATCH_OP(E_OP_ANS_G_LED); };
        "ANS.G"       => { MATCH_OP(E_OP_ANS_G); };
        "ANS.G.P"     => { MATCH_OP(E_OP_ANS_G_P); };
        "ANS.A"       => { MATCH_OP(E_OP_ANS_A); };
        "ANS.A.LED"   => { MATCH_OP(E_OP_ANS_A_LED); };
        "ANS.APP"     => { MATCH_OP(E_OP_ANS_APP); };
        "KR.PRE"      => { MATCH_OP(E_OP_KR_PRE); };
        "KR.PAT"      => { MATCH_OP(E_OP_KR_PAT); };
        "KR.SCALE"    => { MATCH_OP(E_OP_KR_SCALE); };
        "KR.PERIOD"   => { MATCH_OP(E_OP_KR_PERIOD); };
        "KR.POS"      => { MATCH_OP(E_OP_KR_POS); };
        "KR.L.ST"     => { MATCH_OP(E_OP_KR_L_ST); };
        "KR.L.LEN"    => { MATCH_OP(E_OP_KR_L_LEN); };
        "KR.RES"      => { MATCH_OP(E_OP_KR_RES); };
        "KR.CV"       => { MATCH_OP(E_OP_KR_CV); };
        "KR.MUTE"     => { MATCH_OP(E_OP_KR_MUTE); };
        "KR.TMUTE"    => { MATCH_OP(E_OP_KR_TMUTE); };
        "KR.CLK"      => { MATCH_OP(E_OP_KR_CLK); };
        "KR.PG"       => { MATCH_OP(E_OP_KR_PG); };
        "KR.CUE"      => { MATCH_OP(E_OP_KR_CUE); };
        "KR.DIR"      => { MATCH_OP(E_OP_KR_DIR); };
        "KR.DUR"      => { MATCH_OP(E_OP_KR_DUR); };
        "ME.PRE"      => { MATCH_OP(E_OP_ME_PRE); };
        "ME.RES"      => { MATCH_OP(E_OP_ME_RES); };
        "ME.STOP"     => { MATCH_OP(E_OP_ME_STOP); };
        "ME.SCALE"    => { MATCH_OP(E_OP_ME_SCALE); };
        "ME.PERIOD"   => { MATCH_OP(E_OP_ME_PERIOD); };
        "ME.CV"       => { MATCH_OP(E_OP_ME_CV); };
        "LV.PRE"      => { MATCH_OP(E_OP_LV_PRE); };
        "LV.RES"      => { MATCH_OP(E_OP_LV_RES); };
        "LV.POS"      => { MATCH_OP(E_OP_LV_POS); };
        "LV.L.ST"     => { MATCH_OP(E_OP_LV_L_ST); };
        "LV.L.LEN"    => { MATCH_OP(E_OP_LV_L_LEN); };
        "LV.L.DIR"    => { MATCH_OP(E_OP_LV_L_DIR); };
        "LV.CV"       => { MATCH_OP(E_OP_LV_CV); };
        "CY.PRE"      => { MATCH_OP(E_OP_CY_PRE); };
        "CY.RES"      => { MATCH_OP(E_OP_CY_RES); };
        "CY.POS"      => { MATCH_OP(E_OP_CY_POS); };
        "CY.REV"      => { MATCH_OP(E_OP_CY_REV); };
        "CY.CV"       => { MATCH_OP(E_OP_CY_CV); };
        "MID.SHIFT"   => { MATCH_OP(E_OP_MID_SHIFT); };
        "MID.SLEW"    => { MATCH_OP(E_OP_MID_SLEW); };
        "ARP.STY"     => { MATCH_OP(E_OP_ARP_STY); };
        "ARP.HLD"     => { MATCH_OP(E_OP_ARP_HLD); };
        "ARP.RPT"     => { MATCH_OP(E_OP_ARP_RPT); };
        "ARP.GT"      => { MATCH_OP(E_OP_ARP_GT); };
        "ARP.DIV"     => { MATCH_OP(E_OP_ARP_DIV); };
        "ARP.RES"     => { MATCH_OP(E_OP_ARP_RES); };
        "ARP.SHIFT"   => { MATCH_OP(E_OP_ARP_SHIFT); };
        "ARP.SLEW"    => { MATCH_OP(E_OP_ARP_SLEW); };
        "ARP.FIL"     => { MATCH_OP(E_OP_ARP_FIL); };
        "ARP.ROT"     => { MATCH_OP(E_OP_ARP_ROT); };
        "ARP.ER"      => { MATCH_OP(E_OP_ARP_ER); };

        # justfriends
        "JF.TR"       => { MATCH_OP(E_OP_JF_TR); };
        "JF.RMODE"    => { MATCH_OP(E_OP_JF_RMODE); };
        "JF.RUN"      => { MATCH_OP(E_OP_JF_RUN); };
        "JF.SHIFT"    => { MATCH_OP(E_OP_JF_SHIFT); };
        "JF.VTR"      => { MATCH_OP(E_OP_JF_VTR); };
        "JF.MODE"     => { MATCH_OP(E_OP_JF_MODE); };
        "JF.TICK"     => { MATCH_OP(E_OP_JF_TICK); };
        "JF.VOX"      => { MATCH_OP(E_OP_JF_VOX); };
        "JF.NOTE"     => { MATCH_OP(E_OP_JF_NOTE); };
        "JF.GOD"      => { MATCH_OP(E_OP_JF_GOD); };
        "JF.TUNE"     => { MATCH_OP(E_OP_JF_TUNE); };
        "JF.QT"       => { MATCH_OP(E_OP_JF_QT); };

        # telex
        "TO.TR"            => { MATCH_OP(E_OP_TO_TR); };
        "TO.TR.TOG"        => { MATCH_OP(E_OP_TO_TR_TOG); };
        "TO.TR.PULSE"      => { MATCH_OP(E_OP_TO_TR_PULSE); };
        "TO.TR.TIME"       => { MATCH_OP(E_OP_TO_TR_TIME); };
        "TO.TR.TIME.S"     => { MATCH_OP(E_OP_TO_TR_TIME_S); };
        "TO.TR.TIME.M"     => { MATCH_OP(E_OP_TO_TR_TIME_M); };
        "TO.TR.POL"        => { MATCH_OP(E_OP_TO_TR_POL); };
        "TO.KILL"          => { MATCH_OP(E_OP_TO_KILL); };

        "TO.TR.PULSE.DIV"  => { MATCH_OP(E_OP_TO_TR_PULSE_DIV); };
        "TO.TR.PULSE.MUTE" => { MATCH_OP(E_OP_TO_TR_PULSE_MUTE); };
        "TO.TR.M.MUL"      => { MATCH_OP(E_OP_TO_TR_M_MUL); };

        "TO.M"             => { MATCH_OP(E_OP_TO_M); };
        "TO.M.S"           => { MATCH_OP(E_OP_TO_M_S); };
        "TO.M.M"           => { MATCH_OP(E_OP_TO_M_M); };
        "TO.M.BPM"         => { MATCH_OP(E_OP_TO_M_BPM); };
        "TO.M.ACT"         => { MATCH_OP(E_OP_TO_M_ACT); };
        "TO.M.SYNC"        => { MATCH_OP(E_OP_TO_M_SYNC); };
        "TO.M.COUNT"       => { MATCH_OP(E_OP_TO_M_COUNT); };

        "TO.TR.M"          => { MATCH_OP(E_OP_TO_TR_M); };
        "TO.TR.M.S"        => { MATCH_OP(E_OP_TO_TR_M_S); };
        "TO.TR.M.M"        => { MATCH_OP(E_OP_TO_TR_M_M); };
        "TO.TR.M.BPM"      => { MATCH_OP(E_OP_TO_TR_M_BPM); };
        "TO.TR.M.ACT"      => { MATCH_OP(E_OP_TO_TR_M_ACT); };
        "TO.TR.M.SYNC"     => { MATCH_OP(E_OP_TO_TR_M_SYNC); };
        "TO.TR.WIDTH"      => { MATCH_OP(E_OP_TO_TR_WIDTH); };
        "TO.TR.M.COUNT"    => { MATCH_OP(E_OP_TO_TR_M_COUNT); };

        "TO.CV"            => { MATCH_OP(E_OP_TO_CV); };
        "TO.CV.SLEW"       => { MATCH_OP(E_OP_TO_CV_SLEW); };
        "TO.CV.SLEW.S"     => { MATCH_OP(E_OP_TO_CV_SLEW_S); };
        "TO.CV.SLEW.M"     => { MATCH_OP(E_OP_TO_CV_SLEW_M); };
        "TO.CV.SET"        => { MATCH_OP(E_OP_TO_CV_SET); };
        "TO.CV.OFF"        => { MATCH_OP(E_OP_TO_CV_OFF); };
        "TO.CV.QT"         => { MATCH_OP(E_OP_TO_CV_QT); };
        "TO.CV.QT.SET"     => { MATCH_OP(E_OP_TO_CV_QT_SET); };
        "TO.CV.N"          => { MATCH_OP(E_OP_TO_CV_N); };
        "TO.CV.N.SET"      => { MATCH_OP(E_OP_TO_CV_N_SET); };
        "TO.CV.SCALE"      => { MATCH_OP(E_OP_TO_CV_SCALE); };
        "TO.CV.LOG"        => { MATCH_OP(E_OP_TO_CV_LOG); };

        "TO.CV.INIT"       => { MATCH_OP(E_OP_TO_CV_INIT); };
        "TO.TR.INIT"       => { MATCH_OP(E_OP_TO_TR_INIT); };
        "TO.INIT"          => { MATCH_OP(E_OP_TO_INIT); };

        "TO.TR.P"          => { MATCH_OP(E_OP_TO_TR_P); };
        "TO.TR.P.DIV"      => { MATCH_OP(E_OP_TO_TR_P_DIV); };
        "TO.TR.P.MUTE"     => { MATCH_OP(E_OP_TO_TR_P_MUTE); };

        "TO.OSC"           => { MATCH_OP(E_OP_TO_OSC); };
        "TO.OSC.SET"       => { MATCH_OP(E_OP_TO_OSC_SET); };
        "TO.OSC.QT"        => { MATCH_OP(E_OP_TO_OSC_QT); };
        "TO.OSC.QT.SET"    => { MATCH_OP(E_OP_TO_OSC_QT_SET); };
        "TO.OSC.FQ"        => { MATCH_OP(E_OP_TO_OSC_FQ); };
        "TO.OSC.FQ.SET"    => { MATCH_OP(E_OP_TO_OSC_FQ_SET); };
        "TO.OSC.N"         => { MATCH_OP(E_OP_TO_OSC_N); };
        "TO.OSC.N.SET"     => { MATCH_OP(E_OP_TO_OSC_N_SET); };
        "TO.OSC.LFO"       => { MATCH_OP(E_OP_TO_OSC_LFO); };
        "TO.OSC.LFO.SET"   => { MATCH_OP(E_OP_TO_OSC_LFO_SET); };
        "TO.OSC.WAVE"      => { MATCH_OP(E_OP_TO_OSC_WAVE); };
        "TO.OSC.SYNC"      => { MATCH_OP(E_OP_TO_OSC_SYNC); };
        "TO.OSC.PHASE"     => { MATCH_OP(E_OP_TO_OSC_PHASE); };
        "TO.OSC.WIDTH"     => { MATCH_OP(E_OP_TO_OSC_WIDTH); };
        "TO.OSC.RECT"      => { MATCH_OP(E_OP_TO_OSC_RECT); };
        "TO.OSC.SLEW"      => { MATCH_OP(E_OP_TO_OSC_SLEW); };
        "TO.OSC.SLEW.S"    => { MATCH_OP(E_OP_TO_OSC_SLEW_S); };
        "TO.OSC.SLEW.M"    => { MATCH_OP(E_OP_TO_OSC_SLEW_M); };
        "TO.OSC.SCALE"     => { MATCH_OP(E_OP_TO_OSC_SCALE); };
        "TO.OSC.CYC"       => { MATCH_OP(E_OP_TO_OSC_CYC); };
        "TO.OSC.CYC.S"     => { MATCH_OP(E_OP_TO_OSC_CYC_S); };
        "TO.OSC.CYC.M"     => { MATCH_OP(E_OP_TO_OSC_CYC_M); };
        "TO.OSC.CYC.SET"   => { MATCH_OP(E_OP_TO_OSC_CYC_SET); };
        "TO.OSC.CYC.S.SET" => { MATCH_OP(E_OP_TO_OSC_CYC_S_SET); };
        "TO.OSC.CYC.M.SET" => { MATCH_OP(E_OP_TO_OSC_CYC_M_SET); };
        "TO.OSC.CTR"       => { MATCH_OP(E_OP_TO_OSC_CTR); };

        "TO.ENV.ACT"       => { MATCH_OP(E_OP_TO_ENV_ACT); };
        "TO.ENV.ATT"       => { MATCH_OP(E_OP_TO_ENV_ATT); };
        "TO.ENV.ATT.S"     => { MATCH_OP(E_OP_TO_ENV_ATT_S); };
        "TO.ENV.ATT.M"     => { MATCH_OP(E_OP_TO_ENV_ATT_M); };
        "TO.ENV.DEC"       => { MATCH_OP(E_OP_TO_ENV_DEC); };
        "TO.ENV.DEC.S"     => { MATCH_OP(E_OP_TO_ENV_DEC_S); };
        "TO.ENV.DEC.M"     => { MATCH_OP(E_OP_TO_ENV_DEC_M); };
        "TO.ENV.TRIG"      => { MATCH_OP(E_OP_TO_ENV_TRIG); };

        "TO.ENV.EOR"       => { MATCH_OP(E_OP_TO_ENV_EOR); };
        "TO.ENV.EOC"       => { MATCH_OP(E_OP_TO_ENV_EOC); };
        "TO.ENV.LOOP"      => { MATCH_OP(E_OP_TO_ENV_LOOP); };

        "TO.ENV"           => { MATCH_OP(E_OP_TO_ENV); };
        "TO.CV.CALIB"      => { MATCH_OP(E_OP_TO_CV_CALIB); };
        "TO.CV.RESET"      => { MATCH_OP(E_OP_TO_CV_RESET); };

        "TI.PARAM"         => { MATCH_OP(E_OP_TI_PARAM); };
        "TI.PARAM.QT"      => { MATCH_OP(E_OP_TI_PARAM_QT); };
        "TI.PARAM.N"       => { MATCH_OP(E_OP_TI_PARAM_N); };
        "TI.PARAM.SCALE"   => { MATCH_OP(E_OP_TI_PARAM_SCALE); };
        "TI.PARAM.MAP"     => { MATCH_OP(E_OP_TI_PARAM_MAP); };
        "TI.IN"            => { MATCH_OP(E_OP_TI_IN); };
        "TI.IN.QT"         => { MATCH_OP(E_OP_TI_IN_QT); };
        "TI.IN.N"          => { MATCH_OP(E_OP_TI_IN_N); };
        "TI.IN.SCALE"      => { MATCH_OP(E_OP_TI_IN_SCALE); };
        "TI.IN.MAP"        => { MATCH_OP(E_OP_TI_IN_MAP); };
        "TI.PARAM.CALIB"   => { MATCH_OP(E_OP_TI_PARAM_CALIB); };
        "TI.IN.CALIB"      => { MATCH_OP(E_OP_TI_IN_CALIB); };
        "TI.STORE"         => { MATCH_OP(E_OP_TI_STORE); };
        "TI.RESET"         => { MATCH_OP(E_OP_TI_RESET); };

        "TI.PARAM.INIT"    => { MATCH_OP(E_OP_TI_PARAM_INIT); };
        "TI.IN.INIT"       => { MATCH_OP(E_OP_TI_IN_INIT); };
        "TI.INIT"          => { MATCH_OP(E_OP_TI_INIT); };

        "TI.PRM"           => { MATCH_OP(E_OP_TI_PRM); };
        "TI.PRM.QT"        => { MATCH_OP(E_OP_TI_PRM_QT); };
        "TI.PRM.N"         => { MATCH_OP(E_OP_TI_PRM_N); };
        "TI.PRM.SCALE"     => { MATCH_OP(E_OP_TI_PRM_SCALE); };
        "TI.PRM.MAP"       => { MATCH_OP(E_OP_TI_PRM_MAP); };
        "TI.PRM.INIT"      => { MATCH_OP(E_OP_TI_PRM_INIT); };

        # fader
        "FADER"            => { MATCH_OP(E_OP_FADER); };
        "FB"               => { MATCH_OP(E_OP_FB); };

        # ER301
        "SC.TR"            => { MATCH_OP(E_OP_SC_TR); };
        "SC.TR.TOG"        => { MATCH_OP(E_OP_SC_TR_TOG); };
        "SC.TR.PULSE"      => { MATCH_OP(E_OP_SC_TR_PULSE); };
        "SC.TR.TIME"       => { MATCH_OP(E_OP_SC_TR_TIME); };
        "SC.TR.POL"        => { MATCH_OP(E_OP_SC_TR_POL); };

        "SC.TR.P"          => { MATCH_OP(E_OP_SC_TR_P); };

        "SC.CV"            => { MATCH_OP(E_OP_SC_CV); };
        "SC.CV.SLEW"       => { MATCH_OP(E_OP_SC_CV_SLEW); };
        "SC.CV.SET"        => { MATCH_OP(E_OP_SC_CV_SET); };
        "SC.CV.OFF"        => { MATCH_OP(E_OP_SC_CV_OFF); };

        # grid
        "G.RST"            => { MATCH_OP(E_OP_G_RST); };
        "G.CLR"            => { MATCH_OP(E_OP_G_CLR); };
        "G.ROTATE"         => { MATCH_OP(E_OP_G_ROTATE); };
        "G.DIM"            => { MATCH_OP(E_OP_G_DIM); };
        "G.KEY"            => { MATCH_OP(E_OP_G_KEY); };

        "G.GRP"            => { MATCH_OP(E_OP_G_GRP); };
        "G.GRP.EN"         => { MATCH_OP(E_OP_G_GRP_EN); };
        "G.GRP.RST"        => { MATCH_OP(E_OP_G_GRP_RST); };
        "G.GRP.SW"         => { MATCH_OP(E_OP_G_GRP_SW); };
        "G.GRP.SC"         => { MATCH_OP(E_OP_G_GRP_SC); };
        "G.GRPI"           => { MATCH_OP(E_OP_G_GRPI); };

        "G.LED"            => { MATCH_OP(E_OP_G_LED); };
        "G.LED.C"          => { MATCH_OP(E_OP_G_LED_C); };
        "G.REC"            => { MATCH_OP(E_OP_G_REC); };
        "G.RCT"            => { MATCH_OP(E_OP_G_RCT); };

        "G.BTN"            => { MATCH_OP(E_OP_G_BTN); };
        "G.BTX"            => { MATCH_OP(E_OP_G_BTX); };
        "G.GBT"            => { MATCH_OP(E_OP_G_GBT); };
        "G.GBX"            => { MATCH_OP(E_OP_G_GBX); };
        "G.BTN.EN"         => { MATCH_OP(E_OP_G_BTN_EN); };
        "G.BTN.V"          => { MATCH_OP(E_OP_G_BTN_V); };
        "G.BTN.L"          => { MATCH_OP(E_OP_G_BTN_L); };
        "G.BTN.X"          => { MATCH_OP(E_OP_G_BTN_X); };
        "G.BTN.Y"          => { MATCH_OP(E_OP_G_BTN_Y); };
        "G.BTNI"           => { MATCH_OP(E_OP_G_BTNI); };
        "G.BTNV"           => { MATCH_OP(E_OP_G_BTNV); };
        "G.BTNL"           => { MATCH_OP(E_OP_G_BTNL); };
        "G.BTNX"           => { MATCH_OP(E_OP_G_BTNX); };
        "G.BTNY"           => { MATCH_OP(E_OP_G_BTNY); };
        "G.BTN.PR"         => { MATCH_OP(E_OP_G_BTN_PR); };
        "G.BTN.SW"         => { MATCH_OP(E_OP_G_BTN_SW); };
        "G.GBTN.V"         => { MATCH_OP(E_OP_G_GBTN_V); };
        "G.GBTN.L"         => { MATCH_OP(E_OP_G_GBTN_L); };
        "G.GBTN.C"         => { MATCH_OP(E_OP_G_GBTN_C); };
        "G.GBTN.I"         => { MATCH_OP(E_OP_G_GBTN_I); };
        "G.GBTN.W"         => { MATCH_OP(E_OP_G_GBTN_W); };
        "G.GBTN.H"         => { MATCH_OP(E_OP_G_GBTN_H); };
        "G.GBTN.X1"        => { MATCH_OP(E_OP_G_GBTN_X1); };
        "G.GBTN.X2"        => { MATCH_OP(E_OP_G_GBTN_X2); };
        "G.GBTN.Y1"        => { MATCH_OP(E_OP_G_GBTN_Y1); };
        "G.GBTN.Y2"        => { MATCH_OP(E_OP_G_GBTN_Y2); };

        "G.FDR"            => { MATCH_OP(E_OP_G_FDR); };
        "G.FDX"            => { MATCH_OP(E_OP_G_FDX); };
        "G.GFD"            => { MATCH_OP(E_OP_G_GFD); };
        "G.GFX"            => { MATCH_OP(E_OP_G_GFX); };
        "G.FDR.EN"         => { MATCH_OP(E_OP_G_FDR_EN); };
        "G.FDR.V"          => { MATCH_OP(E_OP_G_FDR_V); };
        "G.FDR.N"          => { MATCH_OP(E_OP_G_FDR_N); };
        "G.FDR.L"          => { MATCH_OP(E_OP_G_FDR_L); };
        "G.FDR.X"          => { MATCH_OP(E_OP_G_FDR_X); };
        "G.FDR.Y"          => { MATCH_OP(E_OP_G_FDR_Y); };
        "G.FDRI"           => { MATCH_OP(E_OP_G_FDRI); };
        "G.FDRV"           => { MATCH_OP(E_OP_G_FDRV); };
        "G.FDRN"           => { MATCH_OP(E_OP_G_FDRN); };
        "G.FDRL"           => { MATCH_OP(E_OP_G_FDRL); };
        "G.FDRX"           => { MATCH_OP(E_OP_G_FDRX); };
        "G.FDRY"           => { MATCH_OP(E_OP_G_FDRY); };
        "G.FDR.PR"         => { MATCH_OP(E_OP_G_FDR_PR); };
        "G.GFDR.V"         => { MATCH_OP(E_OP_G_GFDR_V); };
        "G.GFDR.N"         => { MATCH_OP(E_OP_G_GFDR_N); };
        "G.GFDR.L"         => { MATCH_OP(E_OP_G_GFDR_L); };
        "G.GFDR.RN"        => { MATCH_OP(E_OP_G_GFDR_RN); };

        "G.XYP"            => { MATCH_OP(E_OP_G_XYP); };
        "G.XYP.X"          => { MATCH_OP(E_OP_G_XYP_X); };
        "G.XYP.Y"          => { MATCH_OP(E_OP_G_XYP_Y); };

        # w/
        "WS.PLAY"     => { MATCH_OP(E_OP_WS_PLAY); };
        "WS.REC"      => { MATCH_OP(E_OP_WS_REC); };
        "WS.CUE"      => { MATCH_OP(E_OP_WS_CUE); };
        "WS.LOOP"     => { MATCH_OP(E_OP_WS_LOOP); };

				# seed
        "SEED"        => { MATCH_OP(E_OP_SEED); };
        "RAND.SEED"	  => { MATCH_OP(E_OP_RAND_SEED); };
        "RAND.SD"	  => { MATCH_OP(E_OP_SYM_RAND_SD); };
        "R.SD"		  => { MATCH_OP(E_OP_SYM_R_SD); };
        "TOSS.SEED"   => { MATCH_OP(E_OP_TOSS_SEED); };
        "TOSS.SD"	  => { MATCH_OP(E_OP_SYM_TOSS_SD); };
        "PROB.SEED"   => { MATCH_OP(E_OP_PROB_SEED); };
        "PROB.SD"	  => { MATCH_OP(E_OP_SYM_PROB_SD); };
        "DRUNK.SEED"  => { MATCH_OP(E_OP_DRUNK_SEED); };
        "DRUNK.SD"	  => { MATCH_OP(E_OP_SYM_DRUNK_SD); };
        "P.SEED"      => { MATCH_OP(E_OP_P_SEED); };
	    "P.SD"		  => { MATCH_OP(E_OP_SYM_P_SD); };

        # MODS
        # controlflow
        "IF"          => { MATCH_MOD(E_MOD_IF); };
        "ELIF"        => { MATCH_MOD(E_MOD_ELIF); };
        "ELSE"        => { MATCH_MOD(E_MOD_ELSE); };
        "L"           => { MATCH_MOD(E_MOD_L); };
        "W"           => { MATCH_MOD(E_MOD_W); };
        "EVERY"       => { MATCH_MOD(E_MOD_EVERY); };
        "SKIP"        => { MATCH_MOD(E_MOD_SKIP); };
        "OTHER"       => { MATCH_MOD(E_MOD_OTHER); };

        # delay
        "PROB"        => { MATCH_MOD(E_MOD_PROB); };
        "DEL"         => { MATCH_MOD(E_MOD_DEL); };
        "DEL.X"       => { MATCH_MOD(E_MOD_DEL_X); };
        "DEL.R"       => { MATCH_MOD(E_MOD_DEL_R); };

        # matrixarchate
        "MA.SELECT"   => { MATCH_OP(E_OP_MA_SELECT); };
        "MA.STEP"     => { MATCH_OP(E_OP_MA_STEP); };
        "MA.RESET"    => { MATCH_OP(E_OP_MA_RESET); };
        "MA.PGM"      => { MATCH_OP(E_OP_MA_PGM); };
        "MA.ON"       => { MATCH_OP(E_OP_MA_ON); };
        "MA.PON"      => { MATCH_OP(E_OP_MA_PON); };
        "MA.OFF"      => { MATCH_OP(E_OP_MA_OFF); };
        "MA.POFF"     => { MATCH_OP(E_OP_MA_POFF); };
        "MA.SET"      => { MATCH_OP(E_OP_MA_SET); };
        "MA.PSET"     => { MATCH_OP(E_OP_MA_PSET); };
        "MA.COL"      => { MATCH_OP(E_OP_MA_COL); };
        "MA.PCOL"     => { MATCH_OP(E_OP_MA_PCOL); };
        "MA.ROW"      => { MATCH_OP(E_OP_MA_ROW); };
        "MA.PROW"     => { MATCH_OP(E_OP_MA_PROW); };
        "MA.CLR"      => { MATCH_OP(E_OP_MA_CLR); };
        "MA.PCLR"     => { MATCH_OP(E_OP_MA_PCLR); };

        # stack
        "S"           => { MATCH_MOD(E_MOD_S); };
    *|;

    write data;          # write any ragel data here
}%%

// these are our macros that are inserted into the code when Ragel finds a match
#define MATCH_OP(op) { out->tag = OP; out->value = op; no_of_tokens++; }
#define MATCH_MOD(mod) { out->tag = MOD; out->value = mod; no_of_tokens++; }
#define MATCH_NUMBER()                           \
    {                                            \
        out->tag = NUMBER;                       \
        int32_t val = strtol(token, NULL, 0);    \
        val = val > INT16_MAX ? INT16_MAX : val; \
        val = val < INT16_MIN ? INT16_MIN : val; \
        out->value = val;                        \
        no_of_tokens++;                          \
    }

// matches a single token, out contains the token, return value indicates
// success or failure
bool match_token(const char *token, const size_t len, tele_data_t *out) {
    // required ragel declarations
    int cs;                       // machine state
    int act;                      // used with '=>'
    const char* ts;               // token start
    const char* te;               // token end

    const char* p = token;        // pointer to data
    const char* pe = token + len; // pointer to end of data
    const char* eof = pe;         // pointer to eof
    (void)match_token_en_main;    // fix unused variable warning

    int no_of_tokens = 0;

    %%{
        write init; # write initialisation
        write exec; # run the machine
    }%%

    // Ragel errors
    if (cs == match_token_error || cs < match_token_first_final) {
        return false;
    }
    // only 1 token!
    else if (no_of_tokens != 1) {
        return false;
    }
    else {
        return true;
    }

}
