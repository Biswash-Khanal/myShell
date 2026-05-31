//first
Repository for a personal project, trying to make a shell with using the linux system calls.

learnt about make files, cmakes and ninja. decided to stick with the lowest level that is the makefile method

//second
part one is complete, the lexer works properly although with some limitations. for now ive decided to stick witha limited scope
the operators it detects are the 
    | -> pipe
    < -> redirect in
    > -> redirect out 
    >> -> redirect out append 
    & -> background

any other sequences of alphanumeric or symbols not involved in the operators list are taken as WORDS
eg. abcd, a2cb, aaaaaa, ^^^^^^, etc

in case an operator needs to be the part of the string, ive added rules to allow for double quotes to be used ""
in its current state the quotes cannot be seperated across lines i.e. no new line between quotes. 
spaces, symbols and operators within quotes are taken as the part of words. eg
"  abc d" -> WORD(  abc d)
"  ||abc<>d" -> WORD(  ||abc<>d)


next part is building the actual parser. for this i will be using the concept of the abstract syntax tree to turn the token vector 
into a structured tree according to the operator precedence and associativity which weill then allow to understand the structure of command
and execute different functions based on the structure of the command


//latest
part 2 is complete, the parser is working properly, i have made a grammar for the command structure and put conditional branches to return error if some invalid command is entered. Proper error handling is still TODO in the later version. anyways, any command converted to the token vector by the lexer, is now converted into a AST tree structure by the parser. for this. the & operator has the higheest priority and ive made the program with some assumptions
some assumptions are
    only 1 & is allowed and that too at the end of the entire command. 
    " " is avalilable, '' is not, and it is only usable for WORD type tokens.

The AST node struct is not optimized as in it contains all the properties for any type of node. can be optimized using unions to make it only take the space it uses
The invalid command console logs are very general, TODO: make it describe the problem accurately and in more detail


-first updated may 19th 2026
-second updated may 27th 2026
-latest updated may 31th 2026
