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

  for (int i = 0; i < 51; i++)
  	directory.name[i] = 0x00;
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

//Given a filename, returns THE SECTOR OF THE MFT it belongs
//if the file was not found, returns -1
int get_parent_dir_MFT_sector(char *filename){
	char *token, *filenamecopy;

	filenamecopy = strdup(filename);
	
	token = strtok(filenamecopy, "/");
	
	//Initializes the root MFT
	int current_dir_sector = ROOT_MFT;
	
	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));
	isolated_filename = isolated_filename + 1;
	
	//Goes through the subdirectories
	while(strcmp(token,isolated_filename) != 0){
		
		current_dir_sector = get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);

		//If the file is not on the directory		
		if (current_dir_sector == -1)
			return -1;

		//Recalculates the current_dir_sector
		current_dir_sector = (current_dir_sector * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;
		// printf ("THIS IS NOW:%d", current_dir_sector);
		token = strtok(NULL, "/");
	}

	free(filenamecopy);
	return current_dir_sector;
	// if(current_dir_sector != 4)
	// 	return current_dir_sector;
	// else
	// 	return -1;

}


//Caio
//Given a t2fs_record and a filename, finds where the file is located and writes the content of the t2fs_record into it
//Returns 0 if it has success, -1 if it fails
int update_file_record_info(char * filename, struct t2fs_record record){

	int directory_MFT_sector = get_parent_dir_MFT_sector(filename);

	MFT * directory_MFT = read_MFT(directory_MFT_sector);

	MFT * iterator = directory_MFT;

	while (iterator != NULL){
		int next_block = iterator->current_MFT.logicalBlockNumber * 4;
	
		for (int current_block = 0; current_block < directory_MFT->current_MFT.logicalBlockNumber; current_block++){

			for (int current_sector = 0; current_sector <  boot_block.blockSize; current_sector++){
				 
				int next_sector = next_block + current_sector + (current_block * boot_block.blockSize);

				int error = update_record_info(filename, record, next_sector);
				if (error == 0)
					return 0;
			}

		}

		iterator = iterator->next;

	}


	return -1;



}


//Caio
// Given a name, a record and a buffer,  writes the record into the buffer of the file_name
//Returns -1 if it has success, -1 if it fails
int update_record_info(char *filename, struct t2fs_record record, int sector){

	//Reads current sector
	unsigned char buffer[SECTOR_SIZE];
	read_sector(sector, buffer);

	//Real filename, without the directories
	char *isolated_filename = (strrchr(filename, '/'));
	isolated_filename = isolated_filename + 1;


	for (int i = 0; i < RECORDS_PER_SECTOR; i++) {

		struct t2fs_record current_record = fill_directory(buffer, i);

		//Found the record, write in it
		if (strcmp(isolated_filename, current_record.name) == 0){
			write_record_in_dir(sector, i * RECORD_SIZE, record);
			return 0;
		}

	}

	return -1;
}


struct t2fs_record search_file_in_directory(char *name, int dir_sector){

  unsigned char buffer[SECTOR_SIZE];


  //For every sector in a block
  for (int current_sector_in_block = 0; current_sector_in_block < boot_block.blockSize;current_sector_in_block++){
  	
  	//Describes the current sector to be searched from both offsets
  	int current_sector = dir_sector + current_sector_in_block;
  	
  	read_sector(current_sector, buffer);


  	//For every record in a block
  	for(int current_record = 0; current_record < RECORDS_PER_SECTOR; current_record++){
  		//Gets the info of the current record
  		struct t2fs_record current_file = fill_directory(buffer, current_record);

		//checks if it is valid  	
  		if (current_file.TypeVal == 1 || current_file.TypeVal == 2){
  			//check if the name and type are the same

	  		if(strcmp(current_file.name, name) == 0){

	  			return current_file;

	  		}
  		}
  	}

  }

  //Failed

  struct t2fs_record failed;
  failed.TypeVal = 0;
  return failed;



}


struct t2fs_record search_file_in_directory_given_MFT(char *name, MFT * mft){

	MFT * aux = mft;

	struct t2fs_record new_struct;
	while (aux != NULL){

		//For every node goes through the contiguous blocks
		for (int current_block = 0; current_block < aux->current_MFT.numberOfContiguosBlocks; current_block++){
			//Gets next position by comparing the strings
			int current_sector = current_block  + aux->current_MFT.logicalBlockNumber * 4;
			printf ("oi oi oi %d\n", current_sector);
			new_struct = search_file_in_directory(name, current_sector);
			
			//If it has found the file,returns it
			printf ("This is what we've got: %d", new_struct.TypeVal);
			if (new_struct.TypeVal != 0)
				return new_struct;

		}
		aux = aux->next;
	}

	return new_struct;
}