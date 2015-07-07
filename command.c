#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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


// Command 'example1'
static int command_example1(int argc, char *argv[]) {

    if (argc < 2) {
        printf("error: missing argument.\n");
        return (0);
    }

    if (!strcmp (argv[1], "on")) {
        //TODO
    } else if (!strcmp (argv[1], "off")) {
        //TODO
    } else
        printf("error: invalid input.\n");

    return(0);
}

// Command 'example2'
static int command_example2(int argc, char *argv[]) {
    long num;
    char *endptr;
   
    if (argc < 2) {
        printf("error: missing value.\n");
        return (0);
    }

    errno = 0;
    num = strtol(argv[1], &endptr, 16);

    if (*endptr != 0 || errno != 0) {
        printf("error: invalid input.\n");
        return (0);
    }

    // TODO: num contains a valid value, now do something with it
    
    return(0);
}

// Command 'help'
static int command_help(int argc, char *argv[]) {
	commandentry_t *entry;

    printf("\nAvailable commands\n");
    printf("-------------------------------------------------------------------------------------------------\n");
    printf("%s\t%s\t%s\t%s\n", "Command", "Args", "Format", "Description");
    printf("-------------------------------------------------------------------------------------------------\n");

    entry = &commandtable[0]; // Point at the beginning of the command table.

    while(entry->commandname_ptr) {

        // Print the command name and the brief description.
        printf("%s\t%s\t%s\t%s\n", 
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

static int command_quit(int argc, char *argv[]) {

	usb_disconnect();
	printf("Quitting...\n");
    exit(0);
}



// Command table entries - fill it!
commandentry_t commandtable[] = {

    { "ex1", "{on/off}" , "ascii", command_example1, "Starts/stops a continuous task." },
    { "ex2", "{VALUE}" , "hex", command_example2, "Execute a task with a hex value input." },
    { "help", "-" , "-", command_help,   "Display list of commands." },
    { "quit", "-" , "-", command_quit,   "Quits application." }, 
   
    { 0, 0, 0, 0, 0} // Don't touch it, last entry must be a terminating NULL entry
};