# Updates

## v4.0.x

- **FIX**: delay when opening docs
- **FIX**: `PROB 100` would execute only 99.01% of the time.
- **FIX**: some `G.FDR` configurations caused incorrect rendering in grid visualizer
- **NEW**: `PROB` accepts an `ELSE` condition
- **NEW**: alias `PRB` for `PROB`
- **NEW**: new op: `WTOSS`, alias `WT`

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
- **NEW**: new multi-logic OPs `AND3`, `AND4`, `OR3` and `OR4` with aliases `&&&`, `&&&&`, `|||` and `||||`
- **NEW**: ops to control live mode: `LIVE.OFF`, `LIVE.VARS`, `LIVE.GRID`, `LIVE.DASH`, `PRINT`
- **NEW**: `SCENE.P` OP: load another scene but keep current pattern state
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

## Version 3.1

### New operators

`DEVICE.FLIP` - change how screen is displayed and how I/O are numbered to let you mount the module upside down

`DEL.X`, `DEL.R` - repeat an action multiple times, separated by a delay

`J` & `K` local script variables

`SEED`, `R.SEED`, `TOSS.SEED`, `DRUNK.SEED`, `P.SEED`, `PROB.SEED` - get/set seed for different random ops

`SCENE.G` - load another scene but keep the current grid configuration

`SCRIPT.POL` / `$.POL` - get / set script polarity. 1 to fire on rising edges as usual, 2 for falling edges, 3 for both. indicated on live mode w/ mutes icon.

#### New Ansible ops

`ANS.G` / `ANS.G.P` - simulate ansible receiving a grid key press

`ANS.A` - simulate ansible receiving an arc encoder turn

`ANS.G.LED` / `ANS.A.LED` - read LED brightness of ansible grid / arc

#### New Kria ops

`KR.CUE` - get / set the cued Kria pattern

`KR.PG` - switch to Kria parameter page

### Changes

DELAY_SIZE increased to 16 from 8

### Bug fixes

[some keyboards losing keystrokes](https://github.com/monome/teletype/issues/156)

[metro rate not updated after `INIT.SCENE`](https://github.com/monome/teletype/issues/174)



## Version 3.0

### Major new features

#### Grid Integration

Grid integration allows you to use grid to visualize, control and execute teletype scripts. You can create your own UIs using grid ops, or control Teletype directly with the Grid Control mode. Built in Grid Visualizer allows designing and using grid scenes without a grid. For more information and examples of grid scenes please see the [Grid Studies](https://github.com/scanner-darkly/teletype/wiki/GRID-INTEGRATION).

#### Improved script editing

You can now select multiple lines when editing scripts by holding `shift`. You can move the current selection up and down with `alt-<up>` and `alt-<down>`. You can copy/cut/paste a multiline selection as well. To delete selected lines without copying into the clipboard use `alt-<delete>`.

Three level undo is also now available with `ctrl-z` shortcut.

#### Support for the Orthogonal Devices ER-301 Sound Computer over i2c

You now can connect up to three ER-301s via i2c and address up to 100 virtual CV channels and 100 virtual TR channels per ER-301. (The outputs range 1-100, 101-200, and 201-300 respectively.) To function, this requires a slight mod to current in-market ER-301s and a specialized i2c cable that reorders two of the pins. Find more information [on the Orthogonal Devices ER-301 Wiki Teletype Integration Page](http://wiki.orthogonaldevices.com/index.php/ER-301/Teletype_Integration).

#### Support for the 16n Faderbank via i2c

The 16n Faderbank is an open-source sixteen fader controller with support for USB MIDI, standard MIDI, and i2c communication with the Teletype. It operates just like an IN or PARAM (or the TXi for that matter) in that you read values from the device. You use the operator FADER (or the alias FB) and the number of the slider you wish to poll (1-16). Know that longer cables may require that you use a powered bus board even if you only have one device on your Teletype's i2c bus. (You will know that you have a problem if your Teletype randomly hangs on reads.)

#### Support for the SSSR Labs SM010 Matrixarchate via i2c

The SSSR Labs SM010 Matrixarchate is a 16x8 IO Sequenceable Matrix Signal Router. Teletype integration allows you to switch programs and control connections. For a complete list of available ops refer to the manual. Information on how to connect the module can be found [in the SM010 manual](https://www.sssrlabs.com/store/sm010/).

#### Support for W/ via i2c

Support for controlling Whimsical Raps W/ module via i2c. See the respective section for a complete list of available ops and refer to https://www.whimsicalraps.com/pages/w-type for more details.

### New operators

`? x y z` is a ternary "if" operator, it will select between `y` and `z` based on the condition `x`.

#### New pattern ops

`P.MIN` `PN.MIN` `P.MAX` `PN.MAX` return the position for the first smallest/largest value in a pattern between the `START` and `END` points.

`P.RND` / `PN.RND` return a randomly selected value in a pattern between the `START` and `END` points.

`P.+` / `PN.+` / `P.-` / `PN.-` increment/decrement a pattern value by the specified amount.

`P.+W` / `PN.+W` / `P.-W` / `PN.-W` same as above and wrap to the specified range.

#### New Telex ops

`TO.CV.CALIB` allows you to lock-in an offset across power cycles to calibrate your TELEX CV output (`TO.CV.RESET` removes the calibration).

`TO.ENV` now accepts gate values (1/0) to trigger the attack and decay.

#### New Kria ops

`KR.CV x` get the current CV value for channel `x`

`KR.MUTE x` `KR.MUTE x y` get/set mute state for channel `x`

`KR.TMUTE x` toggle mute state for channel `x`

`KR.CLK x` advance the clock for channel `x`

#### Ops for ER-301, 16n Faderbank, SM010, W/

Too many to list, please refer to their respective sections.

### New aliases

`$` for `SCRIPT`

`RND` / `RRND` `RAND` / `RRAND`

`WRP` for `WRAP`

`SCL` for `SCALE`

### New keybindings

Hold `shift` while making line selection in script editing to select multiple lines. Use `alt-<up>` and `alt-<down>` to move selected lines up and down. Copy/cut/paste shortcuts work with multiline selection as well. To delete selected lines without copying into the clipboard use `alt-<delete>`.

While editing a line you can now use `ctrl-<left>` / `ctrl-<right>` to move by words.

`ctrl-z` provides three level undo in script editing.

Additional `Alt-H` shortcut is available to view the Help screen.

`Alt-G` in Live mode will turn on the Grid Visualizer, which has its own shortcuts. Refer to the **Keys** section for a complete list.

The keybindings to insert a scaled knob value in the Tracker mode were changed from `ctrl` to `ctrl-alt` and from `shift` to `ctrl-shift`.

### Bug fixes

i2c initialization delayed to account for ER-301 bootup

last screen saved to flash

knob jitter when loading/saving scenes reduced

[duplicate commands not added to history](https://github.com/monome/teletype/issues/99)

`SCALE` precision improved

`PARAM` set properly when used in the init script

`PARAM` and `IN` won't reset to 0 after `INIT.DATA`

[`PN.HERE`, `P.POP`, `PN.POP` will update the tracker screen](https://github.com/monome/teletype/issues/151)

[`P.RM` was 1-based, now 0-based](https://github.com/monome/teletype/issues/149)

[`P.RM` / `PN.RM` will not change pattern length if deleting outside of length range](https://github.com/monome/teletype/issues/150)

[`JI` op fixed](https://llllllll.co/t/teletype-the-ji-op/10553)

[`TIME` and `LAST` are now 1ms accurate](https://github.com/monome/teletype/issues/144)

[`RAND` / `RRAND` will properly work with large range values](https://github.com/monome/teletype/issues/143)

[`L .. 32767` won't freeze](https://github.com/monome/teletype/issues/148)

### New behavior

Previously, when pasting the clipboard while in script editing the pasted line would replace the current line. It will now instead push the current line down. This might result in some lines being pushed beyond the script limits - if this happens, use `ctrl-z` to undo the change, delete some lines and then paste again.

`I` would previously get initialized to 0 when executing a script. If you called a script from another script's loop this meant you had to use a variable to pass the loop's current `I` value to the called script. This is not needed anymore - when a script is called from another script its `I` value will be set to the current `I` value of the calling script.

## Version 2.2

Teletype version 2.2 introduces Chaos and Bitwise operators, Live mode view of variables, INIT operator, ability to calibrate CV In and Param knob and set Min/Max scale values for both, a screensaver, Random Number Generator, and a number of fixes and improvements.

### Major new features

#### Chaos Operators

The `CHAOS` operator provides a new source of uncertainty to the Teletype via chaotic yet deterministic systems. This operator relies on various chaotic maps for the creation of randomized musical events. Chaotic maps are conducive to creating music because fractals contain a symmetry of repetition that diverges just enough to create beautiful visual structures that at times also apply to audio. In mathematics a map is considered an evolution function that uses polynomials to drive iterative procedures. The output from these functions can be assigned to control voltages. This works because chaotic maps tend to repeat with slight variations offering useful oscillations between uncertainty and predictability.

#### Bitwise Operators

Bitwise operators have been added to compliment the logic functions and offer the ability to maximize the use of variables available on the Teletype.

Typically, when a variable is assigned a value it fully occupies that variable space; should you want to set another you’ll have to use the next available variable. In conditions where a state of on, off, or a bitwise mathematical operation can provide the data required, the inclusion of these operators give users far more choices. Each variable normally contains 16 bits and Bitwise allows you to `BSET`, `BGET`, and `BCLR` a value from a particular bit location among its 16 positions, thus supplying 16 potential flags in the same variable space.

#### INIT

The new op family `INIT` features operator syntax for clearing various states from the unforgiving INIT with no parameters that clears ALL state data (be careful as there is no undo) to the ability to clear CV, variable data, patterns, scenes, scripts, time, ranges, and triggers.

#### Live Mode Variable Display

This helps the user to quickly check and monitor variables across the Teletype. Instead of single command line parameter checks the user is now able to simply press the `~ key` (Tilde) and have a persistent display of eight system variables.

#### Screensaver

Screen saver engages after 90 minutes of inactivity

#### New Operators

  - `IN.SCALE min max` sets the min/max values of the CV Input jack
  - `PARAM.SCALE min max` set the min/max scale of the Parameter Knob
  - `IN.CAL.MIN` sets the zero point when calibrating the CV Input jack
  - `IN.CAL.MAX` sets the max point (16383) when calibrating the CV Input jack
  - `PARAM.CAL.MIN` sets the zero point when calibrating the Parameter Kob
  - `PARAM.CAL.MAX` sets the max point (16383) when calibrating the Parameter Kob
  - `R` generate a random number
  - `R.MIN` set the low end of the random number generator
  - `R.MAX` set the upper end of the random number generator

#### Fixes

  - Multiply now saturates at limits (-32768 / 32767) while previous behavior returned 0 at overflow
  - Entered values now saturate at Int16 limits which are -32768 / 32767
  - Reduced flash memory consumption by not storing TEMP script
  - I now carries across `DEL` commands
  - Corrected functionality of `JI` (Just Intonation) op for 1V/Oct tuning
  - Reduced latency of `IN` op

#### Improvements

  - Profiling code (optional developer feature)
  - Screen now redraws only lines that have changed

## Version 2.1

Teletype version 2.1 introduces new operators that mature the syntax and capability of the Teletype, as well as several bug fixes and enhancement features.

### Major new features

#### Tracker Data Entry Improvements

Data entry in the tracker screen is now _buffered_, requiring an `ENTER` keystroke to commit changes, or `SHIFT-ENTER` to insert the value.  All other navigation keystrokes will abandon data entry.  The increment / decrement keystrokes (`]` and `[`), as well as the negate keystroke (`-`) function immediately if not in data entry mode, but modify the currently buffered value in edit mode (again, requiring a commit).

#### Turtle Operator

The Turtle operator allows 2-dimensional access to the patterns as portrayed out in Tracker mode.  It uses new operators with the `@` prefix.  You can `@MOVE X Y` the turtle relative to its current position, or set its direction in degrees with `@DIR` and its speed with `@SPEED` and then execute a `@STEP`.

To access the value that the turtle operator points to, use `@`, which can also set the value with an argument.

The turtle can be constrained on the tracker grid by setting its fence with `@FX1`, `@FY1`, `@FX2`, and `@FY2`, or by using the shortcut operator `@F x1 y1 x2 y2`.  When the turtle reaches the fence, its behaviour is governed by its _fence mode_, where the turtle can simply stop (`@BUMP`), wrap around to the other edge (`@WRAP`), or bounce off the fence and change direction (`@BOUNCE`).  Each of these can be set to `1` to enable that mode.

Setting `@SCRIPT N` will cause script `N` to execute whenever the turtle crosses the boundary to another cell.  This is different from simply calling `@STEP; @SCRIPT N` because the turtle is not guaranteed to change cells on every step if it is moving slowly enough.

Finally, the turtle can be displayed on the tracker screen with `@SHOW 1`, where it will indicate the current cell by pointing to it from the right side with the `<` symbol.

#### New Mods: EVERY, SKIP, and OTHER, plus SYNC

These mods allow rhythmic division of control flow. EVERY X: executes the post-command once per X at the Xth time the script is called. SKIP X: executes it every time but the Xth. OTHER: will execute when the previous EVERY/SKIP command did not.

Finally, SYNC X will set each EVERY and SKIP counter to X without modifying its divisor value. Using a negative number will set it to that number of steps before the step. Using SYNC -1 will cause each EVERY to execute on its next call, and each SKIP will not execute.

#### Script Line "Commenting"

Individual lines in scripts can now be disabled from execution by highlighting the line and pressing `ALT-/`.  Disabled lines will appear dim.  This status will persist through save/load from flash, but will not carry over to scenes saved to USB drive.

### New Operators

`W [condition]:` is a new mod that operates as a while loop.
The `BREAK` operator stops executing the current script
`BPM [bpm]` returns the number of milliseconds per beat in a given BPM, great for setting `M`.
`LAST [script]` returns the number of milliseconds since `script` was last called.

### New Operator Behaviour

`SCRIPT` with no argument now returns the current script number.
`I` is now local to its corresponding `L` statement.
`IF/ELSE` is now local to its script.

### New keybindings

`CTRL-1` through `CTRL-8` toggle the mute status for scripts 1 to 8 respectively.
`CTRL-9` toggles the METRO script.
`SHIFT-ENTER` now inserts a line in Scene Write mode.

### Bug fixes

Temporal recursion now possible by fixing delay allocation issue, e.g.: DEL 250: SCRIPT SCRIPT
`KILL` now clears `TR` outputs and stops METRO.
`SCENE` will no longer execute from the INIT script on initial scene load.
`AVG` and `Q.AVG` now round up from offsets of 0.5 and greater.

### Breaking Changes

As `I` is now local to `L` loops, it is no longer usable across scripts or as a general-purpose variable.
As `IF/ELSE` is now local to a script, scenes that relied on IF in one script and ELSE in another will be functionally broken.

## Version 2.0

Teletype version 2.0 represents a large rewrite of the Teletype code base. There are many new language additions, some small breaking changes and a lot of under the hood enhancements.


### Major new features

#### Sub commands

Several commands on one line, separated by semicolons.

e.g. `CV 1 N 60; TR.PULSE 1`

See the section on "Sub commands" for more information.

#### Aliases

For example, use `TR.P 1` instead of `TR.PULSE 1`, and use `+ 1 1`, instead of `ADD 1 1`.

See the section on "Aliases" for more information.

#### `PN` versions of every `P` `OP`

There are now `PN` versions of every `P` `OP`. For example, instead of:

```
P.I 0
P.START 0
P.I 1
P.START 10
```

You can use:

```
PN.START 0 0
PN.START 1 10
```

#### TELEXi and TELEXo `OP`s

Lots of `OP`s have been added for interacting with the wonderful TELEXi input expander and TELEXo output expander. See their respective sections in the documentation for more information.

#### New keybindings

The function keys can now directly trigger a script.

The `<tab>` key is now used to cycle between live, edit and pattern modes, and there are now easy access keys to directly jump to a mode.

Many new text editing keyboard shortcuts have been added.

See the "Modes" documentation for a listing of all the keybindings.

#### USB memory stick support

You can now save you scenes to USB memory stick at any time, and not just at boot up. Just insert a USB memory stick to start the save and load process. Your edit scene should not be effected.

It should also be significantly more reliable with a wider ranger of memory sticks.

**WARNING:** Please backup the contents of your USB stick before inserting it. Particularly with a freshly flashed Teletype as you will end up overwriting all the saved scenes with blank ones.

### Other additions

 - Limited script recursion now allowed (max recursion depth is 8) including self recursion.
 - Metro scripts limited to 25ms, but new `M!` op to set it as low as 2ms (at your own risk), see "Metronome" `OP` section for more.

### Breaking changes

  - **Removed the need for the `II` `OP`.**

    For example, `II MP.PRESET 1` will become just `MP.PRESET 1`.

  - **Merge `MUTE` and `UNMUTE` `OP`s to `MUTE x` / `MUTE x y`.**

    See the documentation for `MUTE` for more information.

  - **Remove unused Meadowphysics `OP`s.**

    Removed: `MP.SYNC`, `MP.MUTE`, `MP.UNMUTE`, `MP.FREEZE`, `MP.UNFREEZE`.

  - **Rename Ansible Meadowphysics `OP`s to start with `ME`.**

    This was done to avoid conflicts with the Meadowphysics `OP`s.

 **WARNING**: If you restore your scripts from a USB memory stick, please manually fix any changes first. Alternatively, incorrect commands (due to the above changes) will be skipped when imported, please re-add them.

### Known issues

#### Visual glitches

The cause of these is well understood, and they are essentially harmless. Changing modes with the `<tab>` key will force the screen to redraw. A fix is coming in version 2.1.
