#include <iostream>
#include <string>
#include <stdlib.h>

#include "../shared/calc_logic.h"

#define HELP_FLAG "-h"
#define CLOSE_COMAND "close"

#define EXP_ERR "Error: "
#define STREAM_ERR "Stream error. Stream will be cleared."

/* Return codes */
enum {
    FINISH_OPT,         /* 0 - Finish */
    HELP_OPT,           /* 1 - Help showed */
    UNKNOWN_ARG_OPT     /* 2 - Unknown arguments */
};

bool loopFlag;
const char * helpInfo = "=== p2r application's help: ===\n\n"
                        "Daemon application convert postfix form of expression to result. For finishing work use signals.\n\n"
                        "SYNOPSIS:\n"
                        "  i2p [-h]\n\n"
                        "DESCRIPTION:\n"
                        "  -h\n"
                        "     help (show this message)\n\n"
                        "More info on:\n"
                        "EN: http://en.wikipedia.org/wiki/Reverse_Polish_notation \n\n"
                        "Created by GromHoll. 2012.\n";

using namespace std;

void loop() {

    string exp;
    loopFlag = true;

    while(loopFlag) {
    	getline(cin, exp);

        if(cin) {
            if(exp.compare(CLOSE_COMAND) == 0) {
                loopFlag = false;
            }
            else
                try {
                    cout << p2r(exp) << endl;
                } catch (string & e) {
                    cerr << EXP_ERR << e << endl;
                }
        } else {
            loopFlag = false;
        }
    }
}

int main(int argc, char *argv[]) {

    if(argc == 1) {
        loop();
        exit(FINISH_OPT);
    } else if(argc == 2) {
        string arg(argv[1]);
        if(arg.compare(HELP_FLAG) == 0) {
            cout << helpInfo << endl;
            exit(HELP_OPT);
        }
    }

    cerr << "Unknown arguments." << endl;
    exit(UNKNOWN_ARG_OPT);
}
