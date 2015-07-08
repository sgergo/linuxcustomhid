#include <signal.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>

#include "command.h"
#include "usb.h"


static int sigexit = 0;

static void sighandler(int signal) {
    switch (signal) {
        case SIGINT:
        	usb_disconnect();
            if (sigexit) {
                printf("force quitting\n");
                exit(0);
            }
            printf("sigint received, quitting\n");
            sigexit = 1;
            break;
        case SIGTERM:
        	usb_disconnect();
            if (sigexit) {
                printf("force quitting\n");
                exit(0);
            }
            printf("sigterm received, quitting\n");
            sigexit = 1;
            break;
    }
}

int main(int argc, char **argv) {
    int flags;

	signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    printf("\n-----------------------------------------\n");
    printf("Simple Linux custom HID test application\n");
    printf("----------type help for commands----------\n");

    // make fgets non-blocking (POSIX only)
    flags = fcntl(0, F_GETFL, 0); /* get current file status flags */
    flags |= O_NONBLOCK; /* turn off blocking flag */
    fcntl(0, F_SETFL, flags); 

    usb_init();
    
    while (!sigexit) {

    	command_get_command();
        
        if (usb_device.irq == true) {
            usb_get_IN_packet(true);
        }
    }
	return 0;
}

