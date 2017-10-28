# Changelog

## vNEXT
- **NEW**: added a cheat sheet PDF
- **IMP**: screen now redraws only lines that have changed
- **FIX**: multiply now saturates at limits, previous behaviour returned 0 at overflow

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
- **FIX**: KILL now clears TR output as well as disabling the METRO script.
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
- **FIX**: `i2c` bus crashes under high `M` times with external triggers
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
