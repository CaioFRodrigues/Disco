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
#include <math.h>
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/bitmap2.h"
#include "../include/files.h"
#include "../include/MFT.h"
#include "../include/MFT_list.h"

//Arateus
int identify2(char *name, int size){
	char devs[] = {"Ana Paula Mello 260723\nArateus Meneses 242260\nCaio Rodrigues 261578\0"};
    name[size] = '\0';
    if(strncpy(name, devs, size))
        return 0;
    else
        return -1;
}

//Ana
int close2(FILE2 handle){

    if (opened_files[handle].is_valid == 1 && handle < 20){
        opened_files[handle].is_valid = 0;
        return 0;
    }
    return -1;
}

//Ana
int closedir2(DIR2 handle){

    if (opened_directories[handle].is_valid == 1 && handle < number_dir_handles){
        opened_directories[handle].is_valid = 0;
        return 0;
    }
    return -1;
}

//Ana
int seek2(FILE2 handle, DWORD offset){

  // Error cases
  if (!opened_files[handle].is_valid)
    return -1;

  if (offset > opened_files[handle].fileSizeBytes)
    return -1;

  if (offset != -1){  // Offset always counted from the start of the file (current_pointer = 0) 
    opened_files[handle].current_pointer = offset;
  }
  else{
    // current_pointer must go to the byte right after the end of the file (0 to fileSizeBytes -1)
    opened_files[handle].current_pointer = opened_files[handle].fileSizeBytes;
  }

  return 0;
}

// Ana
int read2(FILE2 handle, char * buffer, int size){

  int i =0;

  // MFT* mft = read_MFT(opened_files[handle].first_MFT_tuple);
  MFT* mft = read_MFT(12);  // For testing only

  unsigned char sector_buffer[SECTOR_SIZE]; // Buffer used to read from sector
  buffer[size] = '\0';

  // int current_pointer = opened_files[handle].current_pointer
  int current_pointer = 0;   // For testing only

  int bytes_left_to_copy = size;

  //   If the number of bytes to be read is larger than the amount of bytes 
  // from the current_pointer to the end of file, make sure that function reads only till end of file
  if (size + current_pointer > opened_files[handle].fileSizeBytes)
    size = opened_files[handle].fileSizeBytes - current_pointer;

  int current_pointer_block = current_pointer/1024; // Virtual block in which current_pointer is in
  int current_pointer_sector = current_pointer/256; //Logical sector in which first byte is in

  // Reads from file until there are no more mft tuples or there are no more bytes left to copy
  while (mft != NULL && bytes_left_to_copy > 0){

    // MFT info for blocks and sectors to be read
    int mft_first_logical_block = mft->current_MFT.logicalBlockNumber;
    int mft_number_blocks = mft->current_MFT.numberOfContiguosBlocks;
    int mft_number_sectors = mft_number_blocks * 4;
    int mft_first_sector = mft_first_logical_block * 4;

    int sector_starting_byte = 0; // Byte in which the reading from the sector must start

    int mft_virtual_blocks_passed = mft->current_MFT.virtualBlockNumber;
    int mft_first_virtual_sector = 4 * mft_virtual_blocks_passed;

    // If the current_pointer byte is in a block that this mft tuple maps or in a tuple previous to this one
    if (current_pointer_block <= mft_virtual_blocks_passed){

      // Iterate through the contiguos sectors mapped by the mft tuple
      for (i=0; i<mft_number_sectors; i++){

        sector_starting_byte = 0; // Reading starts at byte 0 by default

        // If current_pointer is in this sector or a previous one
        if (current_pointer_sector <= i+mft_first_virtual_sector){

          // Is current_pointer is in this specific sector
          if (current_pointer_sector == i+mft_first_virtual_sector){
            sector_starting_byte = current_pointer - (256*current_pointer_sector);
          }

          read_sector(i+mft_first_sector, sector_buffer);

          if (bytes_left_to_copy + sector_starting_byte >= 256){ // If the amount of bytes to read is bigger than the amount of bytes in the sector
            
            read_bytes(sector_starting_byte, 256, 
                          256-sector_starting_byte, (char *) sector_buffer, (char *) buffer);
                          
            bytes_left_to_copy -= (256 - sector_starting_byte);

          }
          else{ // If all bytes to be read are in this sector
            
            read_bytes(sector_starting_byte, bytes_left_to_copy+sector_starting_byte, 
                          bytes_left_to_copy, (char *) sector_buffer, (char *) buffer);

            opened_files[handle].current_pointer = current_pointer + size + 1; // Moves current_pointer to the next byte from where reading was finished
            
            return size;  // Function ended correctly
          }
        }
      }
    }
    mft = mft->next;
  }
  // Function couldn't read everything
  return -1;
}

//Caio
//Open2: Searches the filename on the tree of directories, then returns allocates the handler to the array of opened files
FILE2 open2 (char *filename){



	int director_sector = get_parent_dir_MFT_sector(filename);

	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));
  isolated_filename = isolated_filename + 1;
	int file_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, director_sector, SEARCHING_DIRECTORY);
	


	int file_mft_sector = (file_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(filename);

	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.first_MFT_tuple = file_mft_sector;

	//Declares the file as valid
	file_descriptor.is_valid = 1;
	

	int i = first_free_file_position();
	opened_files[i] = file_descriptor;
	printf ("o2asreasrei\n\n");
	return 0;




}


DIR2 opendir(char *filename){

	int director_sector = get_parent_dir_MFT_sector(filename);

	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));
  isolated_filename = isolated_filename + 1;
	int dir_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, director_sector, SEARCHING_DIRECTORY);

	int dir_mft_sector = (dir_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(filename);


	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.first_MFT_tuple = dir_mft_sector;

	//Declares the file as valid
	file_descriptor.is_valid = 1;

	int index = first_free_dir_position();

	opened_directories[index] = file_descriptor;

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

  // filenamecopy = strdup(filename);
  // fatherRecord = strdup(filename);

  // token = strtok(filenamecopy, "/");
  // tokenRecord = strtok(fatherRecord, "/");

  //Initializes the root MFT
  // int current_dir_sector = 6;//ROOT_MFT;
  // int current_dir_sectorFather = 6;
  MFT_father = MFT_sec;

  //isolated_filename is the filename without the subdirectories it is in
  // char *isolated_filename = (strrchr(filename, '/'));
  // isolated_filename = isolated_filename + 1;
  
  //Goes through the subdirectories
  // while(strcmp(token,isolated_filename) != 0)
  // {
  //   // depht++;
  //   MFT_sec = (unsigned int)get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
  //   // probably returning MFTNumber, use this as a powerfull weapon
  //   if(strcmp(token,isolated_filename) != 0)
  //     break;
  //   MFT_father = (unsigned int)get_MFTnumber_of_file_with_directory_number(tokenRecord, current_dir_sectorFather, SEARCHING_DIRECTORY);
    
  // }
  MFT_father = get_parent_dir_MFT_sector(filename);

  //isolated_filename is the filename without the subdirectories it is in
  char *isolated_filename = (strrchr(filename, '/'));
  isolated_filename = isolated_filename + 1;

  unsigned int file_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, (int)MFT_father, SEARCHING_DIRECTORY);
  
  MFT_sec = (file_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


  if(MFT_sec == -1)
    return -1;

  filenamecopy = strdup(filename);
  fatherRecord = strdup(filename);

  token = strtok(filenamecopy, "/");
  tokenRecord = strtok(fatherRecord, "/");

  while(strcmp(token,isolated_filename) != 0)
  {
    token = strtok(filenamecopy, "/");
    if(strcmp(token,isolated_filename) == 0)
      break;
    tokenRecord = strtok(fatherRecord, "/");
  }

// MUDAR ISSO AQUI
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
  int lastTPosition;
  unsigned int lastTPositionSector;
  unsigned int writeBlock;

  // filenamecopy = strdup(filename);
  // fatherRecord = strdup(filename);

  // token = strtok(filenamecopy, "/");
  // tokenRecord = strtok(fatherRecord, "/");

  //Initializes the root MFT
  // int current_dir_sector = 6;//ROOT_MFT;
  // int current_dir_sectorFather = 6;
  MFT_father = MFT_sec;

  //isolated_filename is the filename without the subdirectories it is in
  // char *isolated_filename = (strrchr(filename, '/'));
  // isolated_filename = isolated_filename + 1;
  
  //Goes through the subdirectories
  // while(strcmp(token,isolated_filename) != 0)
  // {
  //   // depht++;
  //   MFT_sec = (unsigned int)get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
  //   // probably returning MFTNumber, use this as a powerfull weapon
  //   if(strcmp(token,isolated_filename) != 0)
  //     break;
  //   MFT_father = (unsigned int)get_MFTnumber_of_file_with_directory_number(tokenRecord, current_dir_sectorFather, SEARCHING_DIRECTORY);
    
  // }
  MFT_father = get_parent_dir_MFT_sector(pathname);

  //isolated_filename is the filename without the subdirectories it is in
  char *isolated_filename = (strrchr(pathname, '/'));
  isolated_filename = isolated_filename + 1;

  unsigned int file_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, (int)MFT_father, SEARCHING_DIRECTORY);
  
  MFT_sec = (file_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


  if(MFT_sec == -1)
    return -1;
  filenamecopy = strdup(pathname);
  fatherRecord = strdup(pathname);

  token = strtok(filenamecopy, "/");
  tokenRecord = strtok(fatherRecord, "/");

  while(strcmp(token,isolated_filename) != 0)
  {
    token = strtok(filenamecopy, "/");
    if(strcmp(token,isolated_filename) == 0)
      break;
    tokenRecord = strtok(fatherRecord, "/");
  }

// MUDAR ISSO AQUI
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
