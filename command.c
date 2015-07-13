#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "cmdline.h"
#include "usb.h"

static void command_execute(char *commandline_received) {
	int ret;
	ret = cmdline_process(commandline_received);

    // If CmdLineProcess returns with a non-zero value something went wrong
	if (ret) 
        printf("Unknown command, error code: %d\n", ret); 
}

void command_get_command(void) {
	static char buf[128];
  	char *p = NULL;

	if (fgets(buf, sizeof(buf), stdin) != NULL) {

		// Return if it was just a CR or NL
		if (buf[0] == '\n' || buf[0] == '\r')
			return;

		// Remove trailing CR
		if ((p = strchr(buf, '\n')) != NULL)
			*p = '\0';

		command_execute(buf);
	}
}

// // Command 'example1'
// static int command_example1(int argc, char *argv[]) {

//     if (argc < 2) {
//         printf("error: missing argument.\n");
//         return (0);
//     }

//     if (!strcmp (argv[1], "on")) {
//         //TODO
//     } else if (!strcmp (argv[1], "off")) {
//         //TODO
//     } else
//         printf("error: invalid input.\n");

//     return(0);
// }

// // Command 'example2'
// static int command_example2(int argc, char *argv[]) {
//     long num;
//     char *endptr;
   
//     if (argc < 2) {
//         printf("error: missing value.\n");
//         return (0);
//     }

//     errno = 0;
//     num = strtol(argv[1], &endptr, 16);

//     if (*endptr != 0 || errno != 0) {
//         printf("error: invalid input.\n");
//         return (0);
//     }

//     // TODO: num contains a valid value, now do something with it
    
//     return(0);
// }

static int command_connect(int argc, char *argv[]) {

    
    usb_print_error_message(usb_connect_device(0x1cbe, 0x000a));

    return(0);
}

static int command_geti(int argc, char *argv[]) {

    if (!usb_device.device_connected || !usb_device.interface_claimed) {
        printf("error: no device is connected.\n");
        return (0);
    }
    printf("getting interrupt IN report...\n");
    usb_get_IN_packet(false);

    return(0);
}

static int command_getr(int argc, char *argv[]) {

    if (!usb_device.device_connected || !usb_device.interface_claimed) {
        printf("error: no device is connected.\n");
        return (0);
    }
    printf("getting control IN report...\n");
    usb_get_control_report_packet(false);

    return(0);
}

static int command_help(int argc, char *argv[]) {
	commandentry_t *entry;

    printf("\nAvailable commands\n");
    printf("-------------------------------------------------------------------------------------------------\n");
    printf("%10.10s%10s%10s\t%s\n", "Command", "Args", "Format", "Description");
    printf("-------------------------------------------------------------------------------------------------\n");

    entry = &commandtable[0]; // Point at the beginning of the command table.

    while(entry->commandname_ptr) {

        // Print the command name and the brief description.
        printf("%10.10s%10s%10s\t%s\n", 
            entry->commandname_ptr, 
            entry->commandarg_ptr, 
            entry->commandargformat_ptr, 
            entry->commandhelptext_ptr );

        // Advance to the next entry in the table. 
        entry++;
    }
    
    printf("\n");
    return(0);
}

static int command_info(int argc, char *argv[]) {

    printf("getting device info strings...\n");
    if (usb_device.device_connected) 
        usb_print_device_strings();
    else
        printf("error: no device is connected.\n");

    return(0);
}

static int command_irq(int argc, char *argv[]) {

    if (argc < 2) {
        printf("error: missing argument.\n");
        return (0);
    }

    if (!usb_device.device_connected || !usb_device.interface_claimed) {
        printf("error: no device is connected.\n");
        return (0);
    }

    if (!strcmp (argv[1], "on")) {
        usb_device.irq = true;
    } else if (!strcmp (argv[1], "off")) {
        usb_device.irq = false;
    } else
        printf("error: invalid input.\n");

    return(0);
}

static int command_list(int argc, char *argv[]) {

    printf("getting USB device list...\n");
    usb_list_usb_devices();

    return(0);
}


static int command_quit(int argc, char *argv[]) {

	usb_disconnect();
	printf("quitting...\n");
    exit(0);
}

static int command_setr(int argc, char *argv[]) {
    long num;
    char *endptr;

    if (argc < 2) {
        printf("error: missing value.\n");
        return (0);
    }

    if (!usb_device.device_connected || !usb_device.interface_claimed) {
        printf("error: no device is connected.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[1], &endptr, 16);

    if (*endptr != 0 || errno != 0) {
        printf("error: invalid input.\n");
        return (0);
    }

    printf("sending control OUT report...\n");
    usb_set_control_report_packet((unsigned char) num);
    
    return(0);
}

// Command table entries - fill it!
commandentry_t commandtable[] = {

    // { "ex1", "{on/off}" , "text", command_example1, "Starts/stops a continuous task." },
    // { "ex2", "{VALUE}" , "hex", command_example2, "Execute a task with a hex value input." },
    { "con", "-" , "-", command_connect, "Connects to VID:PID 2047:0301 device." },
    { "geti", "-" , "-", command_geti, "Gets one irq IN query." },
    { "getr", "-" , "-", command_getr, "Gets one irq IN query." },
    { "help", "-" , "-", command_help,   "Display list of commands." },
    { "info", "-" , "-", command_info, "Get device info of the VID:PID 2047:0301 device." },
    { "irq", "{on/off}" , "text", command_irq, "Starts/stops periodic interrupt IN endpoint query." },
    { "lst", "-" , "-", command_list, "Lists all USB devices." },
    { "q", "-" , "-", command_quit,   "Disconnects device and quits application." }, 
    { "setr", "{VALUE}" , "hex", command_setr, "Sends a control out report (1 byte)." },
    { 0, 0, 0, 0, 0} // Don't touch it, last entry must be a terminating NULL entry
};