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


FILE2 create2(char *filename)
{
  FILE2 handle = 0;
  char *token, *filenamecopy, *fatherRecord, *tokenRecord;
  unsigned int MFT_father = 0;
  // int depht = 0; // 0 - is root
  unsigned int empty_MFTRegister;
  unsigned int empty_MFT_sector;
  unsigned int byte_position;
  unsigned int byte_sector;
  unsigned int byte_record_pos;
  unsigned int empty_block;
  // unsigned int empty_sector = empty_block * 4;
  unsigned int MFT_sec = 6; // stars with root register, which is the register 1
  struct t2fs_4tupla emptyTupla;
  struct t2fs_record record;

  struct t2fs_4tupla lastT;
  int lastTPosition;
  unsigned int lastTPositionSector;
  unsigned int writeBlock;

  filenamecopy = strdup(filename);
  fatherRecord = strdup(filename);

  token = strtok(filenamecopy, "/");
  tokenRecord = strtok(fatherRecord, "/");

  //Initializes the root MFT
  int current_dir_sector = 6;//ROOT_MFT;
  int current_dir_sectorFather = 6;
  MFT_father = MFT_sec;

  //isolated_filename is the filename without the subdirectories it is in
  char *isolated_filename = (strrchr(filename, '/'));
  isolated_filename = isolated_filename + 1;
  
  //Goes through the subdirectories
  while(strcmp(token,isolated_filename) != 0)
  {
    // depht++;
    MFT_sec = (unsigned int)get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
    // probably returning MFTNumber, use this as a powerfull weapon
    if(strcmp(token,isolated_filename) != 0)
      break;
    MFT_father = (unsigned int)get_MFTnumber_of_file_with_directory_number(tokenRecord, current_dir_sectorFather, SEARCHING_DIRECTORY);
    
  }

  if(MFT_sec == -1)
    return -1;

  if(strcmp(tokenRecord, isolated_filename) != 0)
  {
    if(MFT_father != 0){
      if(find_record_and_add_byteRecord(MFT_father, tokenRecord) != 1)
        return -1;

    }

  }

  lastTPositionSector = MFT_sec;
  lastT = find_last_tuple_MFT_register(lastTPositionSector); // last tuple with usable info, not type 0
  lastTPosition = find_position_last_tuple_MFT_register(lastTPositionSector);
  
  // this shall go to the last tuple in the sequence of registers if necessary
  while(lastT.atributeType == 2)
  {
    lastTPositionSector = lastT.virtualBlockNumber*2 + 4;
    lastT = find_last_tuple_MFT_register(lastTPositionSector); // find last not 0 atributeType tuple
    lastTPosition = find_position_last_tuple_MFT_register(lastTPositionSector);
    
  }

  byte_position = find_empty_record_info(lastT.logicalBlockNumber, lastT.numberOfContiguosBlocks);
  writeBlock = lastT.logicalBlockNumber;
  if(byte_position == -1) // if didn't find an empty position (dir is full)
  {
    // search for the first empty block if contigous
    // int cont = 0;
    if(getBitmap2(lastT.logicalBlockNumber + 1) == 1) // if next position is allocated
    { // alloc new tuple
      struct t2fs_4tupla newTuple;
      newTuple.atributeType = 1;
      newTuple.logicalBlockNumber = (unsigned int)searchBitmap2(0);
      setBitmap2(newTuple.logicalBlockNumber, 1);
      newTuple.numberOfContiguosBlocks = 1;
      newTuple.virtualBlockNumber = lastT.virtualBlockNumber + lastT.virtualBlockNumber;

      if(write_on_last_tuple_MFT_and_set_0_second(lastTPositionSector, newTuple, lastTPosition+1) != 1) // write newTuple with new info in the next position
          return -1;
      writeBlock = newTuple.logicalBlockNumber;

    }
    else if(getBitmap2(lastT.logicalBlockNumber + 1) == 0)// if next block is empty
    {
      lastT.numberOfContiguosBlocks = lastT.numberOfContiguosBlocks + 1;
      setBitmap2(lastT.logicalBlockNumber + 1, 1);
      if(write_on_last_tuple_MFT_and_set_0_second(lastTPositionSector, lastT, lastTPosition) != 1) // sobrescribe lastTuple
        return -1;
      writeBlock = lastT.logicalBlockNumber + 1;
    }
    else 
      return -1;

    byte_position = 0;
    // byte_sector = 0;
    // byte_record_pos = 0;
  }
  // else
  // {
  //   byte_position = find_empty_record_info(lastT.logicalBlockNumber);
  //   byte_sector = take_sector_from_position(byte_position);
  //   byte_record_pos = take_right_position(byte_position);

  // }
  byte_sector = take_sector_from_position(byte_position);
  byte_record_pos = take_right_position(byte_position);

  empty_MFTRegister = find_empty_MFT_reg();
  record.MFTNumber = empty_MFTRegister;
  empty_MFT_sector = boot_block.blockSize + 2*empty_MFTRegister;

  record.TypeVal = 0x01;
  for(int i; i<MAX_FILE_NAME_SIZE; i++)
    record.name[i] = 0x00;
  memcpy(record.name, isolated_filename, strlen(isolated_filename)+1); // copying pathname to record
  record.blocksFileSize = 1;
  record.bytesFileSize = 0;

  emptyTupla.atributeType = 1;
  emptyTupla.virtualBlockNumber = 0;
  empty_block = (unsigned int)searchBitmap2(0);
  emptyTupla.logicalBlockNumber = empty_block;
  emptyTupla.numberOfContiguosBlocks = 1;


  // write tuple in the MFT
  if(write_first_tuple_MFT_and_set_0_second(empty_MFT_sector, emptyTupla) != 1)
    return -1;

  if(clear_block(empty_block) != 1)
    return -1;

  if(setBitmap2(empty_block, 1))
    return -1;
  
  if(write_record_in_dir(writeBlock*4 + byte_sector, byte_record_pos, record) != 1)
    return -1;



  return handle; // if it worked :)
}















// Arateus
int mkdir2(char *pathname)
{
  // FILE2 handle = 0;
  char *token, *filenamecopy, *fatherRecord, *tokenRecord;
  unsigned int MFT_father = 0;
  // int depht = 0; // 0 - is root
  unsigned int empty_MFTRegister;
  unsigned int empty_MFT_sector;
  unsigned int byte_position;
  unsigned int byte_sector;
  unsigned int byte_record_pos;
  unsigned int empty_block;
  // unsigned int empty_sector = empty_block * 4;
  unsigned int MFT_sec = 6; // stars with root register, which is the register 1
  struct t2fs_4tupla emptyTupla;
  struct t2fs_record record;

  struct t2fs_4tupla lastT;
   // fatherTuple;
  int lastTPosition;
  unsigned int lastTPositionSector;
  unsigned int writeBlock;

  filenamecopy = strdup(pathname);
  fatherRecord = strdup(pathname);

  token = strtok(filenamecopy, "/");
  tokenRecord = strtok(fatherRecord, "/");

  //Initializes the root MFT
  int current_dir_sector = 6;//ROOT_MFT;
  int current_dir_sectorFather = 6;
  MFT_father = MFT_sec;

  //isolated_filename is the pathname without the subdirectories it is in
  char *isolated_filename = (strrchr(pathname, '/'));
  isolated_filename = isolated_filename + 1;
  
  //Goes through the subdirectories
  while(strcmp(token,isolated_filename) != 0)
  {
    // depht++;
    // tokenRecord = strtok(fatherRecord, "/");
    // MFT_father = MFT_sec;
    MFT_sec = (unsigned int)get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
    // probably returning MFTNumber, use this as a powerfull weapon
    if(strcmp(token,isolated_filename) != 0)
      break;
    MFT_father = (unsigned int)get_MFTnumber_of_file_with_directory_number(tokenRecord, current_dir_sectorFather, SEARCHING_DIRECTORY);
    
  }

  if(MFT_sec == -1)
    return -1;

  // printf("\nMFT_father: %u\nMFT_sec: %u", MFT_father, MFT_sec);
  // printf("\ntoken: %s", token);
  // printf("\ntokenRecord: %s", tokenRecord);
  // here I need to take the record of the directory where the file will be added
  if(strcmp(tokenRecord, isolated_filename) != 0)
  {
    // printf("\nTO NA TV MAE");
    if(MFT_father != 0){
      if(find_record_and_add_byteRecord(MFT_father, tokenRecord) != 1)
        return -1;

    }

  }

  lastTPositionSector = MFT_sec;
  lastT = find_last_tuple_MFT_register(lastTPositionSector); // last tuple with usable info, not type 0
  lastTPosition = find_position_last_tuple_MFT_register(lastTPositionSector);
  
  // this shall go to the last tuple in the sequence of registers if necessary
  while(lastT.atributeType == 2)
  {
    lastTPositionSector = lastT.virtualBlockNumber*2 + 4;
    lastT = find_last_tuple_MFT_register(lastTPositionSector); // find last not 0 atributeType tuple
    lastTPosition = find_position_last_tuple_MFT_register(lastTPositionSector);
    
  }

  byte_position = find_empty_record_info(lastT.logicalBlockNumber, lastT.numberOfContiguosBlocks);
  writeBlock = lastT.logicalBlockNumber;
  if(byte_position == -1) // if didn't find an empty position (dir is full)
  {
    // search for the first empty block if contigous
    // int cont = 0;
    if(getBitmap2(lastT.logicalBlockNumber + 1) == 1) // if next position is allocated
    { // alloc new tuple
      struct t2fs_4tupla newTuple;
      newTuple.atributeType = 1;
      newTuple.logicalBlockNumber = (unsigned int)searchBitmap2(0);
      setBitmap2(newTuple.logicalBlockNumber, 1);
      newTuple.numberOfContiguosBlocks = 1;
      newTuple.virtualBlockNumber = lastT.virtualBlockNumber + lastT.virtualBlockNumber;

      if(write_on_last_tuple_MFT_and_set_0_second(lastTPositionSector, newTuple, lastTPosition+1) != 1) // write newTuple with new info in the next position
          return -1;
      writeBlock = newTuple.logicalBlockNumber;

    }
    else if(getBitmap2(lastT.logicalBlockNumber + 1) == 0)// if next block is empty
    {
      lastT.numberOfContiguosBlocks = lastT.numberOfContiguosBlocks + 1;
      setBitmap2(lastT.logicalBlockNumber + 1, 1);
      if(write_on_last_tuple_MFT_and_set_0_second(lastTPositionSector, lastT, lastTPosition) != 1) // sobrescribe lastTuple
        return -1;
      writeBlock = lastT.logicalBlockNumber + 1;
    }
    else 
      return -1;

    byte_position = 0;
    // byte_sector = 0;
    // byte_record_pos = 0;
  }
  // else
  // {
  //   byte_position = find_empty_record_info(lastT.logicalBlockNumber);
  //   byte_sector = take_sector_from_position(byte_position);
  //   byte_record_pos = take_right_position(byte_position);

  // }
  byte_sector = take_sector_from_position(byte_position);
  byte_record_pos = take_right_position(byte_position);

  empty_MFTRegister = find_empty_MFT_reg();
  record.MFTNumber = empty_MFTRegister;
  empty_MFT_sector = boot_block.blockSize + 2*empty_MFTRegister;

  record.TypeVal = 0x02;
  for(int i; i<MAX_FILE_NAME_SIZE; i++)
    record.name[i] = 0x00;
  memcpy(record.name, isolated_filename, strlen(isolated_filename)+1); // copying pathname to record
  record.blocksFileSize = 1;
  record.bytesFileSize = 0;

  emptyTupla.atributeType = 1;
  emptyTupla.virtualBlockNumber = 0;
  empty_block = (unsigned int)searchBitmap2(0);
  emptyTupla.logicalBlockNumber = empty_block;
  emptyTupla.numberOfContiguosBlocks = 1;


  // write tuple in the MFT
  if(write_first_tuple_MFT_and_set_0_second(empty_MFT_sector, emptyTupla) != 1)
    return -1;

  if(clear_block(empty_block) != 1)
    return -1;

  if(setBitmap2(empty_block, 1))
    return -1;
  
  if(write_record_in_dir(writeBlock*4 + byte_sector, byte_record_pos, record) != 1)
    return -1;



  return 0; // if it worked :)
}

// na read eu preciso atualizar a quantidade de bytes do record