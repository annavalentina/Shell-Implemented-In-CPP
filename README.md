# Shell-Implemented-In-CPP
A shell that executes UNIX programs and basic commands (eg. cd, exit, cat)


#	Manual
I. The shell does not recognize symbols like ">". So the command cat> filename for example, should be written as cat - filename.

II. After the successful execution of the cd path command it prints the address to which it was moved to, but does not appear next to Myshell $ :. The user stays at this address until he chooses another one.

III. It is not possible to return to Home folder with the cd command without an argument. It is necessary to type cd ./. The program goes first to / and then to the Home folder.

IV. If the user gives an executable name without a path then the current directory is searched. If an executable file is found, it is executed, otherwise an appropriate message is displayed.
