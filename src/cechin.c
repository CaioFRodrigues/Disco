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
	char* buffer;
	buffer = malloc(sizeof(unsigned char)*(4066));

	read2(0, buffer, 4066);
	printf("%s\n", buffer);
    return 0;
}
