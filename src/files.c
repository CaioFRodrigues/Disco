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
#include "../include/files.h"
#include "../include/bitmap2.h"
#include "../include/t2fs.h"
#include "../include/MFT_list.h"
#include "../include/MFT.h"


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
            
            unsigned char temp_buffer[256-sector_starting_byte+1]; // Temporary buffer that stores bytes to be appended to buffer
            
            k = 0;

            // Copy wanted bytes only
            for (j=sector_starting_byte; j<256; j++){
              temp_buffer[k] = sector_buffer[j];
              k++;
            }
            temp_buffer[k] = '\0';

            buffer = append_buffers(buffer, temp_buffer);
            bytes_left_to_copy -= (256 - sector_starting_byte);

          }
          else{ // If all bytes to be read are in this sector

            unsigned char temp_buffer[bytes_left_to_copy]; // Temporary buffer that stores bytes to be appended to buffer

            k = 0;

            // Copy wanted bytes only
            for (j=sector_starting_byte; j<bytes_left_to_copy+sector_starting_byte; j++){
              temp_buffer[k] = sector_buffer[j];
              k++;
            }
            temp_buffer[k] = '\0';

            buffer = append_buffers(buffer, temp_buffer);
            opened_files[handle].current_pointer = current_pointer + size + 1; // Moves current_pointer to the next byte from where reading was finished
            
            return size;
          }
        }
      }
    }

    mft = mft->next;
  }

  // Function couldn't read everything
  return -1;
}