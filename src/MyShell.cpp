#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/wait.h>
#include<vector>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>

using namespace std;
pid_t pID = 1;
void ctrlcHandler(int sig);
string input;

//function to check if input has a url(path)

bool detectURL(string in) {
    int i = 0, u = 0, s = 0, b = 0;
    if (in[0] == 'c' && in[1] == 'd') {//for cd command (paths contain '/')
        return false;
    }
    while (i < in.length()) {
        if (in[i] == '/') {
            u++;
        }
        i++;
    }
    if (u > 0) {
        return true;
    } else {
        return false;
    }
}

//function to decode input

void decodeLine(string in, string &c, vector<string> &p, bool &b) {
    string w;
    int i = 0, start = 0;
    while (i < in.length()) {
        while (in[i] != ' ' && i < in.length()) {
            i++;
        }
        if (start == 0) {
            c.assign(in, 0, i);//assign the command in variable c
        } else {
            w.assign(in, start, i - start);
            if (w == "&") {
                b = true;//background is enabled
            } else {
                p.push_back(w);//put arguments in p
            }
        }

        i++;
        start = i;
    }

}


//built in methods
int myShell_System_cd(char **args);
int myShell_System_help(char **args);
int myShell_System_exit(char **args);

//list of built in commands
char *builtin_str[] = {
	const_cast<char*>("cd"),
	const_cast<char*>("help"),
	const_cast<char*>("cat"),
	const_cast<char*>("wc"),
	const_cast<char*>("ls"),
	const_cast<char*>("exit")
};


int (*builtin_func[]) (char **) = {
    &myShell_System_cd,
    &myShell_System_help,
&myShell_System_exit
};

int myShell_System_num_builtins() {
    return sizeof (builtin_str) / sizeof (char *);
}

//function for command cd

int myShell_System_cd(char **args) {
    char directory[1024];
    if (args[1] == NULL) {
        fprintf(stderr, "myShell_System: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) == 0) {//change directory
           getcwd(directory, sizeof (directory));//find the current directory
            printf("In %s\n", directory);
        } else {
            perror("myShell_System");
        }
    }
    return 1;
}

//function for command help

int myShell_System_help(char **args) {
    int i;
    printf("HELP\n");
    printf("This is the Shell project of 2644,2489,2506,2651\n");
    printf("You can use the standard terminal commands and the following System commands:\n");

    for (i = 0; i < myShell_System_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    return 1;
}

//function for command exit
int myShell_System_exit(char **args) {
  return 0;
}


//function that executes a command

int executeProgram(string url, string command, vector<string> param, bool background) {
	pid_t wpid;
	int status;

	//constact the command in char**
	char * arguments[(param.size() + 2)];
	if (url != "") {
        arguments[0] = const_cast<char*> (url.c_str());//if there is a url assign it to the first element with the command
        } else {
            arguments[0] = const_cast<char*> (command.c_str());//in the first element is the command
        }
       
        //assign arguments
        for (int i = 1; i < param.size() + 1; i++) {
            arguments[i] = const_cast<char*> (param[i - 1].c_str());
        }

        arguments[param.size() + 1] = NULL;//last element is 

	//finds the command
	if (command == "help") {	
		myShell_System_help(arguments);
	} else if (command == "cd") {
          if(arguments[1]!=NULL){
		if(strcmp(arguments[1], const_cast<char*>("./"))==0){//if the argument is ./ go to home directory
			//get home directory
                        struct passwd *pw = getpwuid(getuid());
			char *homedir = pw->pw_dir;
			arguments[1]=const_cast<char*>("/");
			myShell_System_cd(arguments);
			arguments[1]=homedir;
			myShell_System_cd(arguments);
		}else{
		myShell_System_cd(arguments);
		}
         }
         else
            myShell_System_cd(arguments);

        } else if(command=="exit"){
               return myShell_System_exit(arguments);
        }else {    
		pID = fork();
		if (pID == 0) {// child
      			if (background) {//in the background
				setpgid(0, 0);
        		} 
		
                        //from exec()
			if(execvp(arguments[0], arguments)==-1){
                                //search for file in current directory
				char directory[100];
				getcwd(directory, sizeof (directory));//find directory
                                strcat(directory,"/");
				char name[100];
                                //add directory to command
				strcpy (name,arguments[0]);
				strcpy(arguments[0],directory);
				strcat(arguments[0],name);
                                //execute again
				if(execvp(arguments[0], arguments)==-1){
					cout<<"File or command not found\n";
				}
			}

    		} else if (pID < 0) {// failed to fork
        		cerr << "Failed to fork" << endl;
        		exit(1); // Throw exception
    		} else {// parent
        		if (!background) {
        			do {
        				wpid = waitpid(pID, &status, WUNTRACED);
        			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
        	}
    	}
    }
    // Code executed by both parent and child
    //continues with the rest of the program
    return 1;
}



//Fuction that runs until the program is terminated

void start() {
	string command, url;
	vector<string> param; //variable for arguments
        int run=1;//this is 1 until user types exit
	do {

        	pID = 1;
        	bool background = false; //variable to check if a process runs in the background
        	//clear variables
        	input.clear();
        	command.clear();
        	url.clear();
        	param.clear();
        	cout << "MyShell$:";
        	getline(cin, input); //get input from user

        	//if the input is exit terminate the program
			//if input has a url
            		if (detectURL(input)) {
                		decodeLine(input, url, param, background);
            		} else {
                		decodeLine(input, command, param, background);
            		}
            		run=executeProgram(url, command, param, background); //execute command

    	} while (run);
}


//Function to handle ctrl-c

void ctrlcHandler(int sig) {

    //enable signal again
    signal(SIGINT, ctrlcHandler);
    //if a process is running stop it else print Myshell$:
    if (pID != 1)
        kill(pID, SIGTERM);
    else
        fprintf(stderr, "\nMyShelll$:");
}

int main() {
    	//enable signal for ctrl-c
    	signal(SIGINT, ctrlcHandler);
    	start();
    	return 0;
}
