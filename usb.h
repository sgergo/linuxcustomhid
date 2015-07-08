#ifndef USB_H
#define USB_H

// HID Class-Specific Requests values. See section 7.2 of the HID specifications 
#define HID_GET_REPORT                0x01 
#define HID_GET_IDLE                  0x02 
#define HID_GET_PROTOCOL              0x03 
#define HID_SET_REPORT                0x09 
#define HID_SET_IDLE                  0x0A 
#define HID_SET_PROTOCOL              0x0B 
#define HID_REPORT_TYPE_INPUT         0x01 
#define HID_REPORT_TYPE_OUTPUT        0x02 
#define HID_REPORT_TYPE_FEATURE       0x03

#define CTRL_IN LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE 
#define CTRL_OUT LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE 
#define ENDPOINT_INT_IN 0x81

#define TIMEOUT 5000

typedef struct{
    bool device_connected;
    bool interface_claimed;
    bool irq;
    unsigned char irqinbuffer[64];
    unsigned char ctrlreportinbuffer[64];
    unsigned char ctrlreportoutbuffer[64];
}usb_state_t;

extern usb_state_t usb_device;

void usb_print_error_message(int errorcode);
int usb_connect_device(uint16_t vid, uint16_t pid);
int usb_get_IN_packet(bool arraymode);
int usb_get_control_report_packet(bool arraymode);
int usb_set_control_report_packet(unsigned char num);
int usb_print_device_strings(void);
int usb_disconnect(void);
int usb_list_usb_devices(void);
int usb_init(void);
#endif