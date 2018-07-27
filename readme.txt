made by: Nicolas Perez and Catherine Daigle

April 2018
A productivity enhancing shell that allows user to traverse directories more quickly via its built in 'tree' command
by displaying the file system as a tree structure and then being able to reference files and directories using automatically 
assigned number aliases
By default it will avoid printing symbolic links, to save space on the screen and also avoid possible infinite loops during
tree traversal

other features include:
nice colours
history of commands (it will create and access a text file called 'History' inside of your documents folder)

'screenShot' is a screenshot of our program running after calling the tree command

'userStudy' is a graph showing average ratings users gave our program and the already-available tree command on Linux 
based on a scale out of 10, with 0 being a poor rating and 10 being outstanding

Google for "tree command" for explanation of tree.
 To start, type in 'make' while inside of this directory in a unix shell.

To launch the shell, type in ./customShell

Type in 'help' for a list of commands
The numbered aliases printed to the screen allows you to reference files and directories by their numbered aliases in all 
subsequent commands after running the tree command
For example, in the screenshot provided, typing in 'cd 24' would make you change to the directory 'Untitled Folder 4'
Overiding aliases requires a '\' before the alias, in the off chance that you own a file or folder by the same alias name
The depth of a tree search can also be specified when typing in the tree command, see 'help' command while running the shell
for instructions

Reuse of our code is not open to commercial use.
 Please contact one of us if wanting to use it commercially.

