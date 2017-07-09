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

#define ATRIB_TYPE 0
#define VBN 4
#define LBN 8
#define CONTIG_BLOCK 12

#define TUPLE_ATRTYPE 0 
#define TUPLE_VBN 4
#define TUPLE_LBN 8
#define TUPLE_NUMCONTIGBLOCK 12

int main(int argc, char *argv[])
{
	char buffer[] = " TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTANDO TESTAN NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW_BLOCK NEW SPOILERS SPOILERS";
	write2(0, buffer, 2066);
	return 0;
}

int write_first_tuple_MFT_and_set_0_second(unsigned int sector, int offset, struct t2fs_4tupla t)
{
	unsigned char buffer[SECTOR_SIZE];
	int error = read_sector(sector, buffer);
	unsigned int zero = 0x00;
	if (error)
		return -1;

	unsigned int aux;
	// write AtributeType in the first tuple in the MFT
	int i;
	for (i = 0; i < 4; i++)
	{
		aux = (t.atributeType >> 8 * i) & 0xff;
		buffer[TUPLE_ATRTYPE + i + offset] = aux;
	}
	// write virtualBlockNumber
	for (i = 0; i < 4; i++)
	{
		aux = (t.virtualBlockNumber >> 8 * i) & 0xff;
		buffer[TUPLE_VBN + i + offset] = aux;
	}
	// write logicalBlockNumber
	for (i = 0; i < 4; i++)
	{
		aux = (t.logicalBlockNumber >> 8 * i) & 0xff;
		buffer[TUPLE_LBN + i + offset] = aux;
	}
	// write numberOfContiguousBocks
	for (i = 0; i < 4; i++)
	{
		aux = (t.numberOfContiguosBlocks >> 8 * i) & 0xff;
		buffer[TUPLE_NUMCONTIGBLOCK + i + offset] = aux;
	}
	// write 0 in the second tuple
	for (i = 0; i < 4; i++)
	{
		buffer[TUPLE_ATRTYPE + 16 + i + offset] = zero;
	}

	int write_error = write_sector(sector, buffer);
	if (write_error)
		return -1;

	return 1;
}