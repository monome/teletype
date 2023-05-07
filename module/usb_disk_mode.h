#ifndef _USB_DISK_MODE_H_
#define _USB_DISK_MODE_H_

#include <stdint.h>

void handler_usb_PollADC(int32_t data);
void handler_usb_Front(int32_t data);
void handler_usb_ScreenRefresh(int32_t data);

void tele_usb_disk(void);

#endif
