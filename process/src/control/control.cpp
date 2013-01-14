#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>

#define P2R_BIN "/home/gromholl/workspace/c++/process/debug/p2r"
#define I2P_BIN "/home/gromholl/workspace/c++/process/debug/i2p"

#define HELP_FLAG "-h"
#define DELAY 10000

bool repeatFlag = false;
bool stopFlag = false;

const char * helpInfo = "=== control application's help: ===\n\n"
                        "Application load list of expression and solve it.\n\n"
                        "SYNOPSIS:\n"
                        "  i2p [-h]\n"
                        "  i2p %exrs_file% %variable_file%\n\n"
                        "DESCRIPTION:\n"
                        "  -h\n"
                        "     help (show this message)\n"
        				"  %exrs_file%\n"
						"     list of expressions. For expression's syntax look i2p help.\n"
						"  %variable_file%\n"
						"     list of variable. See as \"var_name value\"\n\n"
                        "Created by GromHoll. 2012.\n";

using namespace std;


void term_handler(int signum) {
	stopFlag = true;
}
void usr1_handler(int signum) {
	repeatFlag = true;
}

string clearExp(const string & exp) {
	string res;

	for(int i = 0; i < exp.length(); i++) {
		if(exp[i] != ' ' && exp[i] != '\t')
			res += exp[i];
	}

	return res;
}

vector<string> loadExpressions(const string & fileName) throw(string) {
	vector<string> expressions;

	ifstream inFile(fileName.c_str(), fstream::in);
	if(inFile) {
		string exp;

		getline(inFile, exp);
		while(inFile) {
			expressions.push_back(clearExp(exp));
			getline(inFile, exp);
		}

		inFile.close();
	} else
		throw string("Can not load expression file.");

	return expressions;
}
map<string, string> loadVariables(const string & fileName) throw(string) {
	map<string, string> variables;

	ifstream inFile(fileName.c_str(), fstream::in);
	if(inFile) {
		string line;
		char name[256];
		char value[256];

		getline(inFile, line);

		while(inFile) {
			sscanf(line.c_str(), "%s %s", name, value);
			variables[string(name)] = string(value);
			getline(inFile, line);
		}

		inFile.close();
	} else
		throw string("Can not load variable file.");

	return variables;
}

void setupEnv(map<string, string> & variables) {
    map<string, string>::const_iterator it;
    for (it=variables.begin(); it != variables.end(); ++it) {
        setenv(it->first.c_str(), it->second.c_str(), 1);
    }
}

void control(const string & exps, const string & vars) {
	cout << "PID of process: " << getpid() << endl;

	/* Subscribe to signals */
	struct sigaction sa_term, sa_usr1;
	sa_term.sa_handler = term_handler;
	sa_usr1.sa_handler = usr1_handler;
	sigaction(SIGTERM, &sa_term, 0);
	sigaction(SIGUSR1, &sa_usr1, 0);

	/* Load info from files*/
	vector<string> expressions;
	map<string, string> variables;
	try {
		expressions = loadExpressions(exps);
		variables = loadVariables(vars);
	} catch(string & exc) {
		cerr << exc << endl;
		return;
	}

	setupEnv(variables);

    /* Prepare pipes */
	int file_rd;
	int file_wr;
	int i2p_wr;
	int p2r_rd;
	int p2r_wr;
	int control_rd;
	int filedes[2];

	file_rd = fileno(stdin);
	file_wr = fileno(stdout);

	pipe(filedes);
	i2p_wr = filedes[1];
	p2r_rd = filedes[0];

	pipe(filedes);
	p2r_wr = filedes[1];
	control_rd = filedes[0];

	/* Creating p2r process */
	int p2r_pid = fork();
	if(p2r_pid < 0) {
		cout << "Starting p2r daemon was failed." << endl;
	} else if(p2r_pid > 0) {
		/* Parent */
		dup2(control_rd, file_rd);

		int pos = 0;

		while(true) {

			if(pos < expressions.size()) {
				/* Creating i2p process */
				int i2p_pid = fork();
				if(i2p_pid < 0) {
					cout << "Starting i2p application was failed." << endl;
					stopFlag = true;
				} else if(i2p_pid > 0) {
					/* Parent */
					int status;
					waitpid(i2p_pid, &status, 0);

					string result;
					getline(cin, result);
					cout << expressions[pos] << " = " << result << endl;

				} else {
					/* Child */
					dup2(i2p_wr, file_wr);
					execl(I2P_BIN, I2P_BIN, expressions[pos].c_str(), NULL);

					cerr << "ERROR ON CREATING I2P." << endl;
				}
				pos++;
			} else {
				usleep(DELAY);
			}

			if(stopFlag) {
				cout << "Closing. Please wait...\t";
				kill(p2r_pid, SIGTERM);

				int status;
				waitpid(p2r_pid, &status, 0);

				printf("OK!");
				exit(0);
			}

			if(repeatFlag) {
				cout << "Calculating starting from begin." << endl;
				repeatFlag = false;
				pos = 0;
			}
		}
	} else {
		/* Child */
		dup2(p2r_rd, file_rd);
		dup2(p2r_wr, file_wr);
		execl(P2R_BIN, P2R_BIN, NULL);

		cerr << "ERROR ON CREATING P2R." << endl;
	}
}

int main(int argc, char *argv[]) {

	if(argc == 2) {
        string arg(argv[1]);
        if(arg.compare(HELP_FLAG) == 0) {
        	cout << helpInfo << endl;
        }
    } else if(argc == 3) {
        string expsFile(argv[1]);
        string varsFile(argv[2]);
        control(expsFile, varsFile);
    } else {
        cout << "Unknown arguments. Write \"control -h\" for help." << endl;
    }

    return 0;
}
