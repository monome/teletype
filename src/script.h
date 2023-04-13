#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#define REGULAR_SCRIPT_COUNT 8

#define METRO_SCRIPT REGULAR_SCRIPT_COUNT
#define INIT_SCRIPT (METRO_SCRIPT + 1)

// used for live screen and delays, should have the max index
#define TEMP_SCRIPT (INIT_SCRIPT + 1)

#define EDITABLE_SCRIPT_COUNT (INIT_SCRIPT + 1)
#define TOTAL_SCRIPT_COUNT (EDITABLE_SCRIPT_COUNT + 1)

#define NO_SCRIPT TOTAL_SCRIPT_COUNT

#endif
