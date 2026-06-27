# Custom C Shell

This is my first project built for Linux! I used WSL on Windows 11 so I could work completely inside a Linux environment. It is a manually built, Unix-like command-line interpreter written in C (using the C11 standard). The shell talks directly to the Linux kernel using system calls to handle running processes, pipelines, input/output redirection, and background jobs.

---

## Why a Custom Makefile?

When I first started designing this, I looked into more modern build tools like CMake and Ninja. I ended up choosing to write a custom `Makefile` instead because I wanted to keep things at the lowest level of abstraction possible and see exactly how everything compiles. 

It is a simple and clean way to handle compilation for my specific project setup, where every header file in the `include/` directory has a matching source file in `src/`. The Makefile automatically builds object files in a structured `build/` folder and links the final executable, keeping the workspace tidy.

---

## How It Works

The shell handles your commands by passing them through three main phases:

```
[User Input String] 
        │
        ▼
┌───────────────┐
│     Lexer     │  (src/Lexer.c) -> Splits the text into tokens
└───────────────┘
        │
        ▼
┌───────────────┐
│    Parser     │  (src/Parser.c) -> Builds the Abstract Syntax Tree (AST)
└───────────────┘
        │
        ▼
┌───────────────┐
│   Executor    │  (src/Executor.c) -> Runs processes, pipes, and redirects
└───────────────┘
```

1. **Lexical Analysis (Lexer):** Takes the raw string you typed and breaks it down into a dynamic list of tokens.
2. **Syntactic Analysis (Parser):** Organizes those tokens into an Abstract Syntax Tree (AST) based on operator precedence.
3. **Execution Engine (Executor):** Walks through the AST recursively to fork processes, manipulate file descriptors, and wire up pipes.

---

## Features

### 1. Tokenizer and Operator Detection
* Recognizes standard operators like:
  * Pipe (`|`)
  * Input redirection (`<`)
  * Output redirection (`>`)
  * Append output redirection (`>>`)
  * Background execution (`&`)
* Words and standard symbols are grouped into `WORD` tokens.
* Supports double quotes (`"..."`) and single quotes (`'...'`) so you can include spaces and operators inside a single argument.
* Catches unclosed quotes early and alerts you with a syntax error before the parser even runs.

### 2. Grammar Parsing and AST Generation
The parser creates specific tree nodes depending on what you typed:
* `NODE_COMMAND`: The actual executable and its arguments.
* `NODE_PIPE`: A bridging node with a left side (writer) and a right side (reader).
* `NODE_REDIRECT_IN` / `NODE_REDIRECT_OUT` / `NODE_REDIRECT_APP`: Redirection nodes that hold the target file path and the command attached to it.
* `NODE_BACKGROUND`: A top-level node that tells the shell to run the command in the background.

*Rules and Assumptions:*
* The background operator `&` sits at the very root of the execution tree but has the lowest priority when grouping commands. The shell only expects one `&` right at the end of your input.
* The parser checks for syntax issues while building the tree, so it will catch things like empty pipes or missing filenames after a redirect.

### 3. Execution Mechanics
* **Spawning Processes:** Uses `fork` and `execvp` to run standard external programs.
* **Redirection:** Uses `open` with the right flags (`O_RDONLY`, `O_WRONLY`, `O_CREAT`, etc.) and swaps out file descriptors using `dup2`.
* **Pipes:** Connects commands together using the `pipe` system call. The executor forks separate child processes for both sides of the pipe so the terminal doesn't freeze up.
* **Background Tasks:** Runs jobs without blocking the shell. It cleans up finished background processes at the start of every input loop using `waitpid(-1, &status, WNOHANG)`.

### 4. Custom Utility Libraries
* **Vector (`Vector.h`):** My own dynamic array implementation to handle generic pointers, tracking sizes, and resizing automatically.
* **Memory Guard (`MemoryGuard.h`):** A simple function to be used along with `malloc`, `calloc`, and `realloc` that makes sure allocations are safe and safely stops the program if it ever runs out of memory.

---

## Project Structure

```
.
├── Makefile             # Build configuration
├── README.md            # This file!
├── include/             # Header files
│   ├── ASTNode.h        # Tree node structures
│   ├── BuiltIns.h       # Built-in shell commands
│   ├── Executor.h       # Execution logic definitions
│   ├── Lexer.h          # Lexer state and structures
│   ├── MemoryGuard.h    # Memory helper declarations
│   ├── Parser.h         # Parser function definitions
│   ├── Token.h          # Token types and vector helpers
│   └── Vector.h         # Dynamic array definitions
└── src/                 # Source code
    ├── ASTNode.c        # Tree creation and memory cleanup
    ├── BuiltIns.c       # Core built-ins (cd, pwd, cat)
    ├── Executor.c       # Forking, redirection, and piping logic
    ├── Lexer.c          # Tokenizer state machine
    ├── MemoryGuard.c    # Allocation checking logic
    ├── Parser.c         # Recursive descent logic
    ├── Token.c          # Token management utilities
    ├── Vector.c         # Dynamic vector logic
    └── main.c           # Main REPL loop entry point
```

---

## Built-in Commands

I wrote manual C implementations for a few specific commands so they run directly inside the shell process without needing to fork a new child process.

| Command | Function Name | Registry Target | Description |
|---|---|---|---|
| `cd` | `shell_cd` | `"cd"` | Uses the `chdir` system call to update the shell's working directory and checks your arguments. |
| `pwd` | `shell_pwd` | `"pwd"` | Grabs and prints the current absolute path using `getcwd` and manages the path buffer dynamically. |
| `catt` | `shell_cat` | `"catt"` | A simplified version of the Unix `cat` utility built to test standard file reading inside my custom shell environment. It does not handle flags yet. |

*Why is cat named catt?*
I mapped my custom implementation of `cat` to the command `"catt"` on purpose. This makes it really obvious that you are running my internal C function rather than just calling the default `cat` program installed on your Linux machine. Also it helped me compare the results of function and see if my implementation results matched the actual result.

---

## How to Build and Run

### Prerequisites
* GCC compiler
* GNU Make
* A Linux environment (like WSL or a Linux distro)

### Commands
Run these from the project root directory:

```bash
# Compile the shell and create the executable
make

# Run the compiled shell
make run

# Clean up build files and object files
make clean
```

The compiled output will live at `build/bin/myShell.out`.

---

## Future Improvements & TODOs

Here is a checklist of features, refactors, and roadmap ideas I want to implement to make this project even more complete:

### Core Code Improvements
- [ ] **AST Memory Optimization:** Right now, the `ASTNode` struct holds variables for all node types at the same time (like arguments, file paths, and left/right branch pointers). Clean this up by putting type-specific fields inside a C `union` to save memory.
- [ ] **Logical Operators:** Extend the lexer and parser to support logical chaining like `&&` and `||`, so commands can run conditionally based on whether the previous one succeeded.
- [ ] **Built-in Flag Options:** Rewrite the built-in command handlers (like `cd` and `catt`) to parse actual flags (like `-n` or `-v`) using a standard parsing format instead of just checking hardcoded argument positions.

### Advanced Shell Features
- [ ] **Signal Handling:** Implement proper handlers for signals like `SIGINT` (Ctrl+C) and `SIGTSTP` (Ctrl+Z). The shell should catch these interrupts and pass them down to active child processes rather than terminating the main shell process itself.
- [ ] **Advanced Job Control:** Support full job management, allowing the user to track and manipulate background processes using standard command utilities like `jobs`, `fg` (foreground), and `bg` (background).
- [ ] **Command History and Line Editing:** Add line-editing utilities and command history. This can be done either by integrating the `libreadline` library or by writing a custom circular terminal buffer that intercepts key inputs (like arrow keys) to load previous commands.
- [ ] **Environment Variables and Expansion:** Develop a storage registry for environment variables (like `PATH` or custom keys) and expand variables prefixed with `$` (e.g., `$USER`, `$PATH`) during the tokenization stage.
- [ ] **Path Resolution Cache:** Optimize command execution latency by implementing a hash table to cache resolved absolute paths of binaries found in the `$PATH` environment variable.
- [ ] **Automated Testing Suite:** Create an integration test script (in Bash or Python) that runs the shell through various pipeline and redirection scenarios, comparing the stdout and stderr output against standard shells (like Bash) to guarantee functional correctness.

---
*Developer Note: Insert screenshots of the Abstract Syntax Tree generation output and pipeline executions here.*