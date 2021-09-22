# Changelog

## v4.0.x

- **FIX**: cache currently-running commands to avoid corruption during SCENE ops.
- **FIX**: delay when opening docs
- **FIX**: `PROB 100` would only execute 99.01% of the time.
- **FIX**: some `G.FDR` configurations caused incorrect rendering in grid visualizer
- **FIX**: fix `EX.LP` not returning correct values
- **IMP**: scene load/save code refactor, add scene load/save tests

## v4.0.0

- **FIX**: `LAST SCRIPT` in live mode gives time since init script was run
- **FIX**: negative pattern values are properly read from USB
- **FIX**: delay when navigating to sections in docs
- **NEW**: generic i2c ops: `IIA`, `IIS..`, `IIQ..`, `IIB..`
- **NEW**: exponential delay operator `DEL.G`
- **NEW**: binary and hex format for numbers: `B...`, `X...`
- **NEW**: Disting EX ops
- **FIX**: `LAST n` is broken for script 1
- **NEW**: bitmasked delay and quantize: `DEL.B..`,  `QT.B..`, `QT.BX..`
- **NEW**: scale and chord quantize: `QT.S..`, `QT.CS..`
- **NEW**: bit toggle OP: `BTOG..`
- **NEW**: volts to semitones helper OP: `VN..`
- **IMP**: DELAY_SIZE increased to 64 from 16
- **FIX**: scale degree arguments 1-indexed: `N.S`, `N.CS`
- **NEW**: Just Friends 4.0 OPs and dual JF OPs
- **NEW**: binary scale ops `N.B` and `N.BX`
- **NEW**: reverse binary for numbers: `R...`
- **NEW**: reverse binary OP: `BREV`
- **NEW**: `ES.CV` read earthsea CV values
- **NEW**: added setter for `R`, sets R.MIN and R.MAX to same value, allowing R to be used as variable
- **NEW**: v/oct to hz/v conversion op: `HZ`
- **FIX**: W/2.0 ops added
- **NEW**: W/2.0 ops documentation
- **NEW**: `><`, `<>`, `>=<` and `<=>` OPs, checks if value is within or outside of range
- **IMP**: new powerful Q OPs
- **IMP**: Improved line editing movement (forward/backward by word skips intervening space).
- **NEW**: Delete to end of word command `alt-d` added.
- **NEW**: new op: `SCENE.P`
- **NEW**: new multi-logic OPs `AND3`, `AND4`, `OR3` and `OR4` with aliases `&&&`, `&&&&`, `|||` and `||||`
- **NEW**: alias: `EV` for `EVERY`
- **NEW**: live mode dashboard
- **NEW**: ops to control live mode: `LIVE.OFF`, `LIVE.VARS`, `LIVE.GRID`, `LIVE.DASH`, `PRINT`
- **FIX**: `PN.ROT` parameters are swapped
- **FIX**: better rendering for fine grid faders
- **FIX**: logical operators should treat all non zero values as `true`, not just positive values
- **NEW**: crow ops
- **NEW**: `TI.PRM.CALIB` alias added (was already in the docs)
- **FIX**: `SCENE` would crash if parameter was out of bounds

## v3.2.0

- **FIX**: improve DAC latency when using `CV` ops
- **NEW**: call metro / init with `SCRIPT 9` / `SCRIPT 10`
- **NEW**: forward (C-f or C-s) and reverse (C-r) search in help mode
- **NEW**: new ops: `LROT` (alias `<<<`), `RROT` (alias `>>>`)
- **NEW**: `LSH` and `RSH` shift the opposite direction when passed a negative shift amount
- **NEW**: new op: `SGN` (sign of argument)
- **NEW**: new kria remote op: `KR.DUR`
- **NEW**: new op: `NR` (binary math pattern generator)
- **NEW**: new ops: `N.S, N.C, N.CS` (use western scales and chords to get values from `N` table)
- **NEW**: new ops: `FADER.SCALE, FADER.CAL.MIN, FADER.CAL.MAX, FADER.CAL.RESET`  for scaling 16n Faderbank values (aliases `FB.S, FB.C.MIN, FB.C.MAX, FB.C.R`)
- **NEW**: new Tracker mode keybinding `alt-[ ]` semitone up, down
- **NEW**: new Tracker mode keybinding `ctrl-[ ]` fifth up, down
- **NEW**: new Tracker mode keybinding `shift-[ ]` octave up, down
- **NEW**: new Tracker mode keybinding `alt-<0-9>` `<0-9>` semitones up (0=10, 1=11)
- **NEW**: new Tracker mode keybinding `shift-alt-<0-9>` `<0-9>` semitones down (0=10, 1=11)
- **FIX**: dim M in edit mode when metro inactive
- **NEW**: new pattern ops: `P.SHUF`, `PN.SHUF`, `P.REV`, `PN.REV`, `P.ROT`, `PN.ROT`
- **NEW**: new pattern mods: `P.MAP:`, `PN.MAP x:`

## v3.1.0

- **NEW**: new op: `DEVICE.FLIP`
- **FIX**: [some keyboards losing keystrokes](https://github.com/monome/teletype/issues/156)
- **NEW**: new op: `DEL.X`
- **NEW**: new op: `DEL.R`
- **IMP**: DELAY_SIZE increased to 16 from 8
- **NEW**: new variables: `J` & `K` local script variables
- **FIX**: [metro rate not updated after `INIT.SCENE`](https://github.com/monome/teletype/issues/174)
- **NEW**: new ops: `SEED`, `R.SEED`, `TOSS.SEED`, `DRUNK.SEED`, `P.SEED`, `PROB.SEED`
- **NEW**: new op: `SCENE.G`
- **NEW**: new op: `SCRIPT.POL`, alias `$.POL`
- **NEW**: new ansible remote ops: `ANS.G`, `ANS.G.P`, `ANS.G.LED`, `ANS.A`, `ANS.A.LED`
- **NEW**: new kria remote ops: `KR.CUE`, `KR.PG`

## v3.0.0

- **NEW**: grid integration / grid visualizer / grid control mode
- **NEW**: multiline copy/paste and editing
- **NEW**: new keybindings to move by words
- **NEW**: undo in script editing
- **NEW**: i2c support for ER-301
- **NEW**: i2c support for 16n Faderbank
- **NEW**: i2c support for Matrixarchate
- **NEW**: i2c support for W/
- **NEW**: new op: `?`
- **NEW**: new ops: `P.MIN`, `PN.MIN`, `P.MAX`, `PN.MAX`, `P.RND`, `PN.RND`, `P.+`, `PN.+`, `P.-`, `PN.-`. `P.+W`, `PN.+W`, `P.-W`, `PN.-W`
- **NEW**: new Telex ops: `TO.CV.CALIB`, `TO.ENV`
- **NEW**: new Kria ops: `KR.CV`, `KR.MUTE`, `KR.TMUTE`, `KR.CLK`, `ME.CV`
- **NEW**: new aliases: `$`, `RND`, `RRND`, `WRP`, `SCL`
- **NEW**: telex, ansible, just friends, w/ added to the help screen
- **FIX**: i2c initialization delayed to account for ER-301 bootup
- **FIX**: last screen saved to flash
- **FIX**: knob jitter when loading/saving scenes reduced
- **FIX**: [duplicate commands not added to history](https://github.com/monome/teletype/issues/99)
- **FIX**: `SCALE` precision improved
- **FIX**: `PARAM` set properly when used in the init script
- **FIX**: `PARAM` and `IN` won't reset to 0 after `INIT.DATA`
- **FIX**: [`PN.HERE`, `P.POP`, `PN.POP` will update the tracker screen](https://github.com/monome/teletype/issues/151)
- **FIX**: [`P.RM` was 1-based, now 0-based](https://github.com/monome/teletype/issues/149)
- **FIX**: [`P.RM` / `PN.RM` will not change pattern length if deleting outside of length range](https://github.com/monome/teletype/issues/150)
- **FIX**: [`JI` op fixed](https://llllllll.co/t/teletype-the-ji-op/10553)
- **FIX**: [`TIME` and `LAST` are now 1ms accurate](https://github.com/monome/teletype/issues/144)
- **FIX**: [`RAND` / `RRAND` will properly work with large range values](https://github.com/monome/teletype/issues/143)
- **FIX**: [`L .. 32767` won't freeze](https://github.com/monome/teletype/issues/148)
- **FIX**: `I` now accessible to child SCRIPTS

## v2.2
- **NEW**: added a cheat sheet PDF
- **NEW**: new bitwise ops: `&`, `|`, `^`, `~`, `BSET`, `BCLR`, `BGET`
- **NEW**: new ops `PARAM.SCALE min max` and `IN.SCALE min max` to add static scaling to inputs
- **NEW**: blanking screensaver after 90 minutes of keyboard inactivity, any key to wake
- **NEW**: new op: `CHAOS` chaotic sequence generator.  Control with `CHAOS.ALG` and `CHAOS.R`
- **NEW**: new op family: `INIT`, to clear device state
- **NEW**: new ops: `R`, `R.MIN`, `R.MAX` programmable RNG
- **IMP**: profiling code (optional, dev feature)
- **IMP**: screen now redraws only lines that have changed
- **FIX**: multiply now saturates at limits, previous behaviour returned 0 at overflow
- **FIX**: entered values now saturate at int16 limits
- **FIX**: reduced flash memory consumption by not storing TEMP script
- **FIX**: `I` now carries across `DEL` commands
- **FIX**: removed TEMP script allocation in flash
- **FIX**: corrected functionality of JI op for 1volt/octave tuning and removed octave-wrapping behaviour (now returns exactly the entered ratio)
- **FIX**: reduced latency of `IN` op

## v2.1
- **BREAKING**: the `I` variable is now scoped to the `L` loop, and does not exist outside of an execution context.  Scripts using `I` as a general-purpose variable will be broken.
- **FIX**: `SCENE` will not run from `INIT` script during scene load.
- **NEW**: Tracker data entry overhaul.  Type numbers, press enter to commit.
- **NEW**: new op: `BPM` to get milliseconds per beat in given BPM
- **NEW**: script lines can be disabled / enabled with ctrl-/
- **NEW**: shift-enter in scene write mode now inserts a line
- **NEW**: new ops: `LAST x` for the last time script `x` was called
- **NEW**: `SCRIPT` with no arguments gets the current script number.
- **FIX**: `AVG` and `Q.AVG` now round up properly
- **NEW**: new op: `BREAK` to stop the remainder of the script
- **NEW**: new mod: `W [condition]: [statement]` will execute `statement` as long as `condition` is true (up to an iteration limit).
- **NEW**: new mods: `EVERY x:`, `SKIP x:`, `OTHER:` to alternately execute or not execute a command.
- **NEW**: new op: `SYNC x` will synchronize all `EVERY` and `SKIP` line to the same step.
- **NEW**: new feature: @ - the turtle.  Walks around the pattern grid.  Many ops, see documentation.
- **OLD**: ctrl-F1 to F8 mute/unmute scripts.
- **NEW**: ctrl-F9 enables/disables METRO.
- **FIX**: recursive delay fix.  Now you can `1: DEL 500: SCRIPT 1` for temporal recursion.
- **FIX**: `KILL` now clears TR output as well as disabling the METRO script.
- **FIX**: if / else conditions no longer transcend their script
- **IMP**: functional exectuion stack for `SCRIPT` operations

## v2.0.1
- **FIX**: update IRQ masking which prevents screen glitches and crashing under heavy load

## v2.0
- **BREAKING**: remove `II` op. Ops that required it will now work with out it. (e.g. `II MP.PRESET 1` will become just `MP.PRESET 1`)
- **BREAKING**: merge the `MUTE` and `UNMUTE` ops. Now `MUTE x` will return the mute status for trigger `x` (`0` is unmuted, `1` is muted), and `MUTE x y` will set the mute for trigger `x` (`y = 0` to unmute, `y = 1` to mute)
- **BREAKING**: remove unused Meadowphysics ops: `MP.SYNC`, `MP.MUTE`, `MP.UNMUTE`, `MP.FREEZE`, `MP.UNFREEZE`
- **BREAKING**: rename Ansible Meadowphysics ops to start with `ME`
- **NEW**: sub commands, use a `;` separator to run multiple commands on a single line, e.g. `X 1; Y 2`
- **NEW**: key bindings rewritten
- **NEW**: aliases: `+` for `ADD`, `-` for `SUB`, `*` for `MUL`, `/` for `DIV`, `%` for `MOD`, `<<` for `LSH`, `>>` for `RSH`, `==` for `EQ`, `!=` for `NE`, `<` for `LT`, `>` for `GT`, `<=` for `LTE`, `>=` for `GTE`, `!` for `EZ`, `&&` for `AND`, `||` for `OR`, `PRM` for `PARAM`, `TR.P` for `TR.PULSE`
- **NEW**: new ops: `LTE` (less than or equal), and `GTE` (greater than or equal)
- **NEW**: new pattern ops: `PN.L`, `PN.WRAP`, `PN.START`, `PN.END`, `PN.I`, `PN.HERE`, `PN.NEXT`, `PN.PREV`, `PN.INS`, `PN.RM`, `PN.PUSH` and `PN.POP`
- **NEW**: USB disk loading and saving works at any time
- **NEW**: `M` limited to setting the metronome speed to 25ms, added `M!` to allow setting the metronome at unsupported speeds as low as 2ms
- **NEW**: TELEX Aliases: `TO.TR.P` for `TO.TR.PULSE` (plus all sub-commands) and `TI.PRM` for `TI.PARAM` (plus all sub-commands)
- **NEW**: TELEX initialization commands: `TO.TR.INIT n`, `TO.CV.INIT n`, `TO.INIT x`, `TI.PARAM.INIT n`, `TI.IN.INIT n`, and `TI.INIT x`
- **IMP**: new Ragel parser backend
- **IMP**: script recursion enhanced, maximum recursion depth is 8, and self recursion is allowed
- **IMP**: removed the need to prefix `:` and `;` with a space, e.g. `IF X : TR.PULSE 1` becomes `IF X: TR.PULSE`
- **IMP**: `AND` and `OR` now work as boolean logic, rather than bitwise, `XOR` is an alias for `NE`
- **FIX**: divide by zero errors now explicitly return a 0 (e.g. `DIV 5 0` now returns 0 instead of -1), previously the behaviour was undefined and would crash the simulator
- **FIX**: numerous crashing bugs with text entry
- **FIX**: i2c bus crashes under high `M` times with external triggers
- **FIX**: `P.I` and `PN.I` no longer set values longer than allowed
- **FIX**: `VV` works correctly with negative values

## v1.4.1
- **NEW**: added Ansible remote commands `LV.CV` and `CY.CV`
- **NEW**: Added TELEX Modules Support for the TXi and the TXo
- **NEW**: 75 New Operators Across the Two Modules
- **NEW**: Supports all basic Teletype functions (add `TI` and `TO` to the commands you already know)
- **NEW**: Extended functionality allows for additional capabilities for existing functions
- **NEW**: Experimental input operators add capabilities such as input range mapping and quantization
- **NEW**: Experimental output operators add oscillators, envelopes, independent metronomes, pulse dividing, etc.
- **NEW**: [Full List of Methods Found and Maintained Here](https://github.com/bpcmusic/telex/blob/master/commands.md)

## v1.2.1
- **NEW**: Just Friends ops: `JF.GOD`, `JF.MODE`, `JF.NOTE`, `JF.RMODE`, `JF.RUN`, `JF.SHIFT`, `JF.TICK`, `JF.TR`, `JF.TUNE`, `JF.VOX`, `JF.VTR`

## v1.2
- **NEW**: Ansible support added to ops: `CV`, `CV.OFF`, `CV.SET`, `CV.SLEW`, `STATE`, `TR`, `TR.POL`, `TR.PULSE`, `TR.TIME`, `TR.TOG`
- **NEW**: `P.RM` will also return the value removed
- **NEW**: `ER` op
- **IMP**: a `TR.TIME` of 0 will disable the pulse
- **IMP**: `O.DIR` renamed to `O.INC`, it's the value by which `O` is *incremented* when it is accessed
- **IMP**: `IF`, `ELIF`, `ELSE` status is reset on each script run
- **IMP**: key repeat now works for all keypresses
- **FIX**: `FLIP` won't interfere with the value of `O`
- **FIX**: the `O` op now returns it's set value *before* updating itself
- **FIX**: the `DRUNK` op now returns it's set value *before* updating itself
- **FIX**: `P.START` and `P.END` were set to 1 when set with too large values, now are set to 63
- **FIX**: `CV.SLEW` is correctly initialised to 1 for all outputs
- **FIX**: several bugs where pattern length wasn't updated in track mode
- **FIX**: fixed `[` and `]` not updating values in track mode

## v1.1
- **NEW**: USB flash drive read/write
- **NEW**: `SCRIPT` op for scripted execution of other scripts!
- **NEW**: `MUTE` and `UNMUTE` ops for disabling trigger input
- **NEW**: hotkeys for `MUTE` toggle per input (meta-shift-number)
- **NEW**: screen indication in live mode for `MUTE` status
- **NEW**: `SCALE` op for scaling number from one range to another
- **NEW**: `JI` op just intonation helper
- **NEW**: `STATE` op to read current state of input triggers 1-8 (low/high = 0/1)
- **NEW**: keypad executes scripts (works for standalone USB keypads and full-sized keyboards)
- **NEW**: `KILL` op clears delays, stack, CV slews, pulses
- **NEW**: hotkey meta+ESC executes `KILL`
- **NEW**: `ABS` op absolute value, single argument
- **NEW**: `FLIP` op variable which changes state (0/1) on each read
- **NEW**: logic ops: `AND`, `OR`, `XOR`
- **NEW**: `O` ops: `O.MIN`, `O.MAX`, `O.WRAP`, `O.DIR` for counter range control
- **NEW**: `DRUNK` ops: `DRUNK.MIN`, `DRUNK.MAX`, `DRUNK.WRAP` for range control
- **NEW**: `TR.POL` specifies the polarity of `TR.PULSE`
- **NEW**: if powered down in tracker mode, will power up in tracker mode
- **IMP**: `TR.PULSE` retrigger behaviour now predictable
- **IMP**: mode switch keys more consistent (not constantly resetting to live mode)
- **FIX**: bug in command history in live mode
- **FIX**: `EXP` op now exists
- **FIX**: `P` and `PN` parse error
- **FIX**: possible crash on excess length line entry
- **FIX**: `CV` wrapping with negative `CV.OFF` values
- **FIX**: INIT script executed now on keyboardless scene recall
- **FIX**: `Q.AVG` overflow no more
- **FIX**: `P.PUSH` will fully fill a pattern
- **FIX**: `CV.SET` followed by slewed CV in one command works
- **FIX**: `DEL 0` no longer voids command

## v1.0
- Initial release
