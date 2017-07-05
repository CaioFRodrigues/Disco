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

  MFT_list* mft_list_copy = mft_list;  
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
}

// Ana
//  Given a byte offset and an mft list, finds the byte offset relative to the 
//logical block number in which the wanted byte will be
//  Example: bytes = 500
//  The 500th byte will be in the file's second virtualBlock, and it will
//be the 12th byte from the beginning of the equivalent logicacl block
int find_byte_position_in_logical_block(MFT* mft, int bytes){
  
  int num_sectors, num_blocks, current_logical_block_number, offsets_bytes;

  num_sectors = ceil(bytes/256);
  num_blocks = ceil(num_sectors/boot_block.MFTBlocksSize);

  offsets_bytes = (num_blocks * num_sectors * 256) - bytes;
  
  return offsets_bytes;

}