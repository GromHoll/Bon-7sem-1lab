#include <iostream>
#include <string>
#include <stdlib.h>

#include "../shared/calc_logig.h"

#define HELP_FLAG "-h"

/* Return codes */
enum {
    SUCCESS_OPT,        /* 0 - Success */
    HELP_OPT,           /* 1 - Help showed */
    INV_NUM_ARG_OPT,    /* 2 - Invalid number of arguments */
    INV_EXP_OPT,        /* 3 - Invalid expression */
};

const char * helpInfo = "=== i2p application's help: ===\n\n"
                        "Application convert infix form of expression to postfix.\n\n"
                        "SYNOPSIS:\n"
                        "  i2p [-h]|[exp]\n\n"
                        "DESCRIPTION:\n"
                        "  -h\n"
                        "     help (show this message)\n\n"
                        "  exp\n"
                        "     math expression. You can use ^*/+- as operations, () as brackets,\n"
                        "     0-9 as numbers and a-z as values.\n\n"
                        "     Example:\n"
                        "       $ i2p 2+2\n"
                        "       2 2 +\n\n"
                        "More info on:\n"
                        "EN: http://en.wikipedia.org/wiki/Reverse_Polish_notation \n\n"
                        "Created by GromHoll. 2012.\n";

using namespace std;

int main(int argc, char *argv[]) {
    if(argc > 1) {
        string exp(argv[1]);
        if(exp.compare(HELP_FLAG) == 0) {
            cout << helpInfo << endl;
            exit(HELP_OPT);
        } else {
            string res = i2p(exp);
            if(res.length() == 0) {
                cerr << "Invalid expression." << endl;
                exit(INV_EXP_OPT);
            } else {
                cout << res << endl;
                exit(SUCCESS_OPT);
            }
        }
    } else
        exit(INV_NUM_ARG_OPT);
}
