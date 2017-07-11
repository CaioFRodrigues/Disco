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
    
    if (numberOfContiguosBlocks + currentVirtualBlockNumber - 1 < currentVirtualBlockNumber){ // If current_virtual_block is not mapped in this tuple
      mft_list_copy = mft_list_copy->next;
    }
    else{
      // currentVirtualBlockNumber maps to logicalBlockNumber, current_virtual_block = logicalBlockNumber + offset
      return mft_list_copy->current_MFT.logicalBlockNumber + (currentVirtualBlockNumber - currentVirtualBlockNumber);
    }
  }

  return -1;
}

// Ana
//  Given a byte offset and an mft list, finds the byte offset relative to the 
//logical sector number in which the wanted byte will be
//  Example: bytes = 500
//  The 500th byte will be in the file's second virtualBlock, and it will
//be the 12th byte from the beginning of the equivalent logical sector
int find_byte_position_in_logical_sector(MFT* mft, int bytes){
  
  int num_sectors, num_blocks, offsets_bytes;

  num_sectors = ceil(bytes/256);
  num_blocks = ceil(num_sectors/boot_block.MFTBlocksSize);

  offsets_bytes = bytes - (num_blocks * num_sectors * 256);
  
  return offsets_bytes;

}

// Ana
char* append_buffers( char* final_buffer, char* temp_buffer ){
  while (*final_buffer)
    final_buffer++;
  
  while (*final_buffer++ = *temp_buffer++);

  return --final_buffer;
}

// Ana
/* Given the starting and ending byte, the amount of bytes to copy, the source and the destination buffer,
copies the specific bytes in the specific amounts to the destination buffer
*/
void read_bytes(int starting_byte, int ending_byte, int bytes_to_copy, char* source, char* destination){

  unsigned char temp_buffer[bytes_to_copy+1]; // Temporary buffer that stores bytes to be appended to buffer

  int k = 0, j = 0;

  // Copy wanted bytes only
  for (j=starting_byte; j<ending_byte; j++){
    temp_buffer[k] = source[j];
    k++;
  }
  temp_buffer[k] = '\0';

  destination = append_buffers(destination, temp_buffer);


}


//Arateus
int write_record_in_dir(unsigned int sector, unsigned int byte_pos, struct t2fs_record record)
{
  // unsigned int byte_sector = take_sector_from_empty_record_info(byte_pos);
  // unsigned int byte_record_pos = take_record_position_in_dir(byte_pos);

  unsigned char buffer[SECTOR_SIZE];

  int error = read_sector(sector, buffer);
  if(error)
    return -1;

  // write TypeVal
  buffer[byte_pos] = record.TypeVal;
  // write name
  int i;
  for (i = 0; i < MAX_FILE_NAME_SIZE; i++)
  {
    buffer[byte_pos+RECORD_NAME+i] = record.name[i];
  }

  unsigned int aux;
  // write in buffer the blocksFileSize
  for (i = 0; i < 4; i++)
  {
    aux = (record.blocksFileSize >> 8*i)&0xff;
    buffer[byte_pos+RECORD_BLOCK_FILESIZE+i] = aux;
  }
  //write in buffer for the bytesFileSize
  for (i = 0; i < 4; i++)
  {
    aux = (record.bytesFileSize >> 8*i)&0xff;
    buffer[byte_pos+RECORD_BYTES_FILESIZE+i] = aux;
  }
  // write for the MFTNumber
  for (i = 0; i < 4; i++)
  {
    aux = (record.MFTNumber >> 8*i)&0xff;
    buffer[byte_pos+RECORD_MFTNUMBER+i] = aux;
  }

  // write buffer in sector
  int write_error = write_sector(sector, buffer);
  if(write_error)
    return -1;

  return 1;
}
