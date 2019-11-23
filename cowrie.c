// cowrie.c a simple shell


// PUT YOUR HEADER COMMENT HERE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


// PUT EXTRA `#include'S HERE
#include <spawn.h>
#include <sys/wait.h>


#define MAX_LINE_CHARS 1024
#define INTERACTIVE_PROMPT "cowrie> "
#define DEFAULT_PATH "/bin:/usr/bin"
#define WORD_SEPARATORS " \t\r\n"
#define DEFAULT_HISTORY_SHOWN 10

// These characters are always returned as single words
#define SPECIAL_CHARS "!><|"


// PUT EXTRA `#define'S HERE


static void execute_command(char **words, char **path, char **environment);
static void do_exit(char **words);
static int is_executable(char *pathname);
static char **tokenize(char *s, char *separators, char *special_chars);
static void free_tokens(char **tokens);


// PUT EXTRA FUNCTION PROTOTYPES HERE
void append_to_history(char **words);
void print_history(int lines);

int main(void) {
    //ensure stdout is line-buffered during autotesting
    setlinebuf(stdout);

    // Environment variables are pointed to by `environ', an array of
    // strings terminated by a NULL value -- something like:
    //     { "VAR1=value", "VAR2=value", NULL }
    extern char **environ;

    // grab the `PATH' environment variable;
    // if it isn't set, use the default path defined above
    char *pathp;
    if ((pathp = getenv("PATH")) == NULL) {
        pathp = DEFAULT_PATH;
    }
    char **path = tokenize(pathp, ":", "");

    char *prompt = NULL;
    // if stdout is a terminal, print a prompt before reading a line of input
    if (isatty(1)) {
        prompt = INTERACTIVE_PROMPT;
    }

    // main loop: print prompt, read line, execute command
    while (1) {
        if (prompt) {
            fputs(prompt, stdout);
        }

        char line[MAX_LINE_CHARS];
        if (fgets(line, MAX_LINE_CHARS, stdin) == NULL) {
            break;
        }

        char **command_words = tokenize(line, WORD_SEPARATORS, SPECIAL_CHARS);
        execute_command(command_words, path, environ);
        free_tokens(command_words);

    }

    free_tokens(path);
    return 0;
}


//
// Execute a command, and wait until it finishes.
//
//  * `words': a NULL-terminated array of words from the input command line
//  * `path': a NULL-terminated array of directories to search in;
//  * `environment': a NULL-terminated array of environment variables.
//
static void execute_command(char **words, char **path, char **environment) {
    assert(words != NULL);
    assert(path != NULL);
    assert(environment != NULL);

    char *program = words[0];

    if (program == NULL) {
        // nothing to do
        return;
    }

    if (strcmp(program, "exit") == 0) {
        do_exit(words);
        append_to_history(words);
        // do_exit will only return if there is an error
        return;
    }

    // ADD CODE HERE TO IMPLEMENT SUBSET 0 //////////////////// 0 //////////// 0 ///////////// 0 ////////////// 0 /////////////////// 0 ////////////////////////////
    int executed_checker = 0;
    if (strcmp(program, "cd") == 0) {
        if(words[1]== NULL){
            chdir(getenv("HOME"));
        }
        if(words[1] != NULL){
           if(chdir(words[1]) != 0){
                //printf("cd: %s: No such file or dirrectory\n",words[1]);
                fprintf(stderr, "cd: %s: No such file or directory\n", words[1]);
           }    
        }
        append_to_history(words);
        return;
    }
    if (strcmp(program, "pwd") == 0) {
        char buff[200];
        char *curr_dir = getcwd(buff, 200);
        
        // char * cur_dir = get_current_dir_name(void);
        printf("current directory is '%s'\n", curr_dir);
        append_to_history(words);
        return;
    }

    if (strcmp(program, "history") == 0) {
        int lines = 3;
        print_history(lines);
        executed_checker = 1;
    }
    /*if (strcmp(program, "!") == 0) {
        char buff[200];
        char *curr_dir = getcwd(buff, 200);
        
        // char * cur_dir = get_current_dir_name(void);
        printf("current directory is '%s'\n", curr_dir);
        return;
    }*/
   
    //////dont forget error messages ///////
    ////////////////////////////////////////



    // CHANGE CODE BELOW HERE TO IMPLEMENT SUBSET 1 ///////////////// 1 //////////// 1 ////////////// 1 ////////////// 1 /////////////// 1 ///////////////////////  

    if (strrchr(program, '/') == NULL) { // this executes if there is no "/" found ie, we gota make the path
        
        int x = 0;
        while (path[x] != NULL){
            char *path_dirrec_1 = path[x]; //idk why we need this , cant you just fopen diary
            char *prog_name_1 = words[0];
            char pathname_1[200];
            snprintf(pathname_1, sizeof pathname_1,"%s/%s",path_dirrec_1,prog_name_1 ); // here we got a possiblie pathname that we gota check 
            //printf("pathname has %s\n",pathname_1); 
                if (is_executable(pathname_1)) { //this just checks if it is
                    pid_t pid;
                    posix_spawn(&pid, pathname_1 , NULL, NULL, words, environment); //probs gota change words here
                    int exit_status;
                    if (waitpid(pid, &exit_status, 0) == -1) {
                        perror("waitpid");
                        return;
                    } 
                    exit_status = WEXITSTATUS(exit_status); 
                    printf("%s exit status = %d\n",pathname_1, exit_status);
                    executed_checker = 1;
                    append_to_history(words);
                    return;
                }        
            x-=-1; //heehehehe
        }
        
        
        
        //contruct pathname
        //call is_executeable
        //posix_spawn(&pid, "/bin/date", NULL, NULL, date_argv, environ)
    }
    //basically with the path** variable, you while through it like path[0], path[1] and each time you append the word* to it then is exectuable it to see if it works
    //if it already has a / in the name, you can just strait run the program
    
    if (is_executable(program)) { //this just checks if it is
        pid_t pid;
        posix_spawn(&pid, program , NULL, NULL, words, environment);
        int exit_status;
        if (waitpid(pid, &exit_status, 0) == -1) {
            perror("waitpid");
            return;
        }
        exit_status = WEXITSTATUS(exit_status);    
        printf("%s exit status = %d\n",program, exit_status);
        executed_checker = 1;
        append_to_history(words);
        
    } 
    if (executed_checker == 0) {
        fprintf(stderr, "%s: command not found\n", program);
        append_to_history(words);
    }
    
}


// PUT EXTRA FUNCTIONS HERE
void append_to_history(char **words){

    char *value = getenv("HOME");  
    char pathname[100];
    snprintf(pathname, sizeof pathname,"%s/%s",value,".cowrie_history" );
    printf("this is the pathname of this history thing %s\n",pathname);
    FILE *fd = fopen(pathname,"a");
    
    int x = 0;
    while(words[x] != NULL){

        fputs(words[x],fd);
        fputs(" ",fd);
        x-=-1;


    }
    fputs("\n",fd);
    fclose(fd);
}
void print_history(int lines){ //lines will be n
    //printf("we got into the print history function\n");
    char *value = getenv("HOME");  
    char pathname[100];
    snprintf(pathname, sizeof pathname,"%s/%s",value,".cowrie_history" );
    FILE *fd = fopen(pathname,"r");
    //gota run through all history lines to see where the bottom is, then start priniting from bottom - in in a second while loop

    //find bottom
    int total_lines = 0; 
    char str[256];
    while ((fgets(str, 256, fd)) != NULL){
       // printf("we reading lines\n"); ////
        total_lines++;
    }
    //printf("total_lines is %d\n",total_lines); /////
    
    //int a = fseek(fd, 0, 1);
    //printf("return val of seek was %d\n",a);
    fclose(fd);
    fd = fopen(pathname,"r");
    //once here we will have the total number of lines in x, so basically write a while loop that keeps fgetsing but only fputs when inside right amount
    int first_print_line = total_lines - lines;
    int x = 0;
    //printf("first_print_line is %d\n",first_print_line); /////
    
    while ((fgets(str, 256, fd)) != NULL){
        //printf("we in the second while\n"); /////
        if(x >= first_print_line){
            printf("%d: ",x);
            fputs(str, stdout);
            //printf("\n");
        }
        x++;
    } 

    fclose(fd);
}




//
// Implement the `exit' shell built-in, which exits the shell.
//
// Synopsis: exit [exit-status]
// Examples:
//     % exit
//     % exit 1
//
static void do_exit(char **words) {
    int exit_status = 0;

    if (words[1] != NULL) {
        if (words[2] != NULL) {
            fprintf(stderr, "exit: too many arguments\n");
        } else {
            char *endptr;
            exit_status = (int)strtol(words[1], &endptr, 10);
            if (*endptr != '\0') {
                fprintf(stderr, "exit: %s: numeric argument required\n",
                        words[1]);
            }
        }
    }

    exit(exit_status);
}


//
// Check whether this process can execute a file.
// Use this function when searching through the directories
// in the path for an executable file
//
static int is_executable(char *pathname) {
    struct stat s;
    return
        // does the file exist?
        stat(pathname, &s) == 0 &&
        // is the file a regular file?
        S_ISREG(s.st_mode) &&
        // can we execute it?
        faccessat(AT_FDCWD, pathname, X_OK, AT_EACCESS) == 0;
}


//
// Split a string 's' into pieces by any one of a set of separators.
//
// Returns an array of strings, with the last element being `NULL';
// The array itself, and the strings, are allocated with `malloc(3)';
// the provided `free_token' function can deallocate this.
//
static char **tokenize(char *s, char *separators, char *special_chars) {
    size_t n_tokens = 0;
    // malloc array guaranteed to be big enough
    char **tokens = malloc((strlen(s) + 1) * sizeof *tokens);


    while (*s != '\0') {
        // We are pointing at zero or more of any of the separators.
        // Skip leading instances of the separators.
        s += strspn(s, separators);

        // Now, `s' points at one or more characters we want to keep.
        // The number of non-separator characters is the token length.
        //
        // Trailing separators after the last token mean that, at this
        // point, we are looking at the end of the string, so:
        if (*s == '\0') {
            break;
        }

        size_t token_length = strcspn(s, separators);
        size_t token_length_without_special_chars = strcspn(s, special_chars);
        if (token_length_without_special_chars == 0) {
            token_length_without_special_chars = 1;
        }
        if (token_length_without_special_chars < token_length) {
            token_length = token_length_without_special_chars;
        }
        char *token = strndup(s, token_length);
        assert(token != NULL);
        s += token_length;

        // Add this token.
        tokens[n_tokens] = token;
        n_tokens++;
    }

    tokens[n_tokens] = NULL;
    // shrink array to correct size
    tokens = realloc(tokens, (n_tokens + 1) * sizeof *tokens);

    return tokens;
}


//
// Free an array of strings as returned by `tokenize'.
//
static void free_tokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}