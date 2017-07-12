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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/bitmap2.h"
#include "../include/files.h"
#include "../include/MFT.h"
#include "../include/MFT_list.h"

//Arateus
int identify2(char *name, int size){
	char devs[] = {"Ana Paula Mello 260723\nArateus Meneses 242260\nCaio Rodrigues 261578\0"};
    name[size] = '\0';
    if(strncpy(name, devs, size))
        return 0;
    else
        return -1;
}

//Ana
int close2(FILE2 handle){

    if (opened_files[handle].is_valid == 1 && handle < 20){
        opened_files[handle].is_valid = 0;
        return 0;
    }
    return -1;
}

//Ana
int closedir2(DIR2 handle){

    if (opened_directories[handle].is_valid == 1 && handle < number_dir_handles){
        opened_directories[handle].is_valid = 0;
        return 0;
    }
    return -1;
}

//Ana
int seek2(FILE2 handle, DWORD offset){

  // Error cases
  if (!opened_files[handle].is_valid)
    return -1;

  if (offset > opened_files[handle].fileSizeBytes)
    return -1;

  if (offset != -1){  // Offset always counted from the start of the file (current_pointer = 0) 
    opened_files[handle].current_pointer = offset;
  }
  else{
    // current_pointer must go to the byte right after the end of the file (0 to fileSizeBytes -1)
    opened_files[handle].current_pointer = opened_files[handle].fileSizeBytes;
  }

  return 0;
}

// Ana
int read2(FILE2 handle, char * buffer, int size){

  int i =0;

  MFT* mft = read_MFT(opened_files[handle].first_MFT_tuple);

  unsigned char sector_buffer[SECTOR_SIZE]; // Buffer used to read from sector
  buffer[size] = '\0';

  int current_pointer = opened_files[handle].current_pointer;

  int bytes_left_to_copy = size;

  //   If the number of bytes to be read is larger than the amount of bytes 
  // from the current_pointer to the end of file, make sure that function reads only till end of file
  if (size + current_pointer > opened_files[handle].fileSizeBytes)
    size = opened_files[handle].fileSizeBytes - current_pointer;

  int current_pointer_block = current_pointer/1024; // Virtual block in which current_pointer is in
  int current_pointer_sector = current_pointer/256; //Logical sector in which first byte is in

  // Reads from file until there are no more mft tuples or there are no more bytes left to copy
  while (mft != NULL && bytes_left_to_copy > 0){

    // MFT info for blocks and sectors to be read
    int mft_first_logical_block = mft->current_MFT.logicalBlockNumber;
    int mft_number_blocks = mft->current_MFT.numberOfContiguosBlocks;
    int mft_number_sectors = mft_number_blocks * 4;
    int mft_first_sector = mft_first_logical_block * 4;

    int sector_starting_byte = 0; // Byte in which the reading from the sector must start

    int mft_virtual_blocks_passed = mft->current_MFT.virtualBlockNumber;
    int mft_first_virtual_sector = 4 * mft_virtual_blocks_passed;

    // If the current_pointer byte is in a block that this mft tuple maps or in a tuple previous to this one
    if (current_pointer_block <= mft_virtual_blocks_passed){

      // Iterate through the contiguos sectors mapped by the mft tuple
      for (i=0; i<mft_number_sectors; i++){

        sector_starting_byte = 0; // Reading starts at byte 0 by default

        // If current_pointer is in this sector or a previous one
        if (current_pointer_sector <= i+mft_first_virtual_sector){

          // Is current_pointer is in this specific sector
          if (current_pointer_sector == i+mft_first_virtual_sector){
            sector_starting_byte = current_pointer - (256*current_pointer_sector);
          }

          read_sector(i+mft_first_sector, sector_buffer);

          if (bytes_left_to_copy + sector_starting_byte >= 256){ // If the amount of bytes to read is bigger than the amount of bytes in the sector
            
            read_bytes(sector_starting_byte, 256, 
                          256-sector_starting_byte, (char *) sector_buffer, (char *) buffer);
                          
            bytes_left_to_copy -= (256 - sector_starting_byte);

          }
          else{ // If all bytes to be read are in this sector
            
            read_bytes(sector_starting_byte, bytes_left_to_copy+sector_starting_byte, 
                          bytes_left_to_copy, (char *) sector_buffer, (char *) buffer);

            opened_files[handle].current_pointer = current_pointer + size + 1; // Moves current_pointer to the next byte from where reading was finished
            
            return size;  // Function ended correctly
          }
        }
      }
    }
    mft = mft->next;
  }
  // Function couldn't read everything
  return -1;
}

//Caio
//Allocates the handle into opened_files, and returns its index
//Returns -1 if the subdirectory the file is in can't be reached
//Returns -2 if the file can't be found on its subdirectory
//Returns -3 if the file is already opened
FILE2 open2 (char *filename){


	if (is_file_open(filename) == 1)
		return -3;

	return open_file (filename, SEARCHING_FILE);

}


//Opens directory
//Allocates the handle into opened_directories, and returns its index
//Returns -1 if the subdirectory the file is in can't be reached
//Returns -2 if the file can't be found on its subdirectory
//Returns -3 if the file is already opened
DIR2 opendir(char *filename){



	if (is_directory_open(filename))
		return -3;

	return open_file (filename, SEARCHING_DIRECTORY);


}

// Arateus
FILE2 create2(char *filename)
{
  FILE2 handle = 0;

  if(generic_create(filename, 1) != 1)
    return -1;
  else
    handle = open2(filename);


  return handle;
}

// Arateus
int mkdir2 (char *pathname)
{
  // DIR2 handle = 0;

  if(generic_create(pathname, 2) != 1)
    return -1;
  // else
  //  handle = opendir2(pathname);

  return 0;
}

// Ana
int write2(FILE2 handle, char *buffer, int size)
{
  int current_pointer = opened_files[handle].current_pointer;

  unsigned char temp_buffer[SECTOR_SIZE]; // Buffer that will recieve the sliced buffer content
  int blocks_needed = ceil((size + current_pointer) / 1024.0);
  int bytes_written = size;

  MFT *mft = read_MFT(opened_files[handle].first_MFT_tuple); // For tests only


  int parent_MFT_sector = get_parent_dir_MFT_sector(opened_files[handle].file_path);

  MFT *current_mft = read_MFT(parent_MFT_sector);

  struct t2fs_record directory_record = search_file_in_directory_given_MFT(strrchr(opened_files[handle].file_path, '/') + 1, current_mft);
  if (size > directory_record.bytesFileSize)
    directory_record.bytesFileSize = current_pointer + size;

  directory_record.blocksFileSize = blocks_needed;
  update_file_record_info(opened_files[handle].file_path, directory_record);
  // Update the current_pointer to the byte right after the last one written
  opened_files[handle].current_pointer = size + current_pointer + 1;

  MFT *last_mft = mft;

  // Gets to the last mft tuple
  while (last_mft->next != NULL)
  {
    last_mft = last_mft->next;
  }
  
  int block_size = ceil(opened_files[handle].fileSizeBytes / 1024.0);

  if (blocks_needed > block_size)
    blocks_needed -= block_size;

  alocate_needed_blocks(blocks_needed, mft, last_mft);
  int current_byte = 0;

  int i = 0, j = 0;
  MFT *all_mft_list = read_MFT(opened_files[handle].first_MFT_tuple);

  int current_pointer_block = current_pointer / 1024; // Virtual block in which current_pointer is in
  int current_pointer_sector = current_pointer / 256; // Logical sector in which first byte is in

  // Write to blocks indicated by the mft
  while (all_mft_list != NULL)
  { // Must be changed to mft-> next !=NULL when mft stores the tuple with attributeType = 0
    // MFT info for blocks and sectors to be read
    int mft_first_logical_block = all_mft_list->current_MFT.logicalBlockNumber;
    int mft_number_blocks = all_mft_list->current_MFT.numberOfContiguosBlocks;
    int mft_number_sectors = mft_number_blocks * 4;
    int mft_first_sector = mft_first_logical_block * 4;

    int sector_starting_byte = 0; // Byte in which the reading from the sector must start

    int mft_virtual_blocks_passed = all_mft_list->current_MFT.virtualBlockNumber;
    int mft_first_virtual_sector = 4 * mft_virtual_blocks_passed;

    // If the current_pointer byte is in a block that this mft tuple maps or in a tuple previous to this one
    if (current_pointer_block <= mft_virtual_blocks_passed)
    {

      // Iterate through the contiguos sectors mapped by the mft tuple
      for (j = 0; j < mft_number_sectors; j++)
      {

        sector_starting_byte = 0; // Reading starts at byte 0 by default

        // If current_pointer is in this sector or a previous one
        if (current_pointer_sector <= j + mft_first_virtual_sector)
        {

          // If current_pointer is in this specific sector
          if (current_pointer_sector == j + mft_first_virtual_sector)
          {
            sector_starting_byte = current_pointer - (256 * current_pointer_sector);
          }

          if (size > (256 - sector_starting_byte))
          { // Fills entire sector

            for (i = 0; i < (256 - sector_starting_byte); i++)
            {

              temp_buffer[i] = buffer[current_byte];

              current_byte++;
            }

            size -= (256 - sector_starting_byte);

            insert_in_sector(j + mft_first_sector, (char *)temp_buffer, sector_starting_byte, (256 - sector_starting_byte));
          }
          else
          { // Writes last bytes into sector

            for (i = 0; i < size; i++)
            {

              temp_buffer[i] = buffer[current_byte];

              current_byte++;
            }

            insert_in_sector(j + mft_first_sector, (char *)temp_buffer, sector_starting_byte, size);
            current_byte += size;

            return bytes_written;
          }
        }
      }
    }
    all_mft_list = all_mft_list->next;
  }
  // Function didn't write everything to the disk
  return -1;
}