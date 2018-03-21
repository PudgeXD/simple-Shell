#ifndef HEAD
#define HEAD

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<signal.h>

#include"del.h"
#endif // head

new_table table;
obj_ptr headp = NULL;
obj_ptr tailp = NULL;
int bg;
int main()
{
    table.size = 0;
    table.num = 0;
    table.begin = NULL;
    while(1)
    {
        int error = 0;
        bg = 0;
        char notation = '$';
        char* input = (char*)malloc(sizeof(char)*128);
        printf("%c ",notation);
        fflush(stdin);
        if(!fgets(input,128,stdin))
        {
            printf("error: fgets");
            continue;
        }
        push(input);
      	error = parser(input);
	bg = is_bg();
        if(error)
        {
            set_free(headp);
            continue;
        }

        if(is_builtin_cmd(&error))
        {
            if(error) continue;
        }
        else
            solve();
        set_free(headp);
    }
    return 0;
 }
