#!/usr/bin/env python3

import sys
from pathlib import Path

import jinja2
import pypandoc
import pytoml as toml

from common import list_ops, list_mods

if (sys.version_info.major, sys.version_info.minor) < (3, 6):
    raise Exception("need Python 3.6 or later")

THIS_FILE = Path(__file__).resolve()
ROOT_DIR = THIS_FILE.parent.parent
TEMPLATE_DIR = ROOT_DIR / "utils" / "templates"
DOCS_DIR = ROOT_DIR / "docs"
OP_DOCS_DIR = DOCS_DIR / "ops"
FONTS_DIR = ROOT_DIR / "utils" / "fonts"

env = jinja2.Environment(
    autoescape=False,
    loader=jinja2.FileSystemLoader(str(TEMPLATE_DIR)),
    trim_blocks=True,
    lstrip_blocks=True
)

# determines the order in which sections are displayed
OPS_SECTIONS = [
    "variables",
    "hardware",
    "patterns",
    "controlflow",
    "maths",
    "metronome",
    "delay",
    "stack",
    "queue",
    "ansible",
    "whitewhale",
    "meadowphysics",
    "earthsea",
    "orca",
    "justfriends",
    "telex_i",
    "telex_o"
]


def deep_merge_dict(source, destination):
    for key, value in source.items():
        if isinstance(value, dict):
            node = destination.setdefault(key, {})
            deep_merge_dict(value, node)
        else:
            destination[key] = value

    return destination


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


def common_md():
    print(f"Using docs directory:     {DOCS_DIR}")
    print(f"Using ops docs directory: {OP_DOCS_DIR}")
    print()

    op_table_template = env.get_template("op_table.jinja2.md")
    op_extended_template = env.get_template("op_extended.jinja2.md")

    output = ""
    output += Path(DOCS_DIR / "intro.md").read_text() + "\n\n"
    output += Path(DOCS_DIR / "whats_new.md").read_text() + "\n\n"
    output += Path(DOCS_DIR / "modes.md").read_text() + "\n\n"
    output += Path(DOCS_DIR / "ops.md").read_text() + "\n\n"

    all_ops = set(list_ops()) | set(list_mods())
    all_ops_dict = {}

    ops_with_docs = set()

    for section in OPS_SECTIONS:
        md_file = Path(OP_DOCS_DIR, section + ".md")
        toml_file = Path(OP_DOCS_DIR, section + ".toml")

        output += "\\newpage\n"

        if md_file.exists() and md_file.is_file():
            print(f"Reading {md_file}")
            output += md_file.read_text() + "\n\n"
            output += "\n"

        if toml_file.exists() and toml_file.is_file():
            print(f"Reading {toml_file}")
            extended = []
            # n.b. Python 3.6 dicts maintain insertion order
            ops = toml.loads(toml_file.read_text())
            validate_toml(ops)
            deep_merge_dict(ops, all_ops_dict)
            for key in ops:
                if key not in all_ops:
                    print(f" - WARNING: unknown {key}")
                ops_with_docs.add(key)
                if "aliases" in ops[key]:
                    ops_with_docs |= set(ops[key]["aliases"])
                if "description" in ops[key]:
                    render = op_extended_template.render(name=key, **ops[key])
                    extended.append((key, render))

            output += op_table_template.render(ops=ops.values())
            output += "\n"
            output += "\n".join([e[1] for e in extended]) + "\n\n"

    output += Path(DOCS_DIR / "advanced.md").read_text() + "\n\n"

    output += "\\appendix\n\n"

    output += "# Alphabetical list of OPs and MODs\n\n"
    sorted_ops = [kv[1] for kv in sorted(all_ops_dict.items())]
    output += op_table_template.render(ops=sorted_ops)

    output += "# Missing documentation\n\n"
    missing_ops = all_ops - ops_with_docs
    output += ", ".join([f"`{o}`" for o in sorted(missing_ops)]) + "\n\n"

    output += Path(ROOT_DIR / "CHANGELOG.md").read_text() + "\n\n"

    return output


def main():
    if len(sys.argv) <= 1:
        sys.exit("Please supply a filename")

    input_format = "markdown"
    output = common_md()

    print()

    for arg in sys.argv[1:]:
        p = Path(arg).resolve()
        print(f"Generating: {p}")

        ext = p.suffix

        if ext == ".md":
            p.write_text(output)
        elif ext == ".html":
            pypandoc.convert_text(
                output,
                format=input_format,
                to="html5",
                outputfile=str(p),
                extra_args=["--standalone",
                            "--self-contained",
                            "--toc",
                            "--toc-depth=2",
                            "--css=" + str(TEMPLATE_DIR / "docs.css"),
                            "--template=" + str(TEMPLATE_DIR / "template.html5")])
        elif ext == ".pdf" or ext == ".tex":
            latex_preamble = env.get_template("latex_preamble.jinja2.md")
            latex = latex_preamble.render(fonts_dir=FONTS_DIR) + "\n\n"
            latex += output
            pypandoc.convert_text(
                latex,
                format=input_format,
                to=ext[1:],
                outputfile=str(p),
                extra_args=["--standalone",
                            "--column=80",
                            "--toc",
                            "--toc-depth=2",
                            "--latex-engine=xelatex",
                            "--variable=papersize:A4"])


if __name__ == "__main__":
    main()
