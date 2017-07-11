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

    // int current_pointer = opened_files[handle].current_pointer;
    int current_pointer = 0; // For tests only
    unsigned char temp_buffer[SECTOR_SIZE]; // Buffer that will recieve the sliced buffer content
    int blocks_needed = ceil((size + current_pointer) / 1024.0);
    int bytes_written = size;
    // MFT* mft = read_MFT(opened_files[handle].first_MFT_tuple);
    MFT * mft = read_MFT(12); // For tests only

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

    MFT * last_mft = mft;

    // Gets to the last mft tuple
    while (last_mft -> next != NULL) {
      last_mft = last_mft -> next;
    }

    // int block_size = ceil(opened_files[handle].fileSizeBytes/1024.0);
    int block_size = 1; // For tests only

    if (blocks_needed > block_size)
      blocks_needed -= block_size;

    alocate_needed_blocks(blocks_needed, mft, last_mft);

    int current_byte = 0;

    int i = 0, j = 0;
    MFT * all_mft_list = read_MFT(12);

    int current_pointer_block = current_pointer / 1024; // Virtual block in which current_pointer is in
    int current_pointer_sector = current_pointer / 256; // Logical sector in which first byte is in
    int sector_starting_byte = 0;

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
      printf("oi\n");
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
                printf("%d\n", mft_first_sector + j);
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