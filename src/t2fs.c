#include <string.h>
#include <stdio.h>
#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/auxiliar.h"
#include "../include/MFT.h"

//Caio
//Open2: Searches the filename on the tree of directories, then returns allocates the handler to the array of opened files
FILE2 open2 (char *filename){
 	
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
		get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
	}

	int file_position = get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_FILE);
	

	int file_mft_sector = (file_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(isolated_filename);

	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.first_MFT_tuple = file_mft_sector;

	//Declares the file as valid
	file_descriptor.is_valid = 1;
	
	isolated_filename = isolated_filename - 1;
	free(isolated_filename);
	


	opened_files[first_free_file_position()] = file_descriptor;

	return 0;


}

// Arateus
FILE2 create2(char *filename)
{
  FILE2 handle;
  char *token, *filenamecopy;
  int depht = 0; // 0 - is root
  unsigned int empty_MFTRegister;
  unsigned int empty_MFT_sector;
  unsigned int byte_position;
  unsigned int byte_sector;
  unsigned int byte_record_pos;
  unsigned int empty_block = (unsigned int)searchBitmap2(0);
  // unsigned int empty_sector = empty_block * 4;
  unsigned int MFT_sec = 6; // stars with root register, which is the register 1
  struct t2fs_4tupla emptyTupla;
  struct t2fs_record record;

  filenamecopy = strdup(filename);

  token = strtok(filenamecopy, "/");


  //Initializes the root MFT
  int current_dir_sector = 6;//ROOT_MFT;

  //isolated_filename is the filename without the subdirectories it is in
  char *isolated_filename = (strrchr(filename, '/'));
  isolated_filename = isolated_filename + 1;
  
  //Goes through the subdirectories
  while(strcmp(token,isolated_filename) != 0)
  {
    depht++;
    MFT_sec = (unsigned int)get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
  	// MFT_num is wrong I think, double check it
  	// well, MFT number is sector in this functions
  }

  // *emptyTupla = malloc(sizeof(struct t2fs_4tupla));
  emptyTupla.atributeType = 1;
  emptyTupla.virtualBlockNumber = 0;
  emptyTupla.logicalBlockNumber = empty_block;
  emptyTupla.numberOfContiguosBlocks = 1;

  // *record = malloc(sizeof(struct t2fs_record));
  record.TypeVal = 0x01;
  for(int i; i<MAX_FILE_NAME_SIZE; i++)
  	record.name[i] = 0x00;
  memcpy(record.name, isolated_filename, strlen(token)+1); // copying filename to record
  record.blocksFileSize = 1;
  record.bytesFileSize = 0;
  
  // supondo que eu ja tenha armazenado as info's daonde eu quero armazenar
  // MFT onde eu vou colocar as coisas novas
  // adicionar um record dentro desse diretorio com as infos
  // achar no BD uma posi'cao livre pra armazenar as info's
  // jogar isso pra opened files (eu acho)
  if(depht == 0) // if it is in ROOT
  {
  	empty_MFTRegister = find_empty_MFT_reg();
  	byte_position = find_empty_record_info(0x802);
	byte_sector = take_sector_from_position(byte_position);
  	byte_record_pos = take_right_position(byte_position);

  	record.MFTNumber = empty_MFTRegister;

  	empty_MFT_sector = boot_block.blockSize + 2*empty_MFTRegister;
  	// record.MFTNumber = empty_MFT_sector;

  	// write tuple in the MFT
  	if(write_first_tuple_MFT_and_set_0_second(empty_MFT_sector, emptyTupla) != 1)
  		return -1;

  	if(clear_block(empty_block) != 1)
  		return -1;

  	if(setBitmap2(empty_block, 1))
  		return -1;
    
    if(write_record_in_dir(ROOT_BD_SECTOR + byte_sector, byte_record_pos, record) != 1)
    	return -1;

   //  printf("\nTupla LBN: %u", emptyTupla.logicalBlockNumber);
  	// printf("\nEmpty MFT Register: %u", empty_MFTRegister);
  	// printf("\nByte Position: %u", byte_position);
  	// printf("\nByte Sector: %u", byte_sector);
  	// printf("\nByte record pos: %u", byte_record_pos);
  	// printf("\nempty MFT sector: %u", empty_MFT_sector);
  	// printf("\nRoot_Sector + byte sector = %u", (ROOT_BD_SECTOR + byte_sector));
  	// printf("\nrecord name: %s", record.name);
	// 
  	// printf("\n");

  }
  // if it is not a root directory
  else{
  	// 
  	struct t2fs_4tupla lastTuple = find_last_tuple_MFT_register(MFT_sec);
  	
  	empty_MFTRegister = find_empty_MFT_reg();
  	byte_position = find_empty_record_info(lastTuple.virtualBlockNumber);
	byte_sector = take_sector_from_position(byte_position);
  	byte_record_pos = take_right_position(byte_position);

  	record.MFTNumber = empty_MFTRegister;

  	empty_MFT_sector = boot_block.blockSize + 2*empty_MFTRegister;
  	// record.MFTNumber = empty_MFT_sector;

  	// write tuple in the MFT
  	if(write_first_tuple_MFT_and_set_0_second(empty_MFT_sector, emptyTupla) != 1)
  		return -1;

  	if(clear_block(empty_block) != 1)
  		return -1;

  	if(setBitmap2(empty_block, 1))
  		return -1;
    
    if(write_record_in_dir(ROOT_BD_SECTOR + byte_sector, byte_record_pos, record) != 1)
    	return -1;

  }


  // handle = open2(filename);
  handle = 0;
  return handle;
}

// Arateus
int mkdir2 (char *pathname)
{
  // int handle;
  char *token, *filenamecopy;
  int depht = 0; // 0 - is root
  unsigned int empty_MFTRegister;
  unsigned int empty_MFT_sector;
  unsigned int byte_position;
  unsigned int byte_sector;
  unsigned int byte_record_pos;
  unsigned int empty_block = (unsigned int)searchBitmap2(0);
  // unsigned int empty_sector = empty_block * 4;
  unsigned int MFT_sec = 6; // stars with root register, which is the register 1
  struct t2fs_4tupla emptyTupla;
  struct t2fs_record record;

  filenamecopy = strdup(pathname);

  token = strtok(filenamecopy, "/");


  //Initializes the root MFT
  int current_dir_sector = 6;//ROOT_MFT;

  //isolated_filename is the pathname without the subdirectories it is in
  char *isolated_filename = (strrchr(pathname, '/'));
  isolated_filename = isolated_filename + 1;
  
  //Goes through the subdirectories
  while(strcmp(token,isolated_filename) != 0){
    depht++;
    MFT_sec = (unsigned int)get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
  	// MFT_num is wrong I think, double check it
  	// well, MFT number is sector in this functions
  }

  // *emptyTupla = malloc(sizeof(struct t2fs_4tupla));
  emptyTupla.atributeType = 1;
  emptyTupla.virtualBlockNumber = 0;
  emptyTupla.logicalBlockNumber = empty_block;
  emptyTupla.numberOfContiguosBlocks = 1;

  // *record = malloc(sizeof(struct t2fs_record));
  // create directory record
  record.TypeVal = 0x02;
  for(int i; i<MAX_FILE_NAME_SIZE; i++)
  	record.name[i] = 0x00;
  memcpy(record.name, isolated_filename, strlen(token)+1); // copying pathname to record
  record.blocksFileSize = 1;
  record.bytesFileSize = 0;
  
  // supondo que eu ja tenha armazenado as info's daonde eu quero armazenar
  // MFT onde eu vou colocar as coisas novas
  // adicionar um record dentro desse diretorio com as infos
  // achar no BD uma posi'cao livre pra armazenar as info's
  // jogar isso pra opened files (eu acho)
  if(depht == 0) // if it is in ROOT
  {
  	empty_MFTRegister = find_empty_MFT_reg();
  	byte_position = find_empty_record_info(0x802);
	byte_sector = take_sector_from_position(byte_position);
  	byte_record_pos = take_right_position(byte_position);

  	record.MFTNumber = empty_MFTRegister;

  	empty_MFT_sector = boot_block.blockSize + 2*empty_MFTRegister;
  	// record.MFTNumber = empty_MFT_sector;

  	// write tuple in the MFT
  	if(write_first_tuple_MFT_and_set_0_second(empty_MFT_sector, emptyTupla) != 1)
  		return -1;

  	if(clear_block(empty_block) != 1)
  		return -1;

  	if(setBitmap2(empty_block, 1))
  		return -1;
    
    if(write_record_in_dir(ROOT_BD_SECTOR + byte_sector, byte_record_pos, record) != 1)
    	return -1;

   //  printf("\nTupla LBN: %u", emptyTupla.logicalBlockNumber);
  	// printf("\nEmpty MFT Register: %u", empty_MFTRegister);
  	// printf("\nByte Position: %u", byte_position);
  	// printf("\nByte Sector: %u", byte_sector);
  	// printf("\nByte record pos: %u", byte_record_pos);
  	// printf("\nempty MFT sector: %u", empty_MFT_sector);
  	// printf("\nRoot_Sector + byte sector = %u", (ROOT_BD_SECTOR + byte_sector));
  	// printf("\nrecord name: %s", record.name);
	// 
  	// printf("\n");

  }
  // if it is not a root directory
  else{
  	// 
  	struct t2fs_4tupla lastTuple = find_last_tuple_MFT_register(MFT_sec);
  	
  	empty_MFTRegister = find_empty_MFT_reg();
  	byte_position = find_empty_record_info(lastTuple.virtualBlockNumber);
	  byte_sector = take_sector_from_position(byte_position);
  	byte_record_pos = take_right_position(byte_position);

  	record.MFTNumber = empty_MFTRegister;

  	empty_MFT_sector = boot_block.blockSize + 2*empty_MFTRegister;
  	// record.MFTNumber = empty_MFT_sector;

  	// write tuple in the MFT
  	if(write_first_tuple_MFT_and_set_0_second(empty_MFT_sector, emptyTupla) != 1)
  		return -1;

  	if(clear_block(empty_block) != 1)
  		return -1;

  	if(setBitmap2(empty_block, 1))
  		return -1;
    
    if(write_record_in_dir(ROOT_BD_SECTOR + byte_sector, byte_record_pos, record) != 1)
    	return -1;

  }

	return 0; // if it worked :)
}