#include "../src/shared/calc_logig.h"

#include <iostream>
#include <stdlib.h>
#include <queue>
#include <string>

using namespace std;


int main() {
	setenv("a", "1", 1);
	setenv("b", "2", 1);

	string exp = "2+2";
	try {
		cout << exp << " = " << p2r("2 2 +") << endl;
	} catch(string & exc) {
		cout << exc << endl;
	}

	return 0;
}
