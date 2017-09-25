# What's new?

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
