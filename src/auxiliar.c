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

  initialize_open_directories(); 

  return 0;
}

//Caio
//Initializes the array of opened files
void initialize_open_files(){

  for(int i = 0; i < MAX_OPENED_FILES; i++)
    opened_files[i].is_valid = 0;

}

void initialize_open_directories(){
  number_dir_handles = 0;
  opened_directories = NULL;
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

//Caio
//Gets first possible position from opened_directories
//Returns -1 if it failed
int first_free_dir_position(){

  //If there is no current directory, return 0 and allocates space
  if (opened_directories == NULL){
    opened_directories = malloc(sizeof(FILE_DESCRIPTOR));
    return 0;
  }
  else{
    //Goes through the current handles, and tries to find a valid one
    for (int i = 0;i < number_dir_handles; i++){
      if (opened_directories[i].is_valid)
        return i;
    }

    //If there is none, it is needed to reallocate the size of opened_directories and return that new_position
    opened_directories = realloc(opened_directories, sizeof(opened_directories) + sizeof(FILE_DESCRIPTOR));
    number_dir_handles++;
    return number_dir_handles;
  }
  return -1;
}

//Ana
DWORD virtual_block_to_logical_block(DWORD current_pointer, MFT* mft_list){

  MFT* mft_list_copy = mft_list;  
  DWORD currentVirtualBlockNumber, numberOfContiguosBlocks;

  while (mft_list_copy != NULL){
      
    currentVirtualBlockNumber = mft_list_copy->current_MFT.virtualBlockNumber;
    numberOfContiguosBlocks = mft_list_copy->current_MFT.numberOfContiguosBlocks;
    
    if (numberOfContiguosBlocks + currentVirtualBlockNumber - 1 < current_pointer){ // If current_pointer is not mapped in this tuple
      mft_list_copy = mft_list_copy->next;
    }
    else{
      // currentVirtualBlockNumber maps to logicalBlockNumber, current_pointer = logicalBlockNumber + offset
      return mft_list_copy->current_MFT.logicalBlockNumber + (current_pointer - currentVirtualBlockNumber);
    }
  }
  return -1;
}

