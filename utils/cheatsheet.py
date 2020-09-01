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
# final column indicates that a new page is inserted _after_ that section
OPS_SECTIONS = [
    ("variables",     "Variables",     False),
    ("hardware",      "Hardware",      False),
    ("patterns",      "Patterns",      False),
    ("controlflow",   "Control flow",  False),
    ("maths",         "Maths",         False),
    ("metronome",     "Metronome",     False),
    ("delay",         "Delay",         False),
    ("stack",         "Stack",         False),
    ("queue",         "Queue",         False),
    ("seed",          "Seed",          False),
    ("turtle",        "Turtle",        True),
    ("grid",          "Grid",          True),
    ("midi_in",       "MIDI In",       True),
    ("i2c",           "Generic I2C",   True),
    ("ansible",       "Ansible",       False),
    ("whitewhale",    "Whitewhale",    False),
    ("meadowphysics", "Meadowphysics", False),
    ("earthsea",      "Earthsea",      False),
    ("orca",          "Orca",          True),
    ("justfriends",   "Just Friends",  False),
    ("wslash",        "W/",            False),
    ("er301",         "ER-301",        False),
    ("fader",         "Fader",         False),
    ("matrixarchate", "Matrixarchate", True),
    ("telex_i",       "TELEXi",        False),
    ("telex_o",       "TELEXo",        False),
    ("disting",       "Disting EX",    False)
]


def latex_safe(s):
    # backslash must be first, otherwise it will duplicate itself
    unsafe = ["\\", "&", "%", "$", "#", "_", "{", "}", "^"]
    for u in unsafe:
        s = s.replace(u, "\\" + u)
    # ~ is special
    s = s.replace("~", "\\~{}")
    return s


def cheatsheet_tex():
    print(f"Using docs directory:     {DOCS_DIR}")
    print(f"Using ops docs directory: {OP_DOCS_DIR}")
    print()

    output = VERSION_STR + "\n\n"
    for (section, title, new_page) in OPS_SECTIONS:
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
                    prototype += " / " + op["prototype_set"]
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
    return output


def main():
    if len(sys.argv) != 2:
        sys.exit("Please supply a filename")

    p = Path(sys.argv[1]).resolve()
    p.write_text(cheatsheet_tex())


if __name__ == "__main__":
    main()
