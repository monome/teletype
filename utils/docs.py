#!/usr/bin/env python3

import sys
from pathlib import Path

import jinja2
import pypandoc
import pytoml as toml

from common import list_ops, list_mods, validate_toml, get_tt_version

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

env = jinja2.Environment(
    autoescape=False,
    loader=jinja2.FileSystemLoader(str(TEMPLATE_DIR)),
    trim_blocks=True,
    lstrip_blocks=True,
    cache_size=0,
    auto_reload=True
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
    "seed",
    "turtle",
    "grid",
    "ansible",
    "whitewhale",
    "meadowphysics",
    "earthsea",
    "orca",
    "justfriends",
    "telex_i",
    "telex_o",
    "er301",
    "fader",
    "wslash",
    "matrixarchate",
]


def deep_merge_dict(source, destination):
    for key, value in source.items():
        if isinstance(value, dict):
            node = destination.setdefault(key, {})
            deep_merge_dict(value, node)
        else:
            destination[key] = value

    return destination


def common_md():
    print(f"Pandoc version:           {pypandoc.get_pandoc_version()}")
    print(f"Using docs directory:     {DOCS_DIR}")
    print(f"Using ops docs directory: {OP_DOCS_DIR}")
    print()

    html_op_list_template = env.get_template("html_op_list.jinja2.md")
    html_op_table_template = env.get_template("html_op_table.jinja2.md")
    pdf_op_table_template = env.get_template("pdf_op_table.jinja2.md")
    pdf_op_extended_template = env.get_template("pdf_op_extended.jinja2.md")

    intro = ""
    intro += Path(DOCS_DIR / "intro.md") \
        .read_text().replace("VERSION", TT_VERSION["tag"][1:]) + "\n\n"
    intro += Path(DOCS_DIR / "whats_new.md").read_text() + "\n\n"
    intro += Path(DOCS_DIR / "quickstart.md").read_text() + "\n\n"
    intro += Path(DOCS_DIR / "keys.md").read_text() + "\n\n"
    intro += Path(DOCS_DIR / "ops.md").read_text() + "\n\n"

    all_ops = set(list_ops()) | set(list_mods())
    all_ops_dict = {}

    ops_with_docs = set()

    pdf_ops_section = ""
    html_ops_section = ""

    for section in OPS_SECTIONS:
        md_file = Path(OP_DOCS_DIR, section + ".md")
        toml_file = Path(OP_DOCS_DIR, section + ".toml")

        pdf_ops_section += "\\newpage\n"
        html_ops_section += "\\newpage\n"

        if md_file.exists() and md_file.is_file():
            print(f"Reading {md_file}")
            pdf_ops_section += md_file.read_text() + "\n\n"
            html_ops_section += md_file.read_text() + "\n\n"

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
                    render = pdf_op_extended_template.render(name=key, **ops[key])
                    extended.append((key, render))

            pdf_ops_section += pdf_op_table_template.render(ops=ops.values())
            pdf_ops_section += "\n"
            pdf_ops_section += "\n".join([e[1] for e in extended]) + "\n\n"

            html_ops_section += html_op_list_template.render(ops=ops.values())

    advanced = Path(DOCS_DIR / "advanced.md").read_text() + "\n\n"

    pdf_alpha_ops = "\\appendix\n\n"
    pdf_alpha_ops += "# Alphabetical list of OPs and MODs\n\n"
    html_alpha_ops = pdf_alpha_ops

    sorted_ops = [kv[1] for kv in sorted(all_ops_dict.items())]
    pdf_alpha_ops += pdf_op_table_template.render(ops=sorted_ops)
    html_alpha_ops += html_op_table_template.render(ops=sorted_ops)

    missing = "\n\n# Missing documentation\n\n"
    missing += ", ".join([f"`{o}`" for o in sorted(all_ops - ops_with_docs)]) + "\n\n"

    changelog = Path(ROOT_DIR / "CHANGELOG.md").read_text() + "\n\n"

    pdf_output = intro + pdf_ops_section + advanced + pdf_alpha_ops + missing + changelog
    html_output = intro + html_ops_section + advanced + html_alpha_ops + missing + changelog

    return {"pdf": pdf_output, "html": html_output}


def main():
    if len(sys.argv) <= 1:
        sys.exit("Please supply a filename")

    input_format = "markdown"
    pdf_output = common_md()
    html_output = pdf_output["html"]
    pdf_output = pdf_output["pdf"]

    print()

    for arg in sys.argv[1:]:
        p = Path(arg).resolve()
        print(f"Generating: {p}")

        ext = p.suffix

        if ext == ".md":
            p.write_text(pdf_output)
        elif ext == ".html":
            html_output = "# " + VERSION_STR + "\n\n" + html_output
            pypandoc.convert_text(
                html_output,
                format=input_format,
                to="html5",
                outputfile=str(p),
                extra_args=["--standalone",
                            "--self-contained",
                            "--toc",
                            "--toc-depth=2",
                            "--css=" + str(TEMPLATE_DIR / "docs.css"),
                            "--template=" + str(TEMPLATE_DIR /
                                                "template.html")])
        elif ext == ".pdf" or ext == ".tex":
            latex_preamble = env.get_template("latex_preamble.jinja2.md")
            latex = latex_preamble \
                .render(title=VERSION_STR, fonts_dir=FONTS_DIR) + "\n\n"
            latex += pdf_output
            pandoc_version = int(pypandoc.get_pandoc_version()[0])
            engine = ("--pdf-engine=xelatex"
                      if pandoc_version >= 2
                      else "--latex-engine=xelatex")
            pypandoc.convert_text(
                latex,
                format=input_format,
                to=ext[1:],
                outputfile=str(p),
                extra_args=["--standalone",
                            "--column=80",
                            "--toc",
                            "--toc-depth=2",
                            engine,
                            "--variable=papersize:A4"])


if __name__ == "__main__":
    main()
