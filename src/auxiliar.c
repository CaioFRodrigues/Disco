/*
*   UFRGS - Universidade Federal do Rio Grande do Sul
*   Instituto de Informática
*   INF01142 - Sistemas Operacionais I (2017/1)
*
*   Trabalho Prático II - Sistema de arquivos T2FS
*
*   Ana Paula Mello - 260723 <apcomello@gmail.com>
*   Arateus Meneses - 242260 <arateus.meneses@gmail.com>
*   Caio Rodrigues -261578  <caio_f.r@hotmail.com>
*
*   Versão: 16.2
*/
#include <string.h>
#include <stdio.h>
#include <math.h>


#include "../include/auxiliar.h"
#include "../include/MFT.h"
#include "../include/MFT_list.h"
#include "../include/bitmap2.h"
struct t2fs_bootBlock boot_block;

//Ana
//Initializes the boot block with the info from the file
int init(){
  int error;
  unsigned char buffer[SECTOR_SIZE];

  error = read_sector(0, buffer);
  if (error)
    return -1;

  int i =0;
  for (i=ID; i<ID+4; i++){
      boot_block.id[i]=buffer[i];
  }

  boot_block.version = (buffer[VERSION+1] << 8) | buffer[VERSION];

  boot_block.blockSize = (buffer[BLOCKSIZE+1] << 8) | buffer[BLOCKSIZE];

  boot_block.MFTBlocksSize = (buffer[MFTBLOCKSSIZE+1] << 8) | buffer[MFTBLOCKSSIZE];

  boot_block.diskSectorSize = (buffer[DISKSECTORSIZE+3] << 16) | (buffer[DISKSECTORSIZE+2] << 12) | (buffer[DISKSECTORSIZE+1] << 8) | buffer[DISKSECTORSIZE];

  initialize_open_files();

  return 0;
}

//Caio
//Initializes the array of opened files
void initialize_open_files(){

  for(int i = 0; i < MAX_OPENED_FILES; i++)
    opened_files[i].is_valid = 0;

}


//Caio
//Gets first possible position from opened_files
//Returns -1 if 20 files have been opened
int first_free_file_position(){

  for (int i = 0; i < MAX_OPENED_FILES; i++){
    if (!opened_files[i].is_valid)
      return i;
  }

  return -1;

}

//Ana
int virtual_block_to_logical_block(DWORD current_virtual_block, MFT* mft_list){

  MFT* mft_list_copy = mft_list;  
  DWORD currentVirtualBlockNumber, numberOfContiguosBlocks;

  while (mft_list_copy != NULL){
      
    currentVirtualBlockNumber = mft_list_copy->current_MFT.virtualBlockNumber;
    numberOfContiguosBlocks = mft_list_copy->current_MFT.numberOfContiguosBlocks;
    
    if (numberOfContiguosBlocks + currentVirtualBlockNumber - 1 < current_virtual_block){ // If current_virtual_block is not mapped in this tuple
      mft_list_copy = mft_list_copy->next;
    }
    else{
      // currentVirtualBlockNumber maps to logicalBlockNumber, current_virtual_block = logicalBlockNumber + offset
      return mft_list_copy->current_MFT.logicalBlockNumber + (current_virtual_block - currentVirtualBlockNumber);
    }
  }
  return -1;

}

// Ana
//  Given a byte offset and an mft list, finds the byte offset relative to the 
//logical block number in which the wanted byte will be
//  Example: bytes = 500
//  The 500th byte will be in the file's second virtualBlock, and it will
//be the 12th byte from the beginning of the equivalent logicacl block
int find_byte_position_in_logical_block(MFT* mft, int bytes){
  
  int num_sectors, num_blocks, offsets_bytes;

  num_sectors = ceil(bytes/256);
  num_blocks = ceil(num_sectors/boot_block.MFTBlocksSize);

  offsets_bytes = (num_blocks * num_sectors * 256) - bytes;
  
  return offsets_bytes;

}

// Ana
char* append_buffers( char* final_buffer, char* temp_buffer ){
  while (*final_buffer)
    final_buffer++;
  
  while (*final_buffer++ = *temp_buffer++);

  return --final_buffer;
}

// Arateus
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

// Ana
// Given a buffer with content to be inserted in a sector,
// inserts it in the given place and writes it to the sector
int insert_in_sector(int sector, char* content, int start, int size){

    unsigned char sector_content[SECTOR_SIZE];
// Buffer that will recieve the sliced buffer content

    int i = 0, k = 0;
    read_sector(sector, sector_content);

    k = start;
    for (i=0; i<size; i++){
        sector_content[k] = content[i];
        k++;
    }

    write_sector(sector, sector_content);

    return 0;
}


// Ana
/* Given the amount of blocks needed, a pointer to the mft list and a pointer to its last node
alocates the blocks in the disk, creating/updating the MFT tuples to map the virtual blocks
to the logical ones
*/
int alocate_needed_blocks(int blocks_needed, MFT* mft, MFT* last_mft){

  int n = 0;

  for (n=0; n<blocks_needed;n++){

    int new_block = searchBitmap2(0);
    if (setBitmap2(new_block, 1) != 0){
      return -1;
    }

    // // New block is contiguos to the last one
    if (new_block == last_mft->current_MFT.logicalBlockNumber + last_mft->current_MFT.numberOfContiguosBlocks){
      last_mft->current_MFT.numberOfContiguosBlocks++;
      write_first_tuple_MFT_and_set_0_second(last_mft->sector, (last_mft->offset)*16, last_mft->current_MFT);
    }
    else{ // New block needs new tuple
      struct t2fs_4tupla new_tuple;
      new_tuple.atributeType = 1;
      new_tuple.logicalBlockNumber = new_block;
      new_tuple.virtualBlockNumber = last_mft->current_MFT.virtualBlockNumber + last_mft->current_MFT.numberOfContiguosBlocks;
      new_tuple.numberOfContiguosBlocks = 1;

      // Treat situation in which sector and/or register is full
      write_first_tuple_MFT_and_set_0_second(last_mft->sector, (last_mft->offset+1)*16, new_tuple);
      mft = push_MFT(mft, new_tuple, last_mft->sector, (last_mft->offset)+1);
      last_mft = last_mft->next;

    }
  }

  return 0;

}