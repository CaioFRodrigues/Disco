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

  int parent_MFT_sector = get_parent_dir_MFT_sector(opened_files[handle].file_path);
  
  MFT* current_mft = read_MFT(parent_MFT_sector);

  struct t2fs_record file_record = search_file_in_directory_given_MFT( strrchr(opened_files[handle].file_path, '/') + 1, current_mft);

  file_record.blocksFileSize -= ceil(current_pointer/1024.0);
  file_record.bytesFileSize = current_pointer + 1 ;

  update_file_record_info(opened_files[handle].file_path, file_record);

  // Write record to disk

  clear_file(mft, current_pointer);
  opened_files[handle].fileSizeBytes = current_pointer + 1;
  return 0;
}

// Ana
int delete2 (char *filename){

  int parent_MFT_sector = get_parent_dir_MFT_sector(opened_files[handle].file_path);
  
  MFT* current_mft = read_MFT(parent_MFT_sector);

  struct t2fs_record file_record = search_file_in_directory_given_MFT( strrchr(filename, '/') + 1, current_mft);

  // Initialize it with path_return_record
  file_record.TypeVal = 0;

  update_file_record_info(filename, file_record);

  // Write record to disk
  
  MFT* mft; // Mock structure
  clear_file(mft, 0);
  return 0;
}

int rmdir2(char* pathname){

  int parent_MFT_sector = get_parent_dir_MFT_sector(opened_files[handle].file_path);
  
  MFT* current_mft = read_MFT(parent_MFT_sector);

  struct t2fs_record file_record = search_file_in_directory_given_MFT( strrchr(pathname, '/') + 1, current_mft);

  // Initialize it with path_return_record
  struct t2fs_record file_record;

  file_record.TypeVal = 0;
  update_file_record_info(pathname, file_record);

  // Write record to disk
  
  MFT* mft; // Mock structure
  clear_file(mft, 0);

}

// Ana
int clear_file(MFT* mft, int current_pointer){

  int pointer_block = current_pointer/1024;

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
  return 0;
}