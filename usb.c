#include <signal.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

#include <libusb-1.0/libusb.h>

static struct libusb_device_handle *devh = NULL;

static void usb_print_error_message(int errorcode) {
	printf("error code %d: %s\n", errorcode, libusb_error_name(errorcode));
}

int usb_connect_device(uint16_t vid, uint16_t pid) {

	devh = libusb_open_device_with_vid_pid(NULL, vid, pid); 
    return devh ? 0 : -EIO;

    libusb_detach_kernel_driver(devh, 0);

    
}
static int usb_print_device_info(libusb_device *device) {
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

int usb_list_usb_devices(libusb_context *ctx) {
	struct libusb_device **devs;
	int cnt = 0;
	int i;

	//get the list of devices
    cnt = libusb_get_device_list(ctx, &devs); 
    if(cnt < 0) {
    	printf("get device error\n");
    	return -1;
    }
    printf("\n\nAvailable USB devices:\n");
    printf("------------------------\n");

    for(i = 0; i < cnt; i++)
       	usb_print_device_info(devs[i]);

    libusb_free_device_list(devs, 1); //free the list, unref the devices in it
}

int usb_disconnect(void) {

	libusb_close(devh); 
    libusb_exit(NULL);
}

int usb_init(void) {
	libusb_context *ctx = NULL;
	int errorcode = 1;

	errorcode = libusb_init(&ctx); 
    if (errorcode < 0) {
    	usb_print_error_message(errorcode);
    	return -1;
    }

    usb_list_usb_devices(ctx);
    return 0;
}