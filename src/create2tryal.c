#include <string.h>
#include <stdio.h>
#include "../include/apidisk.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/auxiliar.h"

/*
TYPEVAL_REGULAR     0x01 -> 1
TYPEVAL_DIRETORIO   0x02 -> 2
*/
#define SIZE_of_MFT_blocos 2048
#define SIZE_of_MFT_REGISTER_bytes 512
#define SIZE_of_MFT_REGISTER_sector 2

// #define SIZE_of_MFT_setores SIZE_of_MFT_blocos * 

#define ATRIB_TYPE 0
#define VBN 4
#define LBN 8
#define CONTIG_BLOCK 12

// find first empty MFT register, create first Tuple with defined Info
// return MFT register (6 must be the first rmpty register)
int find_empty_MFT_reg()
{
	unsigned char buffer[SECTOR_SIZE];
	unsigned char buffer2[SECTOR_SIZE];

	int full_MFT = 0;

	unsigned int SIZE_of_MFT_setores = ((unsigned int)boot_block.blockSize) * ((unsigned int)boot_block.MFTBlockSize);
	// unsigned int LAST_MFT_sector = 
	// first empty MFT register must be at block 4
	unsigned int first_MFT_sector = (unsigned int)boot_block.blockSize;
	unsigned int first_MFT_usable_sector = first_MFT_sector + SIZE_of_MFT_REGISTER_sector * 4; // reg0 = Bitmap; reg1 = root; reg2 and reg3 = reserved

	// this shall got to every register in the MFT
	for(int i = first_MFT_usable_sector; i <= SIZE_of_MFT_setores; i = i + SIZE_of_MFT_REGISTER_sector){
		int error = read_sector(i, buffer);
		if(error){
			printf("\nError at Find_empty_MFT_reg, i: %i, error: %d\n", i, error);
			return -1;
		}

		DWORD atrType = (buffer[ATRIB_TYPE+3] << 16) | 
						(buffer[ATRIB_TYPE+2] << 12) |
						(buffer[ATRIB_TYPE+1] << 8) |
						buffer[ATRIB_TYPE];

		if(atrType != -1){
			continue;
		}
		else{


			return //address
		}



	}


	return full_MFT;
}


int generic_create(BYTE type, char *filename)
{
	int empty_block;
	// struct t2fs_record arch;
	
	if(type == TYPEVAL_REGULAR){
		empty_block = searchBitmap2(0);
		if(empty_block > 0) // if found first empty position
		{
			struct t2fs_record arch;
			strcpy((char *)arch.TypeVal, (char *)type);
			strcpy(arch.name, filename);
			arch.blocksFileSize = 1; // init file with 1 block
			arch.bytesFileSize = SECTOR_SIZE * boot_block.blockSize;
			// find MFTNumber


		}

	}


}


FILE2 create2 (char *filename){
	FILE2 handle;


	return handle;
}