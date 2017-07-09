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

  // MFT* mft = read_MFT(opened_files[handle].first_MFT_tuple);
  MFT* mft = read_MFT(12);  // For testing only
  unsigned char sector_buffer[SECTOR_SIZE]; // Buffer used to read from sector
  buffer[size] = '\0';
  int first_byte = 0;

  int copy_size = size;

  /*
    Treat case in which size + current_pointer > file_size
  */

  int pointer_block = first_byte/1024; // Virtual block in which first_byte is in
  int pointer_sector = first_byte/256; //Logical sector in which first byte is in

  while (mft != NULL && size > 0){

    int first_block = mft->current_MFT.logicalBlockNumber;
    int number_blocks = mft->current_MFT.numberOfContiguosBlocks;

    int num_sectors = number_blocks * 4;
    int first_sector = first_block * 4;

    int i =0,j=0, k=0;

    int current_sector_pointer = 0;

    int passed_blocks = mft->current_MFT.virtualBlockNumber;
    int passed_sectors = 4 * passed_blocks;
    if (pointer_block <= passed_blocks){

      for (i=0; i<num_sectors; i++){

        current_sector_pointer = 0;
        if (pointer_sector <= i+passed_sectors){

          if (pointer_sector == i+passed_sectors){
            current_sector_pointer = first_byte - (256*pointer_sector);
          }

          read_sector(i+first_sector, sector_buffer);

          int sector_current = ceil(ceil(first_byte/256.0)/4.0)-1;

          if (sector_current == i){
            current_sector_pointer = first_byte;
          }

          if (size + current_sector_pointer >= 256){ // If the amount of bytes to read is bigger than the amount of bytes in the sector
            unsigned char temp_buffer[256-current_sector_pointer+1]; // Temporary buffer that stores bytes to be appended to buffer
            k = 0;
            // Copy wanted bytes only
            for (j=current_sector_pointer; j<256; j++){
              temp_buffer[k] = sector_buffer[j];
              k++;
            }
            temp_buffer[k] = '\0';
            buffer = append_buffers(buffer, temp_buffer);
            size -= (256 - current_sector_pointer);
          }
          else{ // If all bytes to be read are in this sector
            unsigned char temp_buffer[size]; // Temporary buffer that stores bytes to be appended to buffer

            k = 0;
            // Copy wanted bytes only
            for (j=current_sector_pointer; j<size+current_sector_pointer; j++){
              temp_buffer[k] = sector_buffer[j];
              k++;
            }
            temp_buffer[k] = '\0';
            buffer = append_buffers(buffer, temp_buffer);
            return copy_size;
          }
        }
      }

    }
    mft = mft->next;
  }

  opened_files[handle].current_pointer = first_byte + size + 1; // Moves current_pointer to the next byte from where reading was finished

  return -1;
}

// Ana
/*
*   TODO:
*     Adapt this to copy the right amount of bytes to the final buffer
*/
int read_bytes_from_sector(int sector, int num_bytes){
  unsigned char buffer[SECTOR_SIZE];
  unsigned char bytes_read[num_bytes];

  int error;

  error = read_sector(sector, buffer);

  if (error)
    return -1;

  int i =0;

  for (i=0; i<num_bytes; i++){
    bytes_read[i] = buffer[i];
  }

  for (i=0; i<num_bytes; i++){
    printf("%02X\n", bytes_read[i]);
  }

  return 0;
}