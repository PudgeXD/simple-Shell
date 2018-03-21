#ifndef HEAD
#define HEAD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<signal.h>

#include "del.h"

#endif // head
extern obj_ptr headp;
extern obj_ptr tailp;
extern struct history_stack table;
extern int bg;
int parser(char *input)
{
    char *cur = input;
    char *token = cur;
    int end = 0;
    int symbol = 0;
    obj_ptr cur_obj = NULL;
    headp = (obj_ptr)malloc(sizeof(obj));
    if(!headp)
    {
    	perror("headp malloc");
    	return 1;
    }
    cur_obj = headp;
    tailp = cur_obj;
    cur_obj->agv = (char **)malloc(sizeof(char *) * 10);
    if(!cur_obj->agv)
    {
    	perror("cur_obj->agv malloc");
    	return 1;
    }
    cur_obj->next = NULL;
    cur_obj->type = 0;
    cur_obj->agc = 0;
    if ((*input) == ' ' || (*input) == '\n')
    {
        printf("invalid input\n");
        return 1;
    }
    while ((*cur) != '\n')
    {
        symbol = 0;
        while (((*cur) != ' ') && ((*cur) != '\n'))
            cur++;
        if ((*cur) == '\n')
            end = 1;
        *cur = '\0';
        cur_obj->agv[cur_obj->agc] = token;
        cur_obj->agc++;
        if (end)
        {
            cur_obj->agv[cur_obj->agc] = NULL;
            return 0;
        }
        cur++;
        token = cur;
        symbol = is_symbol(cur, cur_obj);

        while (symbol == 0)
        {
            while (((*cur) != ' ') && ((*cur) != '\n'))
                cur++;
            if ((*cur) == '\n')
                end = 1;
            *cur = '\0';
            cur_obj->agv[cur_obj->agc] = token;
            cur_obj->agc++;
            if (end)
            {
                cur_obj->agv[cur_obj->agc] = NULL;
                return 0;
            }
            cur++;
            token = cur;
            symbol = is_symbol(cur, cur_obj);
        }

        if (symbol == -1)
            return 1;
        cur += 2;
        if (symbol == 3)
            cur++;
        token = cur;
        if ((*cur < 'A') || (*cur > 'z') || ((*cur > 'Z') && (*cur < 'a')))
        {
            printf("type error: The name of file/program must start with letter\n");
            return 1;
        }
        cur_obj->agv[cur_obj->agc] = NULL;
        cur_obj->next = (obj_ptr)malloc(sizeof(obj));
        if(!cur_obj->next)
    	{
    		perror("cur_obj->next malloc");
    		return 1;
    	}
        if (((cur_obj->type) >= 1) && ((cur_obj->type) <= 3))
            cur_obj->next->type = -1;
        else
            cur_obj->next->type = 0;
        cur_obj = cur_obj->next;
        tailp = cur_obj;

        cur_obj->agv = NULL;
        cur_obj->next = NULL;
        cur_obj->agc = 0;
        cur_obj->agv = (char **)malloc(sizeof(char *) * 10);
        if(!cur_obj->agv)
    	{
    		perror("cur_obj->agv malloc");
    		return 1;
   		}
    }
    //set_in_out(headp);
    return 0;
}

int is_symbol(char *cur, obj_ptr cur_obj)
{
    char *token = cur;
    if (*token == '<')
    {
        cur_obj->type = 1;
        cur++;
        if (*cur != ' ')
        {
            printf("type error: no space after symbol '<'\n");
            return -1;
        }
        return 1;
    }
    else if (*token == '|')
    {
        cur_obj->type = 4;
        cur++;
        if (*cur != ' ')
        {
            printf("type error: no space after symbol '|' \n");
            return -1;
        }
        return 4;
    }
    else if (*token == '>')
    {
        cur++;
        if ((*token) == (*cur)) // >>
        {
            cur_obj->type = 3;
            cur++;
            if (*cur != ' ')
            {
                printf("3:type error: no space after symbol '>>'\n");
                return -1;
            }
            return 3;
        }
        else if (*cur == ' ') // >
        {
            cur_obj->type = 2;
            return 1;
        }
        else
        {
            printf("type error: no space after symbol '>'\n");
            return -1;
        }
    }
    return 0;
}

void set_free(obj_ptr headp)
{
    obj_ptr tmp = NULL;
    while (tmp != NULL)
    {
        tmp = headp->next;
        free(headp);
        headp = tmp;
    }
}

int is_builtin_cmd(int *error)
{
    char *cmd = headp->agv[0];
    if (!strcmp(cmd, "cd"))
    {
        *error = cd_cmd(headp);
        return 1;
    }
    else if (!strcmp(cmd, "history"))
    {
        if (headp->agc > 2)
        {
            *error = 1;
            printf("error: parameter count is over\n");
            return 1;
        }
        if (headp->agv[1] != NULL)
        {
            history(atoi(headp->agv[1]));
            return 1;
        }
        else
        {
            history(table.num);
            return 1;
        }
    }
    else if (!strcmp(cmd, "exit"))
    {
        exit(0);
    }
    else
        return 0;
}

void push(char *input)
{
    char **tmp;
    char **swap;
    int i;
    if (table.size == 0)
    {
        table.begin = (char **)malloc(sizeof(char *));
        table.begin[table.num] = (char *)malloc(sizeof(char) * 128);
        strcpy(table.begin[table.num], input);
        table.size = 1;
        table.num = 1;
    }
    else
    {
        if (table.size == table.num)
        {
            tmp = (char **)malloc(sizeof(char *) * table.size * 2);
            table.num++;
            tmp[table.size] = (char *)malloc(sizeof(char) * 128);
            strcpy(tmp[table.size], input);
            for (i = table.size - 1; i >= 0; i--)
            {
                tmp[i] = table.begin[i];
            }
            table.size = table.size * 2;
            swap = table.begin;
            table.begin = tmp;
            free(swap);
        }
        else
        {
            table.begin[table.num] = (char *)malloc(sizeof(char) * 128);
            strcpy(table.begin[table.num], input);
            table.num++;
        }
    }
}

void history(int n)
{
    int i;
    int count = 1;
    for (i = table.num - 1; i >= 0 && count <= n; i--, count++)
    {
        printf("%s", table.begin[i]);
    }
    if (count < n)
    {
        printf("the parameter is oversize\n");
    }
}
int is_bg()
{
    obj_ptr cur = tailp;
    char *back = "&";
    if ((cur->agc>1)&& (!strcmp(cur->agv[cur->agc - 1], back)))
    {
    	cur->agv[cur->agc-1] = NULL;
        return 1;
    }
    return 0;
}

int cd_cmd(obj_ptr node)
{
    int para_count = node->agc - 1;
    if (para_count > 1)
    {
        printf("error : the count of parameter must be one\n");
        return 1;
    }
    else if (para_count == 0)
    {
        printf("error : no parameter after cd\n");
        return 1;
    }
    else
    {
        if (chdir(node->agv[1]))
        {
            printf("error : can't change to %s\n", node->agv[1]);
            return 1;
        }
        return 0;
    }
}

void solve()
{
    if (headp->type == 0)
    {

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
        else if (pid == 0)
        {
            if (execvp(headp->agv[0], headp->agv) < 0)
            {
                printf("execvp error\n" );
                exit(1);
            }
        }
        else
        {
            int status = 0;
            if (bg)
                waitpid(pid, &status, WNOHANG);
            else
                waitpid(pid, &status, 0);
        }
    }
    else if (headp->type == 1)
    {

        pid_t pid = fork();
        obj_ptr in_obj = headp->next;
        int fd_out = 0;
        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
        else if (pid == 0)
        {
            int fd_in = open(in_obj->agv[0], O_RDONLY);
            is_open_correct(fd_in);
            close(0);
            dup2(fd_in, 0);
            close(fd_in);
            if (in_obj->type != -1)
            {
                obj_ptr out_obj = in_obj->next;
                if (in_obj->type == 2)
                    fd_out = open(out_obj->agv[0], O_WRONLY | O_CREAT | O_TRUNC);
                else if (in_obj->type == 3)
                    fd_out = open(out_obj->agv[0], O_WRONLY | O_CREAT | O_APPEND);
                else
                {
                    printf("error type: after '< file' must be '> file', '>> file' or nothing\n");
                    exit(1);
                }
                is_open_correct(fd_out);
                close(1);
                dup2(fd_out, 1);
                close(fd_out);
            }
            if (execvp(headp->agv[0], headp->agv) < 0)
            {
                printf("execvp error\n");
                exit(1);
            }
        }
        else
        {
            int status = 0;
            if (bg)
                waitpid(pid, &status, WNOHANG);
            else
                waitpid(pid, &status, 0);
        }
    }

    else if ((headp->type == 2) || (headp->type == 3))
    {
        int type = headp->type;
        obj_ptr out_obj = headp->next;
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
        else if (pid == 0)
        {
            int fd_out;
            if (type == 2)
                fd_out = open(out_obj->agv[0], O_WRONLY | O_CREAT | O_TRUNC);
            if (type == 3)
                fd_out = open(out_obj->agv[0], O_WRONLY | O_CREAT | O_APPEND);
            is_open_correct(fd_out);
            close(1);
            dup2(fd_out, 1);
            close(fd_out);
            if (execvp(headp->agv[0], headp->agv) < 0)
            {
                printf("execvp error\n");
                exit(1);
            }
        }
        else
        {
            int status = 0;
            if (bg)
                waitpid(pid, &status, WNOHANG);
            else
                waitpid(pid, &status, 0) ;
        }
    }

    else if(headp->type == 4)
    {
        obj_ptr cur = headp;
        int count = count_of_pipe();
        int i =0;
        int** fps = (int**)malloc(sizeof(int*)*count);
        if(!fps)
        {
        	perror("malloc of fps");
        	exit(1);
        }
        int j =0;
        int flag = 0;
        obj_ptr end;
        if(tailp->type !=0)
        	end = tailp;
        else
            end = NULL;

        while (cur != end)
        {
            fps[j] = (int*)malloc(sizeof(int)*2);
            if(pipe(fps[j]) < 0)
            {
                perror("pipe failed:");
                exit(1);
            }
            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork");
                exit(1);
            }
            else if (pid == 0)
            {
                if (cur->type == 4)
                {
                    close(1);
                    dup2(fps[j][1], 1);
                }
                close(fps[j][1]);
                if (cur != headp)
                {
                    close(0);
                    dup2(fps[j-1][0], 0);
                }
                close(fps[j][0]);
                if((cur->type == 2)||(cur->type == 3))
                {
                    int type = cur->type;
                    obj_ptr out_obj = cur->next;
                    int fd_out;
                    if (type == 2)
                        fd_out = open(out_obj->agv[0], O_WRONLY | O_CREAT | O_TRUNC);
                    if (type == 3)
                        fd_out = open(out_obj->agv[0], O_WRONLY | O_CREAT | O_APPEND);
                    is_open_correct(fd_out);
                    close(1);
                    dup2(fd_out,1);
                    close(fd_out);
                }
                if (execvp(cur->agv[0], cur->agv) < 0)
                {
                    printf("execvp error\n");
                    exit(1);
                }
            }
            else
            {
                int status = 0;
                waitpid(pid, &status, 0);
                cur = cur->next;
                close(fps[j][1]);
                j++;
            }
        }
    }

}

void is_open_correct(int fd)
{
    if (fd < 0)
    {
        printf("open file error\n");
        exit(1);
    }
}
int count_of_pipe()
{
	int count = 0;
	obj_ptr cur = headp;
	while(cur->type==4)
	{
		count++;
		cur = cur->next;
	}
	return count;
}
