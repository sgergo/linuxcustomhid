#include <signal.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h> 

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


	signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    usb_init();
    usb_connect_device(0x2047, 0x0301);

    while (!sigexit) {

    	command_get_command();
    }
	return 0;
}

