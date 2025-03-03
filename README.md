#  C Shell Project

A basic shell implementation in C, supporting several built-in commands and execution of external programs.

## Features

- **Built-in Commands**:
  - `exit`: Terminates the shell.
  - `echo [args]`: Prints arguments to stdout.
  - `type [command]`: Checks if a command is a built-in or locates it in the `PATH`.
  - `pwd`: Prints the current working directory.
  - `cd [dir]`: Changes the current directory (supports `~` for home directory).
- **External Program Execution**: Executes programs found in the `PATH` environment variable.
- **Input Handling**: Parses user input into commands and arguments.

## Installation

1. **Prerequisites**: 
   - Ensure `gcc` is installed.

2. **Compilation**:
   ```bash
   git clone https://github.com/zxasc/shell-c shell-c
   cd shell-c
   gcc src/main.c -o shell-c
   ```
   
3. **Run**
    ```bash
    ./shell.c
    ```

- Example commands

   ```bash
   $ echo Hello World
   $ type echo      # Output: "echo is a shell builtin"
   $ cd ~
   $ pwd            # Output: /home/user 
   ```
   
## License
MIT License - See [LICENSE](./LICENSE) for details.