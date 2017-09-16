# List of available make goals:
#
# all                     Default target, builds the project
# clean                   Clean up the project
# rebuild                 Rebuild the project
#
# isp                     Use BatchISP instead of avr32program when interfacing
#                         the target device.
#
# chiperase               Perform a JTAG chip erase command
# cpuinfo                 Get CPU information
# debug                   Open a debug connection with the MCU
# erase                   Perform a flash erase
# halt                    Stop CPU execution
# program                 Program MCU memory from ELF output file
# reset                   Reset MCU
# readregs                Read CPU registers
# run                     Start CPU execution
# secureflash             Protect chip by setting security bit
#
# doc                     Build the documentation
# cleandoc                Clean up the documentation
# rebuilddoc              Rebuild the documentation
#
#
# Copyright (c) 2009-2010 Atmel Corporation. All rights reserved.
#
# \asf_license_start
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

# Makefile.avr32.in defines an unused variable build, which is used in the clean
# target, it's probably there to list other build targets
build += ../src/scanner.c ../src/match_token.c ../module/gitversion.c

# Include the common Makefile, which will also include the project specific
# config.mk file.
MAKEFILE_PATH = ../libavr32/asf/avr32/utils/make/Makefile.avr32.in
include $(MAKEFILE_PATH)

# Add a rule to build match_token.c from match_token.rl
../src/match_token.c: ../src/match_token.rl
	ragel -C -G2 ../src/match_token.rl -o ../src/match_token.c

# Add a rule to build scanner.c from scanner.rl
../src/scanner.c: ../src/scanner.rl
	ragel -C -G2 ../src/scanner.rl -o ../src/scanner.c

# Add the git commit id to a file for use when printing out the version
../module/gitversion.c: ../.git/HEAD ../.git/index
	echo "const char *git_version = \"$(shell git describe --always | tr '[a-z]' '[A-Z]')-beta3\";" > $@
