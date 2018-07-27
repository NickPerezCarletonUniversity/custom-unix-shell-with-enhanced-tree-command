//how to make a shell, code taken from https://www.geeksforgeeks.org/making-linux-shell-c/
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>



//parts taken from https://stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c
//https://stackoverflow.com/questions/5525668/how-to-implement-readlink-to-find-the-path
//for symbolic link and readfile()
//needed for ls impementation
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

//list of escape codes for colours 
// see https://misc.flogisoft.com/bash/tip_colors_and_formatting for list of colours

#define NORMAL "\x1B[0m"
#define RED  "\e[91m"
#define GREEN  "\e[92m"
#define YELLOW  "\e[93m"
#define CYAN  " \e[96m"
#define BLUE  "\e[94m"
#define DARKERBLUE "\e[38;5;38m"

int countlog = 0;


//taken from https://stackoverflow.com/questions/3536153/c-dynamically-growing-array
typedef struct {
  char **array;
  size_t used;
  size_t size;
} Array;

void initArray(Array *a, size_t initialSize) {\
  a->array = (char **)malloc(initialSize * sizeof(char *));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(Array *a, char * element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (a->used == a->size) {
    a->size *= 2;
    a->array = (char **)realloc(a->array, a->size * sizeof(char *));
  }
  a->array[a->used++] = element;
}

void freeArray(Array *a) {
  for (int i = 0; i < a->used; i++){
    free(a->array[i]);
  }
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

Array treeEntries;

int treeEntriesInUse = 0;

int treeLineNumber = 1;
int treeDepth = 0;
int maxLineNumber = 999;
int maxDepth = -1;

char* currDirectory;

int checkIfSymbolic(char* fileName);
void myTreeHelper();
void myTree();
void myTreeRecursion(char* subdirectory);
char* concatonate(char* s1, char* s2);

void myTreeHelper()
{
    DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;

    char buf[512];


    mydir = opendir(currDirectory);

    while((myfile = readdir(mydir)) != NULL)
    {

    char* checkForDir = concatonate(currDirectory, concatonate("/", myfile->d_name));

    if(checkIfSymbolic(checkForDir) == 1){
    	printf(CYAN "%s is a symbolic link\n", myfile->d_name);
        free(checkForDir);
    	continue;
    }

	if (myfile->d_name[0] == 46){
		continue;
	}

	char tempChar[treeDepth * 2];

	if(treeDepth != 0){
            printf(YELLOW "%d", treeLineNumber);
            for (int i = 0; i < 3 - floor(log10(abs(treeLineNumber))); i++){
              printf(" ");
            }
            for (int i = 0; i < treeDepth-1; i++){
              printf(YELLOW "│  ");
            }
            printf(YELLOW "├──");

            if(opendir(checkForDir))
            	printf(BLUE "%s\n", myfile->d_name);
            else
            	printf(GREEN "%s\n", myfile->d_name);

	}else{
	    printf(YELLOW "%d", treeLineNumber);
            for (int i = 0; i < 3 - floor(log10(abs(treeLineNumber))); i++){
              printf(NORMAL " ");
            }

            if(opendir(checkForDir))
            	printf(BLUE "%s\n", myfile->d_name);
            else
            	printf(GREEN "%s\n", myfile->d_name);
	}
	
	treeLineNumber++;
	DIR *tempdir;

	insertArray(&treeEntries, checkForDir);

	tempdir = opendir(checkForDir);

	if (tempdir != NULL){
	    myTreeRecursion(myfile->d_name);
	}
    }
    closedir(mydir);
}

void myTree(){

    treeEntriesInUse = 1;
    
    currDirectory = get_current_dir_name();

    freeArray(&treeEntries);
    initArray(&treeEntries, 1);
    treeLineNumber = 1;
    treeDepth = 0;
    myTreeHelper();
    free(currDirectory);
    maxDepth = -1;
    maxLineNumber = 999;
}

void stripLastDirectory(char* pathToStrip){
    for (int i = strlen(pathToStrip) - 1; i >= 0; i--){
        pathToStrip[i] = '\0';
        if (pathToStrip[i - 1] == '/'){
	    pathToStrip[i - 1] = '\0';
	    break;
        }
    }
}

void myTreeRecursion(char* subdirectory){
    char* tempPointer = currDirectory;
    currDirectory = concatonate(currDirectory, concatonate("/", subdirectory));
    free(tempPointer);
    treeDepth++;
    if((maxDepth != -1 & treeDepth < maxDepth) | maxDepth == -1){
      myTreeHelper(currDirectory);
    }
    treeDepth--;
    stripLastDirectory(currDirectory);
}

int checkIfSymbolic(char* fileName){
    char buffer[2000];
    memset(buffer, 0, sizeof(buffer));

    if(!access(fileName,F_OK)){
        if (readlink(fileName, buffer, sizeof(buffer)-1) >= 0){
            return 1;
        }else{
            return -1;
        }
    }else{
        return -1;
    }
}

 
#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
 
// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")
 
// Greeting shell during startup
void init_shell()
{
    clear();
    printf(NORMAL "");
    printf("\n\n\n\n******************"
        "************************");
    printf("\n\n\n\t****MY SHELL****");
    printf("\n\n\t-USE AT YOUR OWN RISK-");
    printf("\n\n\n\n*******************"
        "***********************");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    clear();
}
 
// Function to take input
int takeInput(char* str)
{
    char* buf;
 
    buf = readline("\n>>> ");
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}
 
// Function to print Current Directory.
void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf(GREEN"\nDir: %s", cwd);
    printf(NORMAL "");
    return;
}
 
// Function where the system command is executed
void execArgs(char** parsed)
{

    // Forking a child
    pid_t pid = fork(); 
 
    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf(RED "\nCould not execute command..");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL); 
        return;
    }
}
 
// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2]; 
    pid_t p1, p2;
 
    if (pipe(pipefd) < 0) {
        printf(RED "\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf(RED "\nCould not fork");
        return;
    }
 
    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
 
        if (execvp(parsed[0], parsed) < 0) {
            printf(RED "\nCould not execute command 1..");
            exit(0);
        }
    } else {
        // Parent executing
        p2 = fork();
 
        if (p2 < 0) {
            printf(RED "\nCould not fork");
            return;
        }
 
        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf(RED "\nCould not execute command 2..");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}
 
// Help command builtin
void openHelp()
{
    puts("\n***WELCOME TO SHELL HELP***"
        "\n Author: Nick Perez & Catherine Daigle"
        "\nList of Commands supported:"
        "\n>tree - optional first argument for tree depth ex: \"tree 5\"\n"  
           "for depth of 5. Also, after typing in tree, you can use the\n"
           "numbered aliases next to each entry to refer to each given\n"
           "file/directory. Example, \"cd 8\" would change directories\n"
           "to the corresponding directory (if it exists) in the last\n"
           "call to tree. To avoid using the aliases, type a \"\\\"\n"
           "before the alias, to interpret as a number. Example: \"tree \\5\"\n"
           "in the case that you wanted to specify a depth of 5 for tree\n"
           "but 5 was an alias from a previous call to tree.\n"
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\n>history"
        "\n>link? - checks for symbolic links"
        "\n>all other general commands available in UNIX shell"
        "\n>pipe handling"
        "\n>improper space handling");
 
    return;
}

char* concatonate(char* s1, char* s2){
	char * output = (char *) malloc(strlen(s1)+strlen(s2)+1);

	strcpy(output, s1);
	strcat(output, s2);

	return output;
}


void logger(char* str){
	char* home = getenv("HOME");
	char* path = concatonate(home, "/Documents/History");


	FILE *fl = fopen(path, "a"); 

	if(fl == NULL){
		printf("could not open file!");
		return;

	}

	fprintf(fl, "Line: %d  %s\n", countlog, str);
	fclose(fl);
	countlog++;
        free(path);
	return;
}

void history(){
    char* home = getenv("HOME");
    char* path = concatonate(home, "/Documents/History");


    int str;

    FILE *fl = fopen(path, "r"); 

    if(fl == NULL){
        printf(RED "could not open file!");
        return;

    }

    while ((str = getc(fl)) != EOF){
        putchar(str);
    }

    fclose(fl);
    free(path);
    return;
}

// Function to execute builtin commands
int ownCmdHandler(char** parsed)
{
    char* home = getenv("HOME");
    char* path = concatonate(home, "/Documents/History");

    int NoOfOwnCmds = 7, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];
    char* username;
 
    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "tree";
    ListOfOwnCmds[5] = "link?";
    ListOfOwnCmds[6] = "history";
 	
 	logger(parsed[0]);

    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }
 
    switch (switchOwnArg) {
    case 1:

        printf(RED "\nGoodbye\n");
        if(remove(path)){printf(RED "Could not remove the file\n");}
        free(path);
	freeArray(&treeEntries);
        exit(0);

    case 2:
        free(path);
    	if(parsed[1] == NULL){
    		chdir(getenv("HOME"));
        }
        else{
        	chdir(parsed[1]);	
        }
        return 1;
    case 3:
        free(path);
        openHelp();
        return 1;
    case 4:
        free(path);
        username = getenv("USER");
        printf("\nHello %s.\nMind that this is "
            "not a place to play around."
            "\nUse help to know more..\n",
            username);
        return 1;
    case 5:
        free(path);
        if (parsed[1] != NULL) {
          maxDepth = atoi(parsed[1]);
          if (maxDepth <= 0) {
            printf(RED "invalid tree depth\n");
            maxDepth = -1;
          }else {
            myTree();
          }
        }else {
          myTree();
        }

        return 1;

    case 6:
        free(path);
        if(parsed[1] != NULL){
        	printf(NORMAL "Checking symbolic link for %s \n", parsed[1]);
        	if(checkIfSymbolic(parsed[1]) == 1){
        		printf(CYAN "Symbolic Link Exists!\n");
        	}
        	else{
        		printf(RED "Symbolic Link does not Exist\n");
        	}
        }
        else{
        	printf(NORMAL "State a file you want to check for a symbolic link\n");
        }
        return 1;
    case 7:
        free(path);
    	history();
    	return 1;

    default:
        free(path);
        break;
    }
 
    return 0;
}
 
// function for finding pipe
int parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }
 
    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else {
        return 1;
    }
}

void convertToShortcut(char** parsedPortion){
    int index;
    if (!(*parsedPortion[0] == 92)){
        index = atoi(*parsedPortion);
        if (index && (index <= treeEntries.used)){
            *parsedPortion = treeEntries.array[index - 1];
        }
    }else {
        memmove(*parsedPortion, *parsedPortion + 1, strlen(*parsedPortion));
    }
}
 
// function for parsing command words
void parseSpace(char* str, char** parsed)
{
    int i;
 
    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");
 
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }

    for (int j = 0; j < i; j++){
        convertToShortcut(&parsed[j]);
    }
}
 
int processString(char* str, char** parsed, char** parsedpipe)
{
 
    char* strpiped[2];
    int piped = 0;
 
    piped = parsePipe(str, strpiped);
 
    if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);
 
    } else {
 
        parseSpace(str, parsed);
    }
 
    if (ownCmdHandler(parsed))
        return 0;
    else
        return 1 + piped;
}
 
int main()
{
    char* path = concatonate(getenv("HOME"), "/Documents/History");

    if(remove(path)){printf(RED "Could not remove the file\n");}

    initArray(&treeEntries, 0);

    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    init_shell();
 
    while (1) {
        // print shell line
        printDir();
        // take input
        if (takeInput(inputString))
            continue;
        // process
        execFlag = processString(inputString,
        parsedArgs, parsedArgsPiped);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.
 
        // execute
        if (execFlag == 1)
            execArgs(parsedArgs);
 
        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}
