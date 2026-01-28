#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shellmemory.h"
#include "shell.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int MAX_ARGS_SIZE = 3;

int badcommand() {
    printf("Unknown Command\n");
    return 1;
}

int badcommand_mkdir() {
    printf("Bad command: my_mkdir\n");
    return 1;
}

int badcommand_cd(){  
    printf("Bad command: my_cd\n");
    return 1;
}

// For source command only
int badcommandFileDoesNotExist() {
    printf("Bad command: File not found\n");
    return 3;
}

int help();
int quit();
int set(char *var, char *value);
int print(char *var);
int source(char *script);
int badcommandFileDoesNotExist();

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size) {
    int i;

    if (args_size < 1 || args_size > MAX_ARGS_SIZE) {
        return badcommand();
    }

    for (i = 0; i < args_size; i++) {   // terminate args at newlines
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help") == 0) {
        //help
        if (args_size != 1)
            return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit") == 0) {
        //quit
        if (args_size != 1)
            return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set") == 0) {
        //set
        if (args_size != 3)
            return badcommand();
        return set(command_args[1], command_args[2]);

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2)
            return badcommand();
        return print(command_args[1]);
    //ECHO command
    } else if (strcmp(command_args[0], "echo") == 0) {
        if (args_size != 2)
            return badcommand();
        char firstchar = command_args[1][0];
        if (firstchar=='$') {
            char *mvar = mem_get_value(command_args[1]+1);
            if (strcmp(mvar, "Variable does not exist")==0)
                printf("\n");
            else {
                printf("%s\n", mvar);
            }
        } else {
            printf("%s\n", command_args[1]);   
        }
        return 0;
    //my_ls command
    } else if(strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1)
            return badcommand();
        DIR *dir;
        struct dirent *entry;
        dir = opendir(".");
        if(dir == NULL) {
            perror("opendir");
            return 1;
        }

        while ((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }

        closedir(dir);

        return 0;
    //my_mkdir command
    } else if(strcmp(command_args[0], "my_mkdir")==0){
        if (args_size != 2)
            return badcommand();
        char firstchar = command_args[1][0];
        if (firstchar=='$') {
            char *mvar = mem_get_value(command_args[1]+1);
            if (strcmp(mvar, "Variable does not exist")==0)
                badcommand_mkdir();
            else {
                //Create directory with name *mvar
                mkdir(mvar, 0755); // trust it, it compiles
            }
        } else {
            //create directory with name command_args[1]
            mkdir(command_args[1],0755);
        }
        return 0;

    //my_touch command BE CAREFUL OVERWRITES EXISTING FILE!!!!
    } else if (strcmp(command_args[0],"my_touch")==0){
        if (args_size != 2)
            return badcommand();
        FILE *fp;
        fp = fopen(command_args[1],"w");
        fclose(fp);
        return 0;

    //my_cd command
    } else if (strcmp(command_args[0],"my_cd")==0){
        if (args_size != 2)
            return badcommand();

        if (chdir(command_args[1]) != 0) {
            badcommand_cd();
        }

        return 0;
        
            
    } else if (strcmp(command_args[0], "source") == 0) {
        if (args_size != 2)
            return badcommand();
        return source(command_args[1]);

    } else
        return badcommand();
}

int help() {

    // note the literal tab characters here for alignment
    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
source SCRIPT.TXT	Executes the file SCRIPT.TXT\n ";
    printf("%s\n", help_string);
    return 0;
}

int quit() {
    printf("Bye!\n");
    exit(0);
}

int set(char *var, char *value) {
    // Challenge: allow setting VAR to the rest of the input line,
    // possibly including spaces.

    // Hint: Since "value" might contain multiple tokens, you'll need to loop
    // through them, concatenate each token to the buffer, and handle spacing
    // appropriately. Investigate how `strcat` works and how you can use it
    // effectively here.

    mem_set_value(var, value);
    return 0;
}


int print(char *var) {
    printf("%s\n", mem_get_value(var));
    return 0;
}

int source(char *script) {
    int errCode = 0;
    char line[MAX_USER_INPUT];
    FILE *p = fopen(script, "rt");      // the program is in a file

    if (p == NULL) {
        return badcommandFileDoesNotExist();
    }

    fgets(line, MAX_USER_INPUT - 1, p);
    while (1) {
        errCode = parseInput(line);     // which calls interpreter()
        memset(line, 0, sizeof(line));

        if (feof(p)) {
            break;
        }
        fgets(line, MAX_USER_INPUT - 1, p);
    }

    fclose(p);

    return errCode;
}
