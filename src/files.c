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
int write2(FILE2 handle, char* buffer, int size){

  int first_byte = 2000;

  int blocks_needed = ceil((size + first_byte)/1024.0);
  int n = 0;

	MFT* mft = read_MFT(12);

  /*
    TODO:
      Actually update the record
    struct t2fs_record directory_record;    This is gonna be found by another function
    directory_record.blocksFileSize = blocks_needed;
    if (size > directory_record.bytesFileSize)
      directory_record.bytesFileSize = size;

    write record to disk

  */
  MFT *last_mft = mft;

  while (last_mft->next != NULL){
    last_mft =last_mft->next;
  }


  int block_size = 3;

  // if (blocks_needed > ceil(opened_files[handle].bytesFileSize)/1024.0)
  if (blocks_needed > block_size){
    // blocks_needed -= ceil(opened_files[handle].bytesFileSize)/1024.0;
    blocks_needed -= block_size;

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
  }

  unsigned char sector_buffer[SECTOR_SIZE]; // Buffer used to read from sector
  unsigned char temp_buffer[SECTOR_SIZE]; // Buffer that will recieve the sliced buffer content

  int current_byte = 0;
  int current_sector_pointer = 0;
  int i=0, j=0;
  MFT* all_mft_list = read_MFT(12);

  int pointer_block = first_byte/1024; // Virtual block in which first_byte is in
  int pointer_sector = first_byte/256; //Logical sector in which first byte is in

	while (all_mft_list != NULL){    // Must be changed to mft-> next !=NULL when mft stores the tuple with attributeType = 0
		
    int passed_blocks = all_mft_list->current_MFT.virtualBlockNumber;
    int passed_sectors = 4 * passed_blocks;
    if (pointer_block <= passed_blocks){

      int number_blocks = all_mft_list->current_MFT.numberOfContiguosBlocks;
      int first_block = all_mft_list->current_MFT.logicalBlockNumber;

      int num_sectors = number_blocks * 4;
      int first_sector = first_block * 4;        

      for (j=0; j<num_sectors; j++){
        current_sector_pointer = 0;
        if (pointer_sector <= j+passed_sectors){

          if (pointer_sector == j+passed_sectors){
            current_sector_pointer = first_byte - (256*pointer_sector);
          }

          if (size > (256-current_sector_pointer)){   // Fills entire sector

            for (i=0; i<(256-current_sector_pointer); i++){
              temp_buffer[i] = buffer[current_byte];
              current_byte++;
            }
            size -= (256-current_sector_pointer);
            insert_in_sector(j+first_sector, temp_buffer, current_sector_pointer, (256-current_sector_pointer));
          }
          else{   // Writes last bytes into sector
            for (i=0; i<size; i++){
              temp_buffer[i] = buffer[current_byte];
              current_byte++;
            }

            insert_in_sector(j+first_sector, temp_buffer, current_sector_pointer, size);
            return 0;
          }

        }

      }

    }


    all_mft_list = all_mft_list->next;
	}

}

// Ana
// Given a buffer with content to be inserted in a sector,
// inserts it in the given place and writes it to the sector
int insert_in_sector(int sector, char* content, int start, int size){

    char sector_content[SECTOR_SIZE];

    read_sector(sector, sector_content);

    int i=0, k=0;

    k = start;
    for (i=0; i<size; i++){
        sector_content[k] = content[i];
        k++;
    }

    write_sector(sector, sector_content);
}