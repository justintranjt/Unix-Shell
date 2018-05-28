# Unix-Shell

## File Descriptions
ish.c: Executed and displays Shell while calling each step of the process. Contains functions handling all built in commands and passes external commands to the OS to execute.

lexAnalyze.c/h: Lexically analyzes a line of input text per Step 1 and produces tokens categorized as ordinary or special.

synAnalyze.c/h: Syntactically parses the tokens from the lexical analyzer and separates the tokens into being the program name, arguments, and redirection files based on the order of input.

command.c/h: Abstract data type representing a program name, its optional arguments, and its optional redirection files.

dynarray.c/h: Abstract object representing a dynamically sized array capable of holding tokens as input.

program.c/h: Stateless module containing the current program name.

token.c/h: Abstract data type representing the input tokens typed by the user.

A full write-up can be found [here](https://justintranjt.github.io/projects/2018-05-27-unix-shell/).
