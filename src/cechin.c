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
	char *buffer;
	char content[] = " TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO";
	buffer = malloc(sizeof(unsigned char) * 256);
	
	write2(0, content, 1080);

	return 0;
}

// int write_first_tuple_MFT_and_set_0_second(unsigned int sector, struct t2fs_4tupla t)
// {
// 	unsigned char buffer[SECTOR_SIZE];
// 	int error = read_sector(sector, buffer);
// 	unsigned int zero = 0x00;
// 	if (error)
// 		return -1;

// 	unsigned int aux;
// 	// write AtributeType in the first tuple in the MFT
// 	int i;
// 	for (i = 0; i < 4; i++)
// 	{
// 		aux = (t.atributeType >> 8 * i) & 0xff;
// 		buffer[TUPLE_ATRTYPE + i] = aux;
// 	}
// 	// write virtualBlockNumber
// 	for (i = 0; i < 4; i++)
// 	{
// 		aux = (t.virtualBlockNumber >> 8 * i) & 0xff;
// 		buffer[TUPLE_VBN + i] = aux;
// 	}
// 	// write logicalBlockNumber
// 	for (i = 0; i < 4; i++)
// 	{
// 		aux = (t.logicalBlockNumber >> 8 * i) & 0xff;
// 		buffer[TUPLE_LBN + i] = aux;
// 	}
// 	// write numberOfContiguousBocks
// 	for (i = 0; i < 4; i++)
// 	{
// 		aux = (t.numberOfContiguosBlocks >> 8 * i) & 0xff;
// 		buffer[TUPLE_NUMCONTIGBLOCK + i] = aux;
// 	}
// 	// write 0 in the second tuple
// 	for (i = 0; i < 4; i++)
// 	{
// 		buffer[TUPLE_ATRTYPE + 16 + i] = zero;
// 	}

// 	int write_error = write_sector(sector, buffer);
// 	if (write_error)
// 		return -1;

// 	return 1;
// }