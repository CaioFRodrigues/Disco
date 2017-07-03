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

#include "../include/directories.h"

//Caio
//Given a block number from a directory and a name, finds the MFT sector belonging to that name
//Parameters:
//block: Block to be searched in disk
//*name: string to be searched in the directory
//mode: defines whether the function will search for a file or directory
//       1 - Searches for a directory
//       2 - Searches for a file

//Return: MFT of the file with name *name
//if it fails, returns 0 
int get_MFTnumber_from_directory_datablock(int block, char *name, int mode){
  unsigned char buffer[SECTOR_SIZE];

  //For every sector in a block
  for (int current_sector_in_block = 0; current_sector_in_block < boot_block.blockSize;current_sector_in_block++){
  	
  	//Describes the current sector to be searched from both offsets
  	int current_sector = (block * (int) boot_block.blockSize) + current_sector_in_block;
  	
  	read_sector(current_sector, buffer);

  	//For every record in a block
  	for(int current_record = 0; current_record < RECORDS_PER_SECTOR; current_record++){
  		//Gets the info of the current record
  		struct t2fs_record current_file = fill_directory(buffer, current_record);
		
		//checks if it is valid  	
  		if (current_file.TypeVal == 1 || current_file.TypeVal == 2){
  			//check if the name and type are the same
	  		if(strcmp(current_file.name, name) == 0 && current_file.TypeVal == mode){
	  			return current_file.MFTNumber;

	  		}
  		}
  	}

  }

  return 0;
} 


//Caio
//Given a buffer and an offset, returns all the information from the N register of the buffer
struct t2fs_record fill_directory(unsigned char* buffer, int directory_number){
  //Struct where info will be saved
  struct t2fs_record directory;

  //Byte where the the tupla starts on the buffer
  int directory_start =  directory_number * RECORD_SIZE;


  //Gets the informtion
  directory.TypeVal = buffer[directory_start];

  if (directory.TypeVal == 1 || directory.TypeVal == 2){
	  //Copies the name to the directory with a '\0' on the end
	  for(int i = 1; i < NAME_SIZE + NAME_START; i++){
	  	if (buffer[directory_start + i] != 0)
	  		directory.name[i-1] = buffer[directory_start + i];
	  	else{
	  		directory.name[i-1] = '\0';
	  		break;
	  	}
	  }

	  //Fills the directory with the info, making sure to swap to big endian when needed
	  directory.blocksFileSize = swap_local_endianess(buffer, BLOCKS_FILE_SIZE_START + directory_start);
	  directory.bytesFileSize = swap_local_endianess(buffer, BYTES_FILE_SIZE_START + directory_start);
	  directory.MFTNumber = swap_local_endianess(buffer, MFT_NUMBER_START + directory_start);

	}
  return directory;
}

