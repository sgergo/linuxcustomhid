#ifndef USB_H
#define USB_H


int usb_connect_device(uint16_t vid, uint16_t pid);
int usb_disconnect(void);
int usb_init(void);
#endif