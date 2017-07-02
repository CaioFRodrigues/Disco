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

	int full_MFT = -1;

	unsigned int SIZE_of_MFT_setores = ((unsigned int)boot_block.blockSize) * ((unsigned int)boot_block.MFTBlockSize);

	// first empty MFT register must be at block 4
	unsigned int first_MFT_sector = (unsigned int)boot_block.blockSize;
	unsigned int first_MFT_usable_sector = first_MFT_sector + SIZE_of_MFT_REGISTER_sector * 4; // reg0 = Bitmap; reg1 = root; reg2 and reg3 = reserved

	// this shall get every register in the MFT
	for(int i = first_MFT_usable_sector; i <= SIZE_of_MFT_setores; i = i + SIZE_of_MFT_REGISTER_sector){
		int error = read_sector(i, buffer);
		if(error){
			printf("\nError at find_empty_MFT_reg, i: %i, error: %d\n", i, error);
			return -1;
		}

		

		if(atrType != -1){
			continue;
		}
		else{
			return i; //address

		}

	}


	return full_MFT;
}

// unsigned int swap_4BYTE_endianess(unsigned char *str)
// {
//     if(strlen(str) == 4){
//         unsigned int num = (unsigned int)strtol((char *)str, NULL, 16);

//         unsigned int swapped = ((num>>24)&0xff) | // move byte 3 to byte 0
//                                ((num<<8)&0xff0000) | // move byte 1 to byte 2
//                                ((num>>8)&0xff00) | // move byte 2 to byte 1
//                                ((num<<24)&0xff000000); // byte 0 to byte 3
//         // swapped = swapped>>16;

// //        printf("%ld\n", sizeof(swapped));


//         return swapped;
//     }
//     else
//         return 0;

// }

int generic_create(BYTE type, char *filename)
{
	int empty_block;
	int empty_MFT_reg;
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
			empty_MFT_reg = find_empty_MFT_reg()


		}

	}


}


FILE2 create2 (char *filename){
	FILE2 handle;

	char nameAux[MAX_FILE_NAME_SIZE];


	return handle;
}