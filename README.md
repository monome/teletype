# teletype

monome eurorack module. This fork contains modified code to support a single 4-channel, 14-bit DAC such as the AD5685 (instead of 2 2-channel, 12-bit AD5687 DACs in the original hardware). For more info, see this comment on [lines](https://llllllll.co/t/teletype-hardware-open/27242/522).

http://monome.org/docs/teletype

## Directories

- `src`: source code for the teletype algorithm
- `module`: `main.c` and additional code for the Eurorack module (e.g. IO and UI)
- `tests`: algorithm tests
- `simulator`: a (very) simple teletype command parser and simulator
- `docs`: files used to generate the teletype manual

## Building

See the [libavr32 repo][libavr32] for more detailed instructions. You will also need `ragel` installed and on the path, see below.

Alternatively, if you have Docker installed, you can quickly get
building with a [Docker image](https://github.com/Dewb/monome-build)
that has all the dependencies set up:

```bash
git clone --recursive --config core.autocrlf=input https://github.com/monome/teletype
docker run --rm -it -v"$(pwd)/teletype":/target dewb/monome-build bash
make
```

The reasoning behind these options:

```
git
  --recursive                   # clone libavr32 and unity submodules
  --config core.autocrlf=input  # if on Windows, avoid adding CRLF line endings that can break compile scripts

docker
  --rm                          # delete the container after exiting it
  -it                           # keep stdin attached, allocate tty
  -v"$(pwd)/teletype":/target  # mount the ./teletype directory at /target inside the container
```

**Make sure that the `libavr32` submodule is correctly checked out**

```bash
cd module
make clean
make
./flash.sh
```

## Tests

To run the tests:

```bash
cd tests
make clean  # only needed if you've built the module code
make test
```

In the case of line ending issues `make test` may fail, in this case
`make tests && ./tests` might work better.

## Ragel

The [Ragel state machine compiler][ragel] is required to build the firmware. It needs to be installed and on the path:

```bash
brew install ragel  # Homebrew (OSX)
apt install ragel   # Debian / Ubuntu / WSL (Windows 10)
pacman -Sy ragel    # Arch Linux / MSYS2 (Windows)
```

Version 6.9 is known to work.

See section 6.3 in the Ragel manual for information on the `=>` scanner constructor used.

## Adding a new `OP` or `MOD` (a.k.a. `PRE`)

If you want to add a new `OP` or `MOD`, please create the relevant `tele_op_t` or `tele_mod_t` in the `src/ops` directory. You will then need to reference it in the following places:

- `src/ops/op.c`: add a reference to your struct to the relevant table, `tele_ops` or `tele_mods`. Ideally grouped with other ops from the same file.
- `src/ops/op_enum.h`: please run `python3 utils/op_enums.py` to generate this file.
- `src/match_token.rl`: add an entry to the Ragel list to match the token to the struct. Again, please try to keep the order in the list sensible.
- `module/config.mk`: add a reference to any added .c files in the CSRCS list.
- `tests/Makefile`: add a reference to any added .c files in /src, replacing ".c" with ".o", in the tests: recipe.
- `simulator/Makefile`: add a reference to any added .c files in /src, replacing ".c" with ".o", in the OBJS list.

There is a test that checks to see if the above have all been entered correctly. (See above to run tests.)

## Code Formatting

To format the code using `clang-format`, run `make format` in the project's root directory. This will _only_ format code that has not been commited, it will format _both_ staged and unstaged code.

To format all the code in this repo, run `make format-all`.

[libavr32]: https://github.com/monome/libavr32
[ragel]: http://www.colm.net/open-source/ragel/

## Documentation

In order to build the documentation you will need Python 3.6 or greater, [Pandoc][], as well as the Python libraries specified in the [`requirements.pip`][requirements.pip] file. In addition, to generate the PDF output you will also require [TexLive][] (or [MacTex][]).

On OSX the dependencies can be installed with `brew`.

```bash
brew install python3
brew install pandoc
brew cask install mactex  # warning, MacTex is a very large install!
cd utils
pip3 install -r requirements.pip
```

On Linux I would suggest using [`virtualenv`][virtualenv] to install all the Python dependencies (including those in the [`requirements.pip`][requirements.pip] file), and to ensure that the `python3` binary is version 3.6 or greater instead of the default of your distro.

[virtualenv]: https://virtualenv.pypa.io/en/stable/

To generate the documentation:

```bash
cd docs
make               # build both teletype.pdf and teletype.html
make teletype.pdf  # build just teletype.pdf (requires TexLive or MacTex)
make teletype.html # build just teletype.html
```

[requirements.pip]: utils/requirements.pip
[pandoc]: http://pandoc.org/
[texlive]: https://www.tug.org/texlive/
[mactex]: https://www.tug.org/mactex/

## Making a Release

To create a `teletype.zip` file containing:

- `teletype.hex`
- `flash.sh`
- `update_firmware.command`
- `teletype.pdf`

Run `make release` in the project's root directory
