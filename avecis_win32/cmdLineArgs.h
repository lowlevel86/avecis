#define string_arg 0
#define int_arg 1
#define float_arg 2
#define only_flag 3

void readCmdLineArgs(char *, char *, int *, int, void ***);
void mergeCmdLineArgs(char *[], int, char **);
void freeCmdLineArgs();
