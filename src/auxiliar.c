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
#include <string.h>
#include <stdio.h>
#include <math.h>


#include "../include/auxiliar.h"
#include "../include/MFT.h"
#include "../include/MFT_list.h"

struct t2fs_bootBlock boot_block;

//Ana
//Initializes the boot block with the info from the file
int init(){
  int error;
  unsigned char buffer[SECTOR_SIZE];

  error = read_sector(0, buffer);
  if (error)
    return -1;

  int i =0;
  for (i=ID; i<ID+4; i++){
      boot_block.id[i]=buffer[i];
  }

  boot_block.version = (buffer[VERSION+1] << 8) | buffer[VERSION];

  boot_block.blockSize = (buffer[BLOCKSIZE+1] << 8) | buffer[BLOCKSIZE];

  boot_block.MFTBlocksSize = (buffer[MFTBLOCKSSIZE+1] << 8) | buffer[MFTBLOCKSSIZE];

  boot_block.diskSectorSize = (buffer[DISKSECTORSIZE+3] << 16) | (buffer[DISKSECTORSIZE+2] << 12) | (buffer[DISKSECTORSIZE+1] << 8) | buffer[DISKSECTORSIZE];

  initialize_open_files();

  return 0;
}

//Caio
//Initializes the array of opened files
void initialize_open_files(){

  for(int i = 0; i < MAX_OPENED_FILES; i++)
    opened_files[i].is_valid = 0;

}


//Caio
//Gets first possible position from opened_files
//Returns -1 if 20 files have been opened
int first_free_file_position(){

  for (int i = 0; i < MAX_OPENED_FILES; i++){
    if (!opened_files[i].is_valid)
      return i;
  }

  return -1;

}

//Ana
int virtual_block_to_logical_block(DWORD current_pointer, MFT* mft_list){

  MFT* mft_list_copy = mft_list;  
  DWORD currentVirtualBlockNumber, numberOfContiguosBlocks;

  while (mft_list_copy != NULL){
      
    currentVirtualBlockNumber = mft_list_copy->current_MFT.virtualBlockNumber;
    numberOfContiguosBlocks = mft_list_copy->current_MFT.numberOfContiguosBlocks;
    
    if (numberOfContiguosBlocks + currentVirtualBlockNumber - 1 < current_pointer){ // If current_pointer is not mapped in this tuple
      mft_list_copy = mft_list_copy->next;
    }
    else{
      // currentVirtualBlockNumber maps to logicalBlockNumber, current_pointer = logicalBlockNumber + offset
      return mft_list_copy->current_MFT.logicalBlockNumber + (current_pointer - currentVirtualBlockNumber);
    }
  }
}

// Ana
//  Given a byte offset and an mft list, finds the byte offset relative to the 
//logical block number in which the wanted byte will be
//  Example: bytes = 500
//  The 500th byte will be in the file's second virtualBlock, and it will
//be the 12th byte from the beginning of the equivalent logicacl block
int find_byte_position_in_logical_block(MFT* mft, int bytes){
  
  int num_sectors, num_blocks, offsets_bytes;

  num_sectors = ceil(bytes/256.0);
  num_blocks = ceil(bytes/1024.0);

  offsets_bytes = (num_blocks * num_sectors * 256) - bytes;
  
  return offsets_bytes;

}

// Arateus
//funcao recebe um path e retorna um record com o arquivo ou dir
struct t2fs_record path_return_record(char* path)
{
  char *token, *filenamecopy;
  unsigned int MFT_sec = 6; // stars with root register, which is the register 1
  unsigned char bufferMFT[SECTOR_SIZE];
  unsigned char bufferBD[SECTOR_SIZE];
  struct t2fs_4tupla t;
  struct t2fs_record record;
  int error;
  unsigned int s;

  filenamecopy = strdup(path);
  
  token = strtok(filenamecopy, "/");
  

  //Initializes the root MFT
  int current_dir_sector = ROOT_MFT;
  
  //isolated_filename is the path without the subdirectories it is in
  char *isolated_filename = (strrchr(path, '/'));
  isolated_filename = isolated_filename + 1;
  
  //Goes through the subdirectories
  while(strcmp(token,isolated_filename) != 0){
    MFT_sec = get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
  }

  // search at MFT for desired info
  int i;
  int j;
  int k;
  int p;
  int r;
  for(i = 0; i < 2; i++)
  {// going through sectors
    error = read_sector(MFT_sec + i, bufferMFT); // reading MFT register
    if(error)
      break;
    for (j = 0; j < 16; j++)
    { // going tuple by tuple in the sector
      t = fill_MFT(bufferMFT, j);

      if(t.atributeType == 0 || t.atributeType == -1){ // if end or non ecziste return error
        // record = (struct t2fs_record)malloc(sizeof(struct t2fs_record));
        return record;
      }
      else if(t.atributeType == 2){ // if reached the end of the MFT register (the last tuple) go to the next register
        i = 0;
        MFT_sec = t.virtualBlockNumber * 2 + 4; // recebe o registro e o converte pro setor desse registro
      }
        
      else{
        for(k = 0; k < t.numberOfContiguosBlocks; k++)
        { // reading BD
          s = (unsigned int)((t.logicalBlockNumber + k) * 4); // find the sector of the respective blocks
          for(p = 0; p<4; p++) // read the whole block
          {
            error = read_sector(s + p, bufferBD); // reading MFT register
            if(error){
              // record = (struct t2fs_record)malloc(sizeof(struct t2fs_record));
              return record;
            }
            for (r = 0; r < 4; r++)
            { //each sector has max 4 records
              record = fill_directory(bufferBD, r);
              if(strcmp(isolated_filename, record.name)==0)
                return record;
            }
          }
        }
      }

    }
  }

  // record = (struct t2fs_record)malloc(sizeof(struct t2fs_record));
  return record;
}

// Arateus
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