## MIDI in

MIDI in ops allow the Teletype to react to MIDI events. MIDI is received via the USB port - simply plug a MIDI controller or sequencer
into the USB port. Unless your MIDI device is powered externally, make sure your power supply can provide sufficient power! Please note 
that not all devices are supported. 

To use the MIDI in ops, you need to assign MIDI events to one of the scripts with `MI.$` op. You can assign different event types
to different scripts (so, script 1 could react to Note On events and script 2 to Note Off, for instance). You can assign multiple
event types to the same script too. Various ops allow you to get detailed information about the event type and any additional data.
It's possible that more than one event happens before a script is called (say, if you turn multiple knobs at once or play chords).
To properly process them all, use indexed ops to get each event data instead of only processing the last event. The indexed ops
use variable `I` as the index to allow easy use in loops.
