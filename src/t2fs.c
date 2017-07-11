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
 #include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/auxiliar.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/MFT_list.h"
#include "../include/MFT.h"
#include "../include/directories.h"
#include "../include/math.h"

// Ana
int write2(FILE2 handle, char * buffer, int size) {
printf("oi\n");
    // int current_pointer = opened_files[handle].current_pointer;
    int current_pointer = 0; // For tests only
    unsigned char temp_buffer[SECTOR_SIZE]; // Buffer that will recieve the sliced buffer content
    int blocks_needed = ceil((size + current_pointer) / 1024.0);
    int bytes_written = size;
    // MFT* mft = read_MFT(opened_files[handle].first_MFT_tuple);
    MFT * mft = read_MFT(opened_files[handle].first_MFT_tuple); // For tests only


    // Update the current_pointer to the byte right after the last one written
    opened_files[handle].current_pointer = size + current_pointer + 1;


    /*
      TODO:
        Actually update the record
      struct t2fs_record directory_record;    This is gonna be found by another function
      directory_record.blocksFileSize = blocks_needed;
      if (size > directory_record.bytesFileSize)
        directory_record.bytesFileSize = size;

      write record to disk

    */

    int parent_MFT_sector = get_parent_dir_MFT_sector(opened_files[handle].file_path);
    



		//Gets current MFTSector
		char *isolated_filename = (strrchr(opened_files[handle].file_path, '/'));
	  isolated_filename = isolated_filename + 1;
		int parent_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, parent_MFT_sector, SEARCHING_FILE);
		printf ("MFT FILE: %d\n ", parent_position);
		int parent_mft_sector = (parent_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE + 2 ;

		MFT* current_mft = read_MFT(parent_mft_sector);
		printf ("MFT FILE: %d\n ", parent_mft_sector);

		struct t2fs_record directory_record = search_file_in_directory_given_MFT( strrchr(opened_files[handle].file_path, '/'), current_mft);
    if (size > directory_record.bytesFileSize)
      directory_record.bytesFileSize = size;

    printf ("%d %s %d\n\n", directory_record.TypeVal, directory_record.name, directory_record.blocksFileSize);

    update_file_record_info(opened_files[handle].file_path, directory_record);


    MFT * last_mft = mft;

    // Gets to the last mft tuple
    while (last_mft -> next != NULL) {
      last_mft = last_mft -> next;
    }
    int block_size = ceil(opened_files[handle].fileSizeBytes/1024.0);

    if (blocks_needed > block_size)
      blocks_needed -= block_size;

    alocate_needed_blocks(blocks_needed, mft, last_mft);
    int current_byte = 0;

    int i = 0, j = 0;
    MFT * all_mft_list = read_MFT(12);

    int current_pointer_block = current_pointer / 1024; // Virtual block in which current_pointer is in
    int current_pointer_sector = current_pointer / 256; // Logical sector in which first byte is in

    // Write to blocks indicated by the mft
    while (all_mft_list != NULL) { // Must be changed to mft-> next !=NULL when mft stores the tuple with attributeType = 0

      // MFT info for blocks and sectors to be read
      int mft_first_logical_block = all_mft_list-> current_MFT.logicalBlockNumber;
      int mft_number_blocks = all_mft_list -> current_MFT.numberOfContiguosBlocks;
      int mft_number_sectors = mft_number_blocks * 4;
      int mft_first_sector = mft_first_logical_block * 4;

      int sector_starting_byte = 0; // Byte in which the reading from the sector must start

      int mft_virtual_blocks_passed = all_mft_list -> current_MFT.virtualBlockNumber;
      int mft_first_virtual_sector = 4 * mft_virtual_blocks_passed;

      // If the current_pointer byte is in a block that this mft tuple maps or in a tuple previous to this one
      if (current_pointer_block <= mft_virtual_blocks_passed) {

        // Iterate through the contiguos sectors mapped by the mft tuple
        for (j = 0; j < mft_number_sectors; j++) {

          sector_starting_byte = 0; // Reading starts at byte 0 by default

          // If current_pointer is in this sector or a previous one
          if (current_pointer_sector <= j + mft_first_virtual_sector) {

            // If current_pointer is in this specific sector
            if (current_pointer_sector == j + mft_first_virtual_sector) {
              sector_starting_byte = current_pointer - (256 * current_pointer_sector);
            }

            if (size > (256 - sector_starting_byte)) { // Fills entire sector

                for (i = 0; i < (256 - sector_starting_byte); i++) {

                  temp_buffer[i] = buffer[current_byte];

                  current_byte++;

                }

                size -= (256 - sector_starting_byte);

                insert_in_sector(j + mft_first_sector, (char * ) temp_buffer, sector_starting_byte, (256 - sector_starting_byte));

              } else { // Writes last bytes into sector

                for (i = 0; i < size; i++) {

                  temp_buffer[i] = buffer[current_byte];

                  current_byte++;

                }

                insert_in_sector(j + mft_first_sector, (char * ) temp_buffer, sector_starting_byte, size);
                current_byte += size;

                return bytes_written;
              }
            }
          }
        }
        all_mft_list = all_mft_list -> next;
      }
      // Function didn't write everything to the disk
      return -1;
    }


//Caio
//Open2: Searches the filename on the tree of directories, then returns allocates the handler to the array of opened files
FILE2 open2 (char *filename){
 	

	int director_sector = get_parent_dir_MFT_sector(filename);

	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));
  isolated_filename = isolated_filename + 1;
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
	

	int i = first_free_file_position();
	opened_files[i] = file_descriptor;
	printf ("o2asreasrei\n\n");
	return 0;




}


DIR2 opendir(char *filename){

	int director_sector = get_parent_dir_MFT_sector(filename);

	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));
  isolated_filename = isolated_filename + 1;
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

