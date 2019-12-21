from os import path
import re
import subprocess

_THIS_FILE = path.realpath(__file__)
_THIS_DIR = path.dirname(_THIS_FILE)

OP_C = path.abspath(path.join(_THIS_DIR, "../../src/ops/op.c"))


def list_tele_ops():
    """Return the names of all the structs defined in tele_ops"""
    with open(OP_C, "r") as f:
        op_c = _remove_comments(f.read())
        ops = re.findall("&op_[a-zA-Z0-9_]+", op_c)
        ops = [s[1:] for s in ops]
        return ops


def list_ops():
    return map(_convert_struct_name_to_op_name, list_tele_ops())


def list_tele_mods():
    """Return the names of all the structs defined in tele_mods"""
    with open(OP_C, "r") as f:
        op_c = _remove_comments(f.read())
        mods = re.findall("&mod_[a-zA-Z0-9_]+", op_c)
        mods = [s[1:] for s in mods]
        return mods


def list_mods():
    return map(_convert_struct_name_to_op_name, list_tele_mods())


def _remove_comments(op_c):
    out = op_c.splitlines()
    out = filter(_is_not_comment, out)
    return "\n".join(out)


def _is_not_comment(line):
    s = line.lstrip()
    return not (s.startswith("//") or s.startswith("/*"))


def _convert_struct_name_to_op_name(name):
    stripped = name.replace("op_", "").replace("mod_", "")

    MAPPINGS = {
        "SYM_PLUS":               "+",
        "SYM_DASH":               "-",
        "SYM_STAR":               "*",
        "SYM_FORWARD_SLASH":      "/",
        "SYM_PERCENTAGE":         "%",
        "SYM_DOLLAR":             "$",
        "SYM_DOLLAR_POL":         "$.POL",
        "SYM_EQUAL_x2":           "==",
        "SYM_EXCLAMATION_EQUAL":  "!=",
        "SYM_LEFT_ANGLED":        "<",
        "SYM_RIGHT_ANGLED":       ">",
        "SYM_LEFT_ANGLED_EQUAL":  "<=",
        "SYM_RIGHT_ANGLED_EQUAL": ">=",
        "SYM_EXCLAMATION":        "!",
        "SYM_LEFT_ANGLED_x2":     "<<",
        "SYM_RIGHT_ANGLED_x2":    ">>",
        "SYM_AMPERSAND_x2":       "&&",
        "SYM_PIPE_x2":            "||",
        "SYM_SEED":               "SEED",
        "SYM_DRUNK_SD":           "DRUNK.SEED",
        "SYM_P_SD":               "P.SEED",
        "SYM_PROB_SD":            "PROB.SEED",
        "SYM_R_SD":               "R.SD",
        "SYM_RAND_SD":            "RAND.SEED",
        "SYM_TOSS_SD":            "TOSS.SEED",
        "BIT_OR":                 "|",
        "BIT_AND":                "&",
        "BIT_NOT":                "~",
        "BIT_XOR":                "^",
        "M_SYM_EXCLAMATION":      "M!",
        "TURTLE":                 "@",
        "TURTLE_X":               "@X",
        "TURTLE_Y":               "@Y",
        "TURTLE_MOVE":            "@MOVE",
        "TURTLE_DIR":             "@DIR",
        "TURTLE_SPEED":           "@SPEED",
        "TURTLE_STEP":            "@STEP",
        "TURTLE_F":               "@F",
        "TURTLE_FX1":             "@FX1",
        "TURTLE_FX2":             "@FX2",
        "TURTLE_FY1":             "@FY1",
        "TURTLE_FY2":             "@FY2",
        "TURTLE_BUMP":            "@BUMP",
        "TURTLE_WRAP":            "@WRAP",
        "TURTLE_BOUNCE":          "@BOUNCE",
        "TURTLE_SCRIPT":          "@SCRIPT",
        "TURTLE_SHOW":            "@SHOW",
        "P_ADD":                  "P.+",
        "PN_ADD":                 "PN.+",
        "P_SUB":                  "P.-",
        "PN_SUB":                 "PN.-",
        "P_ADDW":                 "P.+W",
        "PN_ADDW":                "PN.+W",
        "P_SUBW":                 "P.-W",
        "PN_SUBW":                "PN.-W",
        "TIF":                    "?",
    }

    if stripped in MAPPINGS:
        return MAPPINGS[stripped]
    else:
        return stripped.replace("_", ".")


def validate_toml(ops):
    for (name, d) in ops.items():
        keys = set(d)
        if "prototype" not in keys:
            print(f" - WARNING: {name} - no prototype entry")

        if "short" not in keys:
            print(f" - WARNING: {name} - no short entry")

        if "aliases" in keys and not isinstance(d["aliases"], list):
            print(f" - WARNING: {name} - aliases is not an array")

        for k in keys - {"prototype", "prototype_set", "aliases",
                         "short", "description"}:
            print(f" - WARNING: {name} - unknown entry - {k}")


def get_tt_version():
    tag = subprocess.check_output(["git", "describe", "--tags"]) \
                    .decode("utf-8").split("-")[0]
    hash = subprocess.check_output(["git", "describe",
                                    "--always", "--dirty"]) \
                     .decode("utf-8")[:-1].upper()
    return {'tag': tag, 'hash': hash}
