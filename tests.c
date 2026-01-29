#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <dirent.h>


int main() {
    DIR *dir;
    struct dirent *entry;
    char *files[100];
    char tmp[100];
    int w;
    dir = opendir(".");
    if(dir == NULL) {
        perror("opendir");
    }
    w = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (w>=2)
            files[w-2] = strdup(entry->d_name);
        w++;
    }
    closedir(dir);
}
        