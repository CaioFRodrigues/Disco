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
#include "../include/files.h"
#include "../include/auxiliar.h"


// Ana
int truncate2 (FILE2 handle){
  
  if (!opened_files[handle].is_valid)
    return -1;

  int current_pointer = opened_files[handle].current_pointer;
  MFT* mft = read_mft(opened_files[handle].first_MFT_tuple);
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

  t2fs_record file_register; // Mock structure

  int byte_position = find_byte_position_in_logical_block(mft, current_pointer);
  int current_virtual_block; //TODO: find this number
  
  // Update file size in bytes and blocks
  file_register.bytesFileSize = current_pointer + 1;
  file_register.blocksFileSize = current_virtual_block + 1;

  while (mft != NULL){
    if (mft->current_MFT.virtualBlockNumber + mft->current_MFT.numberOfContiguosBlocks - 1 < current_virtual_block){
      mft = mft->next;  // Byte in which the trucation starts is not in this MFT tuple
    }
    else{
      if (mft->current_MFT.virtualBlockNumber > current_virtual_block){
        mft->current_MFT.atributeType = -1; // Invalidate MFT tuple
      }
      else{
        //Byte in which the truncation starts belongs to this MFT
        mft->current_MFT.numberOfContiguosBlocks =  mft->current_MFT.numberOfContiguosBlocks - (current_virtual_block - mft->current_MFT.virtualBlockNumber);
      }
    }
  }
}