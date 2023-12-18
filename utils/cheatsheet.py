#!/usr/bin/env python3

import sys
import multiprocessing
from pathlib import Path

import jinja2
import pypandoc
import pytoml as toml

from common import validate_toml, get_tt_version

if (sys.version_info.major, sys.version_info.minor) < (3, 6):
    raise Exception("need Python 3.6 or later")

THIS_FILE = Path(__file__).resolve()
ROOT_DIR = THIS_FILE.parent.parent
TEMPLATE_DIR = ROOT_DIR / "utils" / "templates"
DOCS_DIR = ROOT_DIR / "docs"
OP_DOCS_DIR = DOCS_DIR / "ops"
FONTS_DIR = ROOT_DIR / "utils" / "fonts"
TT_VERSION = get_tt_version()
VERSION_STR = " ".join(["Teletype", TT_VERSION["tag"], "Documentation"])


# We want to run inject_latex in parallel as it's quite slow, and we must run
# it once for each op.
# But it must be defined before the multiprocessing.Pool is defined (because...
# python.)
def inject_latex(value):
    latex = pypandoc.convert_text(value["short"], format="markdown", to="tex")
    value["short_latex"] = latex
    return value


# create a multiprocessing pool
pool = multiprocessing.Pool()

# our jinja2 environment
env = jinja2.Environment(
    autoescape=False,
    loader=jinja2.FileSystemLoader(str(TEMPLATE_DIR)),
    trim_blocks=True,
    lstrip_blocks=True
)

# determines the order in which sections are displayed,
# last two columns indicates when a column break or page break is inserted _after_ that section
OPS_SECTIONS = {
    "core": [
        ("variables",     "Variables",       False, False),
        ("hardware",      "Hardware",        False, False),
        ("patterns",      "Patterns",         True, False),
        ("controlflow",   "Control flow",    False, False),
        ("maths",         "Maths",           False, False),
        ("music",         "Musical Maths",   False, False),
        ("metronome",     "Metronome",       False, False),
        ("delay",         "Delay",           False, False),
        ("stack",         "Stack",           True, False),
        ("queue",         "Queue",           True, False),
        ("seed",          "Seed",            True, False),
        ("turtle",        "Turtle",          False, True),
        ("grid",          "Grid",            False, False),
        ("midi_in",       "MIDI In",         False, False),
        ("hardware_setup", "Hardware Setup", False, False)
    ],
    "i2c": [
        ("i2c",           "Generic I2C",    False, False),
        ("ansible",       "Ansible",        False, False),
        ("whitewhale",    "Whitewhale",     True, False),
        ("meadowphysics", "Meadowphysics",  False, False),
        ("earthsea",      "Earthsea",       False, False),
        ("orca",          "Orca",           False, False),
        ("justfriends",   "Just Friends",   True, False),
        ("er301",         "ER-301",         False, False),
        ("fader",         "Faderbank",      False, False),
        ("matrixarchate", "Matrixarchate",  False, False),
        ("telex_i",       "TELEXi",         False, False),
        ("telex_o",       "TELEXo",         False, False),
        ("disting",       "Disting EX",     False, False),
        ("wslash",        "W/1.0",          False, False),
        ("wslashdelay",   "W/2.0 delay",    False, False),
        ("wslashsynth",   "W/2.0 synth",    False, False),
        ("wslashtape",    "W/2.0 tape",     False, False),
        ("crow",          "Crow",           False, True),
        ("i2c2midi",      "I2C2MIDI",       False, False)
    ]
}

def latex_safe(s):
    # backslash must be first, otherwise it will duplicate itself
    unsafe = ["\\", "&", "%", "$", "#", "_", "{", "}", "^"]
    for u in unsafe:
        s = s.replace(u, "\\" + u)
    # ~ is special
    s = s.replace("~", "\\~{}")
    return s


def cheatsheet_tex(sections):
    print(f"Using docs directory:     {DOCS_DIR}")
    print(f"Using ops docs directory: {OP_DOCS_DIR}")
    print()

    output = VERSION_STR + "\n\n"
    for (section, title, new_col, new_page) in sections:
        toml_file = Path(OP_DOCS_DIR, section + ".toml")
        if toml_file.exists() and toml_file.is_file():
            output += f"\\group{{{ title }}}\n\n"
            print(f"Reading {toml_file}")
            # n.b. Python 3.6 dicts maintain insertion order
            ops = toml.loads(toml_file.read_text())
            validate_toml(ops)
            ops_array = pool.map(inject_latex, ops.values())
            for op in ops_array:
                prototype = latex_safe(op["prototype"])
                if "prototype_set" in op:
                    prototype += " / " + latex_safe(op["prototype_set"])
                output += "\\begin{op}"
                if "aliases" in op and len(op["aliases"]) > 0:
                    output += "[" + latex_safe(" ".join(op["aliases"])) + "]"
                output += "{" + prototype + "}"
                output += "\n"
                output += op["short_latex"]
                output += "\\end{op}"
                output += "\n\n"
            if new_page:
                output += "\\pagebreak\n\n"
            if new_col:
                output += "\\vfill\\null\n\\columnbreak\n"
    return output


def main():
    if len(sys.argv) != 3:
        sys.exit("Please supply a cheatsheet category and filename")

    category = sys.argv[1]
    p = Path(sys.argv[2]).resolve()
    p.write_text(cheatsheet_tex(OPS_SECTIONS[category]))

if __name__ == "__main__":
    main()
