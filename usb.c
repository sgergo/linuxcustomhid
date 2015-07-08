#include <signal.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

#include <libusb-1.0/libusb.h>
#include "usb.h"


usb_state_t usb_device;
static struct libusb_device_handle *handle;

void usb_print_error_message(int errorcode) {
    if (errorcode)
        printf("libusb error code %d: %s\n", errorcode, libusb_error_name(errorcode));
    else
        printf("%s\n", libusb_error_name(errorcode));
}

int usb_connect_device(uint16_t vid, uint16_t pid) {
    int errorcode;

    usb_device.device_connected = false;
	handle = libusb_open_device_with_vid_pid(NULL, vid, pid); 

    if (handle == NULL) 
        return (-1);
    
    libusb_detach_kernel_driver(handle, 0); 
    usb_device.device_connected = true;

    usb_device.interface_claimed = false;
    errorcode = libusb_set_configuration(handle, 1); 
    if (errorcode < 0) {
            usb_print_error_message(errorcode);
            return -1;
        }
    
    errorcode = libusb_claim_interface(handle, 0); 
    if (errorcode < 0) {
            usb_print_error_message(errorcode);
            return -1;
        }

    usb_device.interface_claimed = true;
    return 0;  
}

int usb_get_IN_packet(bool arraymode) {
    int errorcode; 
    unsigned int transferred;
    int i;
    static int j = 0;

    errorcode = libusb_interrupt_transfer(handle, ENDPOINT_INT_IN, usb_device.irqinbuffer, IRQ_IN_PACKETSIZE, 
        &transferred, TIMEOUT); 
    if (errorcode < 0) {
            usb_print_error_message(errorcode);
            return -1;
    }

    if (transferred < IRQ_IN_PACKETSIZE) { 
        printf("Interrupt transfer short read (%d byte(s))\n", transferred); 
        return -1; 
    }

    for (i = 0; i < IRQ_IN_PACKETSIZE; i++) {
        printf("%3d ", usb_device.irqinbuffer[i]);
        if (arraymode) {
            j++;
            if(j%8 == 0) 
                 printf("\n");
        } else
            printf("\n"); 
    }

    return 0;
 }

 int usb_get_control_report_packet(bool arraymode) {
    int errorcode; 
    int i;
    static int j = 0;
    printf("getr magic: %0d\n", (HID_REPORT_TYPE_INPUT<<8)|0x00);

    errorcode = libusb_control_transfer(handle, CTRL_IN, HID_GET_REPORT, 
                                        (HID_REPORT_TYPE_INPUT<<8)|0x00, 
                                        0, usb_device.ctrlreportinbuffer, 
                                        CTRL_REPORT_PACKETSIZE, TIMEOUT); 
    if (errorcode < 0) {
            usb_print_error_message(errorcode);
            return -1;
    }

    for (i = 0; i < CTRL_REPORT_PACKETSIZE; i++) {
        printf("%3d ", usb_device.ctrlreportinbuffer[i]);
        if (arraymode) {
            j++;
            if(j%8 == 0) 
                 printf("\n");
        } else
            printf("\n"); 
    }

    return 0;
}

int usb_set_control_report_packet(unsigned char num) {
    int errorcode; 
    int i;
    static int j = 0;

    usb_device.ctrlreportoutbuffer[0] = num;
    printf("setr magic: %d\n", (HID_REPORT_TYPE_OUTPUT<<8)|0x00);

    errorcode = libusb_control_transfer(handle, CTRL_OUT, HID_SET_REPORT, 
                                        (HID_REPORT_TYPE_OUTPUT<<8)|0x00, 
                                        0, usb_device.ctrlreportoutbuffer, 
                                        CTRL_REPORT_PACKETSIZE, TIMEOUT); 
    if (errorcode < 0) {
            usb_print_error_message(errorcode);
            return -1;
    }

    return 0;
}

int usb_print_device_strings(void) {
    int errorcode;
    struct libusb_device_descriptor desc;
    struct libusb_device *device;
    unsigned char strman[64];
    unsigned char strprod[64];
    unsigned char strver[64];

    device = libusb_get_device(handle);

    errorcode = libusb_get_device_descriptor(device, &desc);
    if (errorcode < 0) {
        usb_print_error_message(errorcode);
        return -1;
    }

    libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, strman, sizeof(strman));
    libusb_get_string_descriptor_ascii(handle, desc.iProduct, strprod, sizeof(strprod));
    libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, strver, sizeof(strver));
    printf("Manufacturer: %s\t Product: %s\t VID:PID %04x:%04x\t Version: %s\n", strman, strprod, desc.idVendor, desc.idProduct, strver);

    return 0;
}

static int usb_print_device_vidpid(libusb_device *device) {
	int errorcode;
	struct libusb_device_descriptor desc;


	errorcode = libusb_get_device_descriptor(device, &desc);
    if (errorcode < 0) {
    	usb_print_error_message(errorcode);
    	return -1;
    }

    printf("Class: %02x \tVID:PID\t%04x:%04x\n", desc.bDeviceClass, desc.idVendor, desc.idProduct);

    return 0;
}

int usb_list_usb_devices(void) {
	struct libusb_device **devs;
	int cnt = 0;
	int i;

	//get the list of devices
    cnt = libusb_get_device_list(NULL, &devs); 
    if(cnt < 0) {
    	printf("get device error\n");
    	return -1;
    }
    printf("\nAvailable USB devices:\n");
    printf("---------------------------------\n");

    for(i = 0; i < cnt; i++)
       	usb_print_device_vidpid(devs[i]);

    //free the list, unref the devices in it
    libusb_free_device_list(devs, 1);
    printf("---------------------------------\n\n");

    return 0;
}

int usb_disconnect(void) {
    if (usb_device.interface_claimed)
        libusb_release_interface(handle, 0);
    if (usb_device.device_connected)
        libusb_close(handle); 
    libusb_exit(NULL);

    return 0;
}

int usb_init(void) {
	libusb_context *ctx = NULL;
	int errorcode = 1;

	errorcode = libusb_init(&ctx); 
    if (errorcode < 0) {
    	usb_print_error_message(errorcode);
    	return -1;
    }

    usb_device.device_connected = false;
    handle = NULL;

    return 0;
}