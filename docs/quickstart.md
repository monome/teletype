# Quickstart

## Panel

![Panel Overlay](img/panel-overlay.png)

The keyboard is attached to the front panel, for typing commands. The commands can be executed immediately in *LIVE mode* or assigned to one of the eight trigger inputs in *EDIT mode*. The knob and in jack can be used to set and replace values.

## LIVE mode

Teletype starts up in LIVE mode. You'll see a friendly **>** prompt, where commands are entered. The command:

    TR.TOG A

will toggle trigger A after pressing enter. Consider:

    CV 1 V 5
    CV 2 N 7
    CV 1 0

Here the first command sets CV 1 to 5 volts. The second command sets CV 2 to note 7 (which is 7 semitones up). The last command sets CV 1 back to 0.

Data flows from right to left, so it becomes possible to do this:

    CV 1 N RAND 12

Here a random note between 0 and 12 is set to CV 1.

We can change the behavior of a command with a *PRE* such as `DEL`:

    DEL 500 : TR.TOG A

`TR.TOG A` will be delayed by 500ms upon execution.

A helpful display line appears above the command line in dim font. Here any entered commands will return their numerical value if they have one.

*SCRIPTS*, or several lines of commands, can be assigned to trigger inputs. This is when things get musically interesting. To edit each script, we shift into EDIT mode.

### LIVE mode icons

Four small icons are displayed in LIVE mode to give some important feedback about the state of Teletype. These icons will be brightly lit when the above is true, else will remain dim. They are, from left to right:

* Slew: CV outputs are currently slewing to a new destination.
* Delay: Commands are in the delay queue to be executed in the future.
* Stack: Commands are presently on the stack waiting for execution.
* Metro: Metro is currently active and the Metro script is not empty.

## EDIT mode

Toggle between EDIT and LIVE modes by pushing **TAB**.

The prompt now indicates the script you're currently editing:

* `1`-`8` indicates the script associated with corresponding trigger
* `M` is for the internal metronome
* `I` is the init script, which is executed upon scene recall

Script 1 will be executed when trigger input 1 (top left jack on the panel) receives a low-to-high voltage transition (trigger, or front edge of a gate). Consider the following as script 1:

1:

    TR.TOG A

Now when input 1 receives a trigger, `TR.TOG A` is executed, which toggles the state of output trigger A.

Scripts can have multiple lines:

1:

    TR.TOG A
    CV 1 V RAND 4

Now each time input 1 receives a trigger, CV 1 is set to a random volt between 0 and 4, in addition to output trigger A being toggled.

### Metronome

The `M` script is driven by an internal metronome, so no external trigger is required. By default the metronome interval is 1000ms. You can change this readily (for example, in LIVE mode):

    M 500

The metronome interval is now 500ms. You can disable/enable the metronome entirely with `M.ACT`:

    M.ACT 0

Now the metronome is off, and the `M` script will not be executed. Set `M.ACT` to 1 to re-enable.

## Patterns

Patterns facilitate musical data manipulation-- lists of numbers that can be used as sequences, chord sets, rhythms, or whatever you choose. Pattern memory consists four banks of 64 steps. Functions are provided for a variety of pattern creation, transformation, and playback. The most basic method of creating a pattern is by directly adding numbers to the sequence:

    P.PUSH 5
    P.PUSH 11
    P.PUSH 9
    P.PUSH 3

`P.PUSH` adds the provided value to the end of the list-- patterns keep track of their length, which can be read or modified with `P.L`. Now the pattern length is 4, and the list looks something like:

    5, 11, 9, 3

Patterns also have an index `P.I`, which could be considered a playhead. `P.NEXT` will advance the index by one, and return the value stored at the new index. If the playhead hits the end of the list, it will either wrap to the beginning (if `P.WRAP` is set to 1, which it is by default) or simply continue reading at the final position.

So, this script on input 1 would work well:

1:

    CV 1 N P.NEXT

Each time input 1 is triggered, the pattern moves forward one then CV 1 is set to the note value of the pattern at the new index. This is a basic looped sequence. We could add further control on script 2:

2:

    P.I 0

Since `P.I` is the playhead, trigger input 2 will reset the playhead back to zero. It won't change the CV, as that only happens when script 1 is triggered.

We can change a value within the pattern directly:

    P 0 12

This changes index 0 to 12 (it was previously 5), so now we have *12, 11, 9, 3.*

We've been working with pattern `0` up to this point. There are four pattern banks, and we can switch banks this way:

    P.N 1

Now we're on pattern bank 1. `P.NEXT`, `P.PUSH`, `P`, (and several more commands) all reference the current pattern bank. Each pattern maintains its own play index, wrap parameter, length, etc.

We can directly access and change *any* pattern value with the command `PN`:

    PN 3 0 22

Here the first argument (3) is the *bank*, second (0) is the *index*, and last is the new value (22). You could do this by doing `P.N 3` then `P 0 22` but there are cases where a direct read/write is needed in your patch.

Check the *Command Set* section below for more pattern commands.

Patterns are stored in flash with each scene!

### TRACKER mode

Editing patterns with scripts or from the command line isn't always ergonomic. When you'd like to visually edit patterns, TRACKER mode is the way.

The `~` (tilde) key, above `TAB`, toggles into TRACKER mode.

The current pattern memory is displayed in columns. Use the arrow keys to navigate. Holding ALT will jump by pages.

The edit position is indicated by the brightest number. Very dim numbers indicate they are outside the pattern length.

Use the square bracket keys `[` and `]` to decrease/increase the values. Backspace sets the value to 0. Entering numbers will overwrite a new value. You can cut/copy/paste with ALT-X-C-V.


## Scenes

A *SCENE* is a complete set of scripts and patterns. Stored in flash, scenes can be saved between sessions. Many scenes ship as examples. On startup, the last used scene is loaded by Teletype.

Access the SCENE menu using `ESCAPE`. The bracket keys (`[` and `]`) navigate between the scenes. Use the up/down arrow keys to read the scene *text*. This text will/should describe what the scene does generally along with input/output functions. `ENTER` will load the selected scene, or `ESCAPE` to abort.

To save a scene, hold `ALT` while pushing `ESCAPE`. Use the brackets to select the destination save position. Edit the text section as usual-- you can scroll down for many lines. The top line is the name of the scene. `ALT-ENTER` will save the scene to flash.

### Keyboard-less Scene Recall

To facilitate performance without the need for the keyboard, scenes can be recalled directly from the module's front panel.

* Press the `SCENE RECALL` button next to the USB jack on the panel.
* Use the `PARAM` knob to highlight your desired preset.
* Hold the `SCENE RECALL` button for 1 second to load the selected scene.

### Init Script

The *INIT* script (represented as `I`) is executed when a preset is recalled. This is a good place to set initial values of variables if needed, like metro time `M` or time enable `TIME.ACT` for example.

## Commands

### Nomenclature

* SCRIPT -- multiple *commands*
* COMMAND -- a series (one line) of *words*
* WORD -- a text string separated by a space: *value*, *operator*, *variable*, *mod*
* VALUE -- a number
* OPERATOR -- a function, may need value(s) as argument(s), may return value
* VARIABLE -- named memory storage
* MOD -- condition/rule that applies to rest of the *command*, e.g.: del, prob, if, s

### Syntax

Teletype uses prefix notation. Evaluation happens from right to left.

The left value gets assignment (*set*). Here, temp variable `X` is assigned zero:

    X 0

Temp variable `Y` is assigned to the value of `X`:

    Y X

`X` is being *read* (*get* `X`), and this value is being used to *set* `Y`.

Instead of numbers or variables, we can use operators to perform more complex behavior:

    X TOSS

`TOSS` returns a random state, either 0 or 1 on each call.

Some operators require several arguments:

    X ADD 1 2

Here `ADD` needs two arguments, and gets 1 and 2. `X` is assigned the result of `ADD`, so `X` is now 3.

If a value is returned at the end of a command, it is printed as a MESSAGE. This is visible in LIVE mode just above the command prompt. (In the examples below ignore the // comments).

    8           // prints 8
    X 4
    X           // prints 4
    ADD 8 32    // prints 40

Many parameters are indexed, such as CV and TR. This means that CV and TR have multiple values (in this case, each has four.) We pass an extra argument to specify which index we want to read or write.

    CV 1 0

Here CV 1 is set to 0. You can leave off the 0 to print the value.

    CV 1        // prints value of CV 1

Or, this works too:

    X CV 1      // set X to current value of CV 1

Here is an example of using an operator `RAND` to set a random voltage:

    CV 1 V RAND 4

First a random value between 0 and 3 is generated. The result is turned into a volt with a table lookup, and the final value is assigned to CV 1.

The order of the arguments is important, of course. Consider:

    CV RRAND 1 4 0

`RRAND` uses two arguments, 1 and 4, returning a value between these two. This command, then, chooses a random CV output (1-4) to set to 0. This might seem confusing, so it's possible to clarify it by pulling it apart:

    X RRAND 1 4
    CV X 0

Here we use `X` as a temp step before setting the final CV.

With some practice it becomes easier to combine many functions into the same command.

Furthermore, you can use a semicolon to include multiple commands on the same line:

    X RRAND 1 4; CV X 0

This is particularly useful in **INIT** scripts where you may want to initialize several values at once:

    A 66; X 101; TR.TIME 1 20;

## Continuing

Don't forget to checkout the [Teletype Studies](https://monome.org/docs/modular/teletype/studies-1) for an example-driven guide to the language.
