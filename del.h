typedef struct obj
{
    char** agv;
    int type;
    int agc;
    struct obj* next;
}obj,*obj_ptr;

typedef struct history_stack
{
    char** begin;
    int num;
    int size;
}new_table;

extern int parser(char* input);
extern int is_symbol(char*cur,obj_ptr cur_obj);
extern void set_free(obj_ptr headp);
extern void find_file();
extern int is_builtin_cmd(int* error);
extern void history(int n);
extern void push(char* input);
extern int is_bg();
extern void is_open_correct(int fd);
extern void solve();
extern int cd_cmd(obj_ptr);
extern int count_of_pipe();
