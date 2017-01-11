#include "stdio.h"
#include "../common/parser.h"

jsonData_t* parse (FILE *flog, char* configFile);


main() {
	jsonData_t* jsonData;
	jsonData = (jsonData_t*) parse(stdout, NULL);
	//cliLoop();
}

	
