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

  int i =0,j=0, k=0;

  // MFT* mft = read_MFT(opened_files[handle].first_MFT_tuple);
  MFT* mft = read_MFT(12);  // For testing only

  unsigned char sector_buffer[SECTOR_SIZE]; // Buffer used to read from sector
  buffer[size] = '\0';

  // int current_pointer = opened_files[handle].current_pointer
  int current_pointer = 0;   // For testing only

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
                          256-sector_starting_byte, sector_buffer, buffer);
                          
            bytes_left_to_copy -= (256 - sector_starting_byte);

          }
          else{ // If all bytes to be read are in this sector
            
            read_bytes(sector_starting_byte, bytes_left_to_copy+sector_starting_byte, 
                          bytes_left_to_copy, sector_buffer, buffer);

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
//Open2: Searches the filename on the tree of directories, then returns allocates the handler to the array of opened files
FILE2 open2 (char *filename){
 	

	int director_sector = get_parent_dir_MFT_sector(filename);

	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));

	int file_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, director_sector, SEARCHING_DIRECTORY);
	

	int file_mft_sector = (file_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(filename);

	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.first_MFT_tuple = file_mft_sector;

	//Declares the file as valid
	file_descriptor.is_valid = 1;
	


	opened_files[first_free_file_position()] = file_descriptor;

	return 0;




}


DIR2 opendir(char *filename){

	int director_sector = get_parent_dir_MFT_sector(filename);

	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));

	int dir_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, director_sector, SEARCHING_DIRECTORY);
	

	int dir_mft_sector = (dir_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(filename);

	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.first_MFT_tuple = dir_mft_sector;

	//Declares the file as valid
	file_descriptor.is_valid = 1;
	
	int index = first_free_dir_position();

	opened_directories[index] = file_descriptor;

	return 0;


}

