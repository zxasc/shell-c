#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int builtin_exit(char **args);
int builtin_echo(char **args);
int builtin_type(char **args);
int builtin_pwd(char **args);
int builtin_cd(char **args);

char *builtin_str[] = {
    "exit",
    "echo",
    "type",
    "pwd",
    "cd"
};

int (*builtin_func[]) (char **) = {
    &builtin_exit,
    &builtin_echo,
    &builtin_type,
    &builtin_pwd,
    &builtin_cd,
};

int num_of_builtins = sizeof(builtin_func) / sizeof(char *);

char *get_input(void)
{
    char *input = malloc(100 * sizeof(char));
    if (!input) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    setbuf(stdout, NULL);
    printf("$ ");
    fgets(input, 100, stdin);

    input[strcspn(input, "\n")] = '\0';
    return input;
}

char *get_cmd(char *input_string)
{
    char *cmd = malloc(100 * sizeof(char));
    if (!cmd) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    sscanf(input_string, "%s", cmd);
    return cmd;
}

char **tokenize_args(char *input_string)
{
    char **args = malloc(100 * sizeof(char *));
    if (!args) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(input_string, " ");
    int i = 0;

    while (token) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    return args;
}

char **parse_input(char *input)
{
    char **input_arr = malloc(100 * sizeof(char *));
    if (!input_arr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(input, " ");
    int i = 0;

    while (token != NULL) {
        input_arr[i++] = token;
        token = strtok(NULL, " ");
    }
    input_arr[i] = NULL;

    return input_arr;
}

/*
 * Builtins
 */

int builtin_exit(char **input_arr)
{
    exit(EXIT_SUCCESS);
}

int builtin_echo(char **input_arr)
{
    for (int i = 1; i < sizeof(input_arr); i++) {
        if (input_arr[i] == NULL) break;
        printf("%s ", input_arr[i]);
    }
    printf("\n");

    return 1;
}

int builtin_type(char **input_arr)
{
    for (int i = 0; i < num_of_builtins; i++) {
        if(strcmp(input_arr[1], builtin_str[i]) == 0) {
            printf("%s is a shell builtin\n", input_arr[1]);
            return 1;
        }
    }

    int wasFound = 0;
    char *env_path = getenv("PATH");
    char *path = malloc(strlen(env_path));
    strcpy(path, env_path);
    char *dir_path = strtok(path, ":");

    while (wasFound == 0 && dir_path) {
        DIR *directory = opendir(dir_path);

        if (!directory) {
            dir_path = strtok(NULL, ":");
            continue;
        }

        struct dirent *file;
        while ((file = readdir(directory))) {
            if (strcmp(file->d_name, input_arr[1]) == 0) {
                printf("%s is %s/%s\n",input_arr[1], dir_path, file->d_name);
                wasFound = 1;
                break;
            }
        }
        dir_path = strtok(NULL, ":");
        closedir(directory);
        if (wasFound) break;
    }

    free(path);
    if (wasFound) return 1;
    printf("%s: not found\n", input_arr[1]);
    return 0;
}

int builtin_pwd(char **input_arr)
{
    printf("%s\n", getcwd(NULL, 0));
    return 1;
}

int builtin_cd(char **input_arr)
{
    if (strcmp(input_arr[1], "~") == 0) {
        chdir(getenv("HOME"));
        return 1;
    }
    if (access(input_arr[1], F_OK) == 0) {
        chdir(input_arr[1]);
        return 1;
    }
    printf("cd: %s: No such file or directory\n", input_arr[0]);
    return 0;
}

int launch_program(char *cmd, char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(cmd, args) == -1) {
            printf("%s: command not found\n", cmd);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        do {
            wpid =  waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int execute_command(char *cmd, char **args)
{
    int i;

    for (i = 0; i < num_of_builtins; i++) {
        if (strcmp(cmd, builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return launch_program(cmd, args);
}

void run_loop(void)
{
    while (1) {
        char *input = get_input();
        char *cmd = get_cmd(input);
        char **args = tokenize_args(input);
        execute_command(cmd, args);
        free(input);
        free(cmd);
        free(args);
    }
}

int main(int argc, const char * argv[])
{
    run_loop();
    return EXIT_SUCCESS;
}

