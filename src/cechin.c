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
void help() {

	printf ("Testing program - read and write setores do arquivo t2fs_disk.dat\n");
	printf ("   DISPLAY - d <setor>\n");
	printf ("\n");
	printf ("   HELP    - ?\n");
	printf ("   FIM     - f\n");
}

int main(int argc, char *argv[])
{	
	char* buffer;
	read2(0, buffer, 16);
	// read_bytes_from_sector(12, 16);
    return 0;
}
