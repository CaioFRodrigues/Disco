#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/auxiliar.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/MFT_list.h"
#include "../include/MFT.h"
#include "../include/directories.h"


int main(int argc, char *argv[])
{
	init();

	// unsigned int arqs;
	// create2("/file3");
	// create2("/file4");
	// create2("/file5");
	// mkdir2("/arateus");
	// mkdir2("/arateus/eh");
	// mkdir2("/arateus/eh/deuso");
	// mkdir2("/arateus/eh/deuso/metaleiro");
	mkdir2("/MyUltimateIsReady");
	create2("/MyUltimateIsReady/takeThis");



	return 0;
}

