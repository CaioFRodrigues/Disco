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

#define ATRIB_TYPE 0
#define VBN 4
#define LBN 8
#define CONTIG_BLOCK 12

#define TUPLE_ATRTYPE 0 
#define TUPLE_VBN 4
#define TUPLE_LBN 8
#define TUPLE_NUMCONTIGBLOCK 12

// Ana
int truncate2 (FILE2 handle){
  
  if (!opened_files[handle].is_valid)
    return -1;

  int current_pointer = opened_files[handle].current_pointer;
  MFT* mft = read_MFT(opened_files[handle].first_MFT_tuple);
  clear_file(mft, current_pointer);
  opened_files[handle].fileSizeBytes = current_pointer + 1;
}

// Ana
int delete2 (char *filename){

  /*
  * TODO:
  *     Return -1 if filename is invalid
  *     Check if file is opened
  *       If it is, in the end of the process its file descriptor must be marked as invalid
  */

  MFT* mft; // Mock structure
  clear_file(mft, 0);
}

// Ana
int clear_file(MFT* mft, int current_pointer){
  
  /*
  * TODO:
  *   Make sure to save the changes in the file register and the mft in the disk, not only in memory
  */

  struct t2fs_record file_register; // Mock structure

  int pointer_block = current_pointer/1024;
  // Update file size in bytes and blocks
  // file_register.bytesFileSize = current_pointer + 1;
  // file_register.blocksFileSize = current_virtual_block + 1;

  int i =0;
  while (mft != NULL){
    int passed_blocks = mft->current_MFT.virtualBlockNumber;
    int number_blocks = mft->current_MFT.numberOfContiguosBlocks;
    int first_block = mft->current_MFT.logicalBlockNumber;

    // MFT MUST BE CHANGED
    if (passed_blocks+number_blocks-1 >= pointer_block){
      
      if (passed_blocks <= pointer_block){
        // Current_pointer is in this MFT
        int blocks_left = number_blocks;
        for (i=0; i<number_blocks; i++){
          if (i > pointer_block - passed_blocks){
            setBitmap2(first_block + i, 0);
            blocks_left--;
          }
        }

        mft->current_MFT.numberOfContiguosBlocks = blocks_left;
        write_first_tuple_MFT_and_set_0_second(mft->sector, mft->offset*16, mft->current_MFT);

      }

      else{
        // MFT must be invalidated
        for (i=0; i<number_blocks; i++){

          if (passed_blocks + i > pointer_block){
            setBitmap2(first_block + i, 0);
          }
          
          mft->current_MFT.atributeType = 0xffff;
          write_first_tuple_MFT_and_set_0_second(mft->sector, mft->offset*16, mft->current_MFT);

        }

      }

    }
    mft = mft->next;
  }	
  return 1;
}

int write_first_tuple_MFT_and_set_0_second(unsigned int sector, int offset, struct t2fs_4tupla t)
{
	unsigned char buffer[SECTOR_SIZE];
	int error = read_sector(sector, buffer);
	unsigned int zero = 0x00;
	if (error)
		return -1;

	unsigned int aux;
	// write AtributeType in the first tuple in the MFT
	int i;
	for (i = 0; i < 4; i++)
	{
		aux = (t.atributeType >> 8 * i) & 0xff;
		buffer[TUPLE_ATRTYPE + i + offset] = aux;
	}
	// write virtualBlockNumber
	for (i = 0; i < 4; i++)
	{
		aux = (t.virtualBlockNumber >> 8 * i) & 0xff;
		buffer[TUPLE_VBN + i + offset] = aux;
	}
	// write logicalBlockNumber
	for (i = 0; i < 4; i++)
	{
		aux = (t.logicalBlockNumber >> 8 * i) & 0xff;
		buffer[TUPLE_LBN + i + offset] = aux;
	}
	// write numberOfContiguousBocks
	for (i = 0; i < 4; i++)
	{
		aux = (t.numberOfContiguosBlocks >> 8 * i) & 0xff;
		buffer[TUPLE_NUMCONTIGBLOCK + i + offset] = aux;
	}
	// write 0 in the second tuple
	for (i = 0; i < 4; i++)
	{
		buffer[TUPLE_ATRTYPE + 16 + i + offset] = zero;
	}

	int write_error = write_sector(sector, buffer);
	if (write_error)
		return -1;

	return 1;
}