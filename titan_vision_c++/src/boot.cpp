#include <iostream>
#include <string>
#include "bikePi.h"
#include "config.h"


using namespace std;

int main(int argc, char **argv)
{
	cout << argc << endl;
	cout << argv[0] << endl;
	if(argc == 1)
		if(IS_BACKUP)
			sparePi();
		else
			bikePi();
	else if(argc > 1)
	{
		cout << argv[1] <<endl;
		string arg1 = argv[1];
		if(arg1 == "bikePi")
			bikePi();
		else if(arg1 == "sparePi")
			sparePi();
	}
	cin.get();
	return 0;
}
