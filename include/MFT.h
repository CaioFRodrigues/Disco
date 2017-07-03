#ifndef __MFTH__
#define __MFTH__

#include "../include/auxiliar.h"


#define ATTRIBUTE_TYPE_START 0
#define VIRTUAL_BLOCK_NUMBER_START 4
#define LOGICAL_BLOCK_NUMBER_START 8
#define NUMBER_OF_CONTIGUOS_BLOCKS_START 12
#define MFT_4TUPLA_SIZE 16

#define SECTOR_PER_MFT 2

MFT* read_MFT(int sector); 

int read_sector_as_MFT(int sector, MFT** list);

struct t2fs_4tupla fill_MFT(unsigned char* buffer, int MFT_4tupla_number);

int get_next_MFT(char *dirname, MFT* directory, int mode);

#endif