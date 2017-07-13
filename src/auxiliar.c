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
#include "../include/bitmap2.h"
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
  boot_block.id[ID+4]='\0';

  boot_block.version = (buffer[VERSION+1] << 8) | buffer[VERSION];

  boot_block.blockSize = (buffer[BLOCKSIZE+1] << 8) | buffer[BLOCKSIZE];

  boot_block.MFTBlocksSize = (buffer[MFTBLOCKSSIZE+1] << 8) | buffer[MFTBLOCKSSIZE];

  boot_block.diskSectorSize = (buffer[DISKSECTORSIZE+3] << 16) | (buffer[DISKSECTORSIZE+2] << 12) | (buffer[DISKSECTORSIZE+1] << 8) | buffer[DISKSECTORSIZE];

  initialize_open_files();
  initialize_open_directories(); 
  return 0;
}

//Caio
//Initializes the array of opened files
void initialize_open_files(){

  for(int i = 0; i < MAX_OPENED_FILES; i++)
    opened_files[i].is_valid = 0;

}

void initialize_open_directories(){
  number_dir_handles = 0;
  opened_directories = NULL;
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

//Returns whether the file is open or not
int is_file_open(char * file_name){

  for (int i = 0; i < MAX_OPENED_FILES; i++){

    if(opened_files[i].is_valid){
      if (strcmp(file_name, opened_files[i].file_path) == 0){
        return 1;
      }
    }

  }
  
  return 0;
  
}

 //Caio
//Gets first possible position from opened_directories
//Returns -1 if it failed
int first_free_dir_position(){

  //If there is no current directory, return 0 and allocates space
  if (opened_directories == NULL){
    opened_directories = malloc(sizeof(FILE_DESCRIPTOR));
    return 0;
  }
  else{
    //Goes through the current handles, and tries to find a valid one
    for (int i = 0;i < number_dir_handles; i++){
      if (opened_directories[i].is_valid)
        return i;
    }

    //If there is none, it is needed to reallocate the size of opened_directories and return that new_position
    opened_directories = realloc(opened_directories, sizeof(opened_directories) + sizeof(FILE_DESCRIPTOR));
    number_dir_handles++;
    return number_dir_handles;
  }
  return -1;
}

int is_directory_open(char * file_name){

  for (int i = 0; i < number_dir_handles; i++){

    if(opened_directories[i].is_valid && (strcmp(file_name, opened_directories[i].file_path) == 0))
      return 1;

  
  }

  return 0;
  
}

DWORD virtual_block_to_logical_block(DWORD current_pointer, MFT* mft_list){

  MFT* mft_list_copy = mft_list;  
  DWORD currentVirtualBlockNumber, numberOfContiguosBlocks;

  while (mft_list_copy != NULL){
      
    currentVirtualBlockNumber = mft_list_copy->current_MFT.virtualBlockNumber;
    numberOfContiguosBlocks = mft_list_copy->current_MFT.numberOfContiguosBlocks;
    
    if (numberOfContiguosBlocks + currentVirtualBlockNumber - 1 < currentVirtualBlockNumber){ // If current_virtual_block is not mapped in this tuple
      mft_list_copy = mft_list_copy->next;
    }
    else{
      // currentVirtualBlockNumber maps to logicalBlockNumber, current_virtual_block = logicalBlockNumber + offset
      return mft_list_copy->current_MFT.logicalBlockNumber + (currentVirtualBlockNumber - currentVirtualBlockNumber);
    }
  }

  return -1;

}

// Ana
//  Given a byte offset and an mft list, finds the byte offset relative to the 
//logical sector number in which the wanted byte will be
//  Example: bytes = 500
//  The 500th byte will be in the file's second virtualBlock, and it will
//be the 12th byte from the beginning of the equivalent logical sector
int find_byte_position_in_logical_sector(MFT* mft, int bytes){
  
  int num_sectors, num_blocks, offsets_bytes;

  num_sectors = ceil(bytes/256.0);
  num_blocks = ceil(bytes/1024.0);

  offsets_bytes = bytes - (num_blocks * num_sectors * 256);
  
  return offsets_bytes;

}

// Ana
char* append_buffers( char* final_buffer, char* temp_buffer ){
  while (*final_buffer)
    final_buffer++;
  
  while ((*final_buffer++ = *temp_buffer++));

  return --final_buffer;
}

// Ana
/* Given the starting and ending byte, the amount of bytes to copy, the source and the destination buffer,
copies the specific bytes in the specific amounts to the destination buffer
*/
void read_bytes(int starting_byte, int ending_byte, int bytes_to_copy, char* source, char* destination){

  char temp_buffer[bytes_to_copy+1]; // Temporary buffer that stores bytes to be appended to buffer

  int k = 0, j = 0;

  // Copy wanted bytes only
  for (j=starting_byte; j<ending_byte; j++){
    temp_buffer[k] = source[j];
    k++;
  }
  temp_buffer[k] = '\0';

  destination = append_buffers(destination, temp_buffer);


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

// Ana
// Given a buffer with content to be inserted in a sector,
// inserts it in the given place and writes it to the sector
int insert_in_sector(int sector, char* content, int start, int size){

    unsigned char sector_content[SECTOR_SIZE];
// Buffer that will recieve the sliced buffer content

    int i = 0, k = 0;
    read_sector(sector, sector_content);

    k = start;
    for (i=0; i<size; i++){
        sector_content[k] = content[i];
        k++;
    }

    write_sector(sector, sector_content);

    return 0;
}


// Ana
/* Given the amount of blocks needed, a pointer to the mft list and a pointer to its last node
alocates the blocks in the disk, creating/updating the MFT tuples to map the virtual blocks
to the logical ones
*/
int alocate_needed_blocks(int blocks_needed, MFT* mft, MFT* last_mft){

  int n = 0;

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
      printf("***%d***", last_mft->sector);

      // Treat situation in which sector and/or register is full
      write_first_tuple_MFT_and_set_0_second(last_mft->sector, (last_mft->offset+1)*16, new_tuple);
      mft = push_MFT(mft, new_tuple, last_mft->sector, (last_mft->offset)+1);
      last_mft = last_mft->next;

    }
  }

  return 0;

}

//Arateus
// Find the first empty register in the MFT
// return first empty register in the MFT
// if error occurs or is full return -1
int find_empty_MFT_reg()
{
  unsigned char buffer[SECTOR_SIZE];

  int full_MFT = -1;
  int registro = 4; // where first enabled MFT register starts (not protected by the system)

  unsigned int SIZE_of_MFT_setores = ((unsigned int)boot_block.blockSize) * ((unsigned int)boot_block.MFTBlocksSize);

  // first empty MFT register must be at block 4
  unsigned int first_MFT_sector = (unsigned int)boot_block.blockSize;
  unsigned int first_MFT_usable_sector = first_MFT_sector + SIZE_of_MFT_REGISTER_sector * 4; // reg0 = Bitmap; reg1 = root; reg2 and reg3 = reserved

  // this shall get every register in the MFT
  unsigned int i;
  for(i = first_MFT_usable_sector; i <= SIZE_of_MFT_setores; i = i + SIZE_of_MFT_REGISTER_sector){
    int error = read_sector(i, buffer);
    if(error){
      printf("\nError at find_empty_MFT_reg, i: %i, error: %d\n", i, error);
      return -1;
    }


    DWORD realAtrType = conv_string_to_hex(buffer, ATRIB_TYPE, 4);

    // printf("atrType: %#X\n", realAtrType); 

    if((int)realAtrType != -1){
      registro++;
      continue;
    }
    else{
      return registro; //address

    }

  }

  return full_MFT;
}

//Arateus
// unsigned int take_sector_from_empty_record_info(unsigned int record_position)
// {
//   // unsigned int sector = record_position / SIZE_of_RECORD;
//   // return sector;
//   return record_position / SIZE_of_RECORD;
// }
//Arateus
// unsigned int take_record_position_in_dir(unsigned int record_position)
// {
//   return record_position % SIZE_of_RECORD;
// }

//Arateus
// Take the right sector of the record position auxiliary function
// example: if position is 320 (byte position), it will give you
// the real sector of the record
unsigned int take_sector_from_position(unsigned int record_position)
{
  return record_position / SECTOR_SIZE;
}
//Arateus
// Take the right byte position to insert the record in the right sector
// example: if position is 320 (byte position), it will result 64,
// which is the right position in the sector to read or write the record
unsigned int take_right_position(unsigned int record_position)
{
  return record_position % SECTOR_SIZE;
}

// Arateus
// find the first empty record space in bytes at the BD (the name of this function is not quite good)
// VBN: Virtual Block Number
// return byte_position or -1 if something went wrong or is full
unsigned int find_empty_record_info(unsigned int lbn, unsigned int contigBlock)
{
  unsigned char buffer[SECTOR_SIZE];
  unsigned int sector = (lbn * (unsigned int)boot_block.blockSize); // convert block into sector
  unsigned int totalSize = contigBlock * 4;

  BYTE type;

  int error;
  int cont = 0;

  int k;
  for(k = 0; k < totalSize; k++) // 'till the end of the block
  {
    error = read_sector(sector+k, buffer);
    if(error)
      return -1;

    int b;
    for (b = 0; b < SECTOR_SIZE; b = b + SIZE_of_RECORD)
    {
      type = buffer[b]; // taking type
      if(type == 0x0){
        cont = cont + b;
        return cont; // return the position of the record in bytes
      }
      
    }
    cont = cont + SECTOR_SIZE;
  }

  return -1;
}

//Arateus
// Write a record in a directory
// given a sector and the byte position to insert the record, insert it in the desired position
// return 1 if it worked and -1 if something went wrong
int write_record_in_dir(unsigned int sector, unsigned int byte_pos, struct t2fs_record record)
{
  // unsigned int byte_sector = take_sector_from_empty_record_info(byte_pos);
  // unsigned int byte_record_pos = take_record_position_in_dir(byte_pos);

  unsigned char buffer[SECTOR_SIZE];

  int error = read_sector(sector, buffer);
  if(error)
    return -1;

  // write TypeVal
  buffer[byte_pos] = record.TypeVal;
  // write name
  int i;
  for (i = 0; i < MAX_FILE_NAME_SIZE; i++)
  {
    buffer[byte_pos+RECORD_NAME+i] = record.name[i];
  }

  unsigned int aux;
  // write in buffer the blocksFileSize
  for (i = 0; i < 4; i++)
  {
    aux = (record.blocksFileSize >> 8*i)&0xff;
    buffer[byte_pos+RECORD_BLOCK_FILESIZE+i] = aux;
  }
  //write in buffer for the bytesFileSize
  for (i = 0; i < 4; i++)
  {
    aux = (record.bytesFileSize >> 8*i)&0xff;
    buffer[byte_pos+RECORD_BYTES_FILESIZE+i] = aux;
  }
  // write for the MFTNumber
  for (i = 0; i < 4; i++)
  {
    aux = (record.MFTNumber >> 8*i)&0xff;
    buffer[byte_pos+RECORD_MFTNUMBER+i] = aux;
  }

  // write buffer in sector
  int write_error = write_sector(sector, buffer);
  if(write_error)
    return -1;

  return 1;
}


int write_tuple(unsigned int sector, struct t2fs_4tupla t, unsigned int offset)
{
  unsigned char buffer[SECTOR_SIZE];
  int error = read_sector(sector, buffer);
  if(error)
    return -1;
  unsigned int aux;
  int i;
  for (i = 0; i < 4; i++)
  {
    aux = (t.atributeType >> 8*i)&0xff;
    buffer[TUPLE_ATRTYPE+i+offset] = aux;
  }
  // write virtualBlockNumber
  for (i = 0; i < 4; i++)
  {
    aux = (t.virtualBlockNumber >> 8*i)&0xff;
    buffer[TUPLE_VBN+i+offset] = aux;
  }
  // write logicalBlockNumber
  for (i = 0; i < 4; i++)
  {
    aux = (t.logicalBlockNumber >> 8*i)&0xff;
    buffer[TUPLE_LBN+i+offset] = aux;
  }
  // write numberOfContiguousBlocks
  for (i = 0; i < 4; i++)
  {
    aux = (t.numberOfContiguosBlocks >> 8*i)&0xff;
    buffer[TUPLE_NUMCONTIGBLOCK+i+offset] = aux;
  }
  int write_error = write_sector(sector, buffer);
  if(write_error)
    return -1;
  return 1;
}

int write_on_last_tuple_MFT_and_set_0_second(unsigned int sector, struct t2fs_4tupla t, unsigned int tupleNum)
{
  unsigned char buffer[SECTOR_SIZE];
  unsigned int realSector;
  struct t2fs_4tupla newMFTRegister;
  int error;
  unsigned int aux;
  unsigned int zero = 0x00;
  unsigned int offset = tupleNum * 16;

  if(tupleNum >= 16 && tupleNum <= 31)
    realSector = sector + 1;
  else
    realSector = sector;

  if(tupleNum == 31) // last tuple
  {
    // alloc new MFT Register
    int newRegister = find_empty_MFT_reg();
    int newRegisterSector = newRegister*2 + 4;

    newMFTRegister.atributeType = 2;
    newMFTRegister.virtualBlockNumber = newRegister;
    newMFTRegister.logicalBlockNumber = -1;
    newMFTRegister.numberOfContiguosBlocks = -1;

    // writing in the last position the pointer MFT Tuple
    int w = write_tuple(realSector, newMFTRegister, offset);
    if(w)
      return -1;

    offset = 0; // write on the first tuple
    realSector = newRegisterSector;

  }
  error = read_sector(realSector, buffer);
  if(error)
    return -1;
  
  // write AtributeType in the first tuple in the MFT
  int i;
  for (i = 0; i < 4; i++)
  {
    aux = (t.atributeType >> 8*i)&0xff;
    buffer[TUPLE_ATRTYPE+i+offset] = aux;
  }
  // write virtualBlockNumber
  for (i = 0; i < 4; i++)
  {
    aux = (t.virtualBlockNumber >> 8*i)&0xff;
    buffer[TUPLE_VBN+i+offset] = aux;
  }
  // write logicalBlockNumber
  for (i = 0; i < 4; i++)
  {
    aux = (t.logicalBlockNumber >> 8*i)&0xff;
    buffer[TUPLE_LBN+i+offset] = aux;
  }
  // write numberOfContiguousBlocks
  for (i = 0; i < 4; i++)
  {
    aux = (t.numberOfContiguosBlocks >> 8*i)&0xff;
    buffer[TUPLE_NUMCONTIGBLOCK+i+offset] = aux;
  }
  // write 0 in the second tuple
  for (i = 0; i < 4; i++)
  {
    buffer[TUPLE_ATRTYPE+16+i+offset] = zero;
  }

  int write_error = write_sector(realSector, buffer);
  if(write_error)
    return -1;

  return 1;
}

// Arateus
// aux function for clear block
// initializates alocated sector with 0x00
// return 1 if work and -1 if had a write error
int clear_sector(unsigned int sector){
  unsigned char buffer[SECTOR_SIZE];
  
  int i;
  for (i = 0; i < SECTOR_SIZE; i++)
  {
    buffer[i] = 0x0;
  }

  int write_error = write_sector(sector, buffer);
  if(write_error)
    return -1;

  return 1;
}

//Arateus
// clears the whole block
// put 0x00 in every single byte in the block
// return 1 if work and -1 if had a write error
int clear_block(int init_sector){
  // unsigned char buffer[SECTOR_SIZE];
  int i;
  for (i = 0; i < 4; i++)
  {
    if(clear_sector(init_sector+i) == -1)
      return -1;
  }

  return 1;
}

//read dir

// percorrer a MFT


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

// funcao q vai receber um nome e um diretorio sector e acha o record com nome correspondente
// fill_derectory ^^^


// check if penultima posicao esta livre
// int check_if_penultimate_Tuple_empty()

int find_record_and_add_byteRecord(unsigned int sector, char *name)
{
  unsigned char bufferMFT[SECTOR_SIZE];
  unsigned char bufferBD[SECTOR_SIZE];
  struct t2fs_4tupla t;
  struct t2fs_record record;
  int error;
  unsigned int s;
  unsigned int MFT_sec = sector;
  int g;

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
        return -1;
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
              return -1;
            }
            for (r = 0; r < 4; r++)
            { //each sector has max 4 records

              record = fill_directory(bufferBD, r);
              for(g = (int)strlen(record.name) + 1; g< MAX_FILE_NAME_SIZE; g++)
                record.name[g] = 0x00;
              if(strcmp(name, record.name)==0){
                record.bytesFileSize = record.bytesFileSize + 64;
                int directory_start =  r * RECORD_SIZE;
                printf("\nrecord.bytesFileSize: %u", record.bytesFileSize);

                error = write_record_in_dir(s+p, directory_start, record);
                if(error)
                  return -1;
                return 1;
                
              }
            }
          }
        }
      }

    }
  }



  return -1;
}


int find_father_record_and_add_byteRecord(char *filename, char *name)
{
  unsigned int MFT_sec = 6;

  char *filenamecopy = strdup(filename);
  char *token;
  token = strtok(filenamecopy, "/");
  MFT_sec = search_record_in_dir_and_add(MFT_sec, token);

  while(strcmp(token, name) != 0)
  {
    token = strtok(NULL, "/");
    // printf("\ntoloko: %s",token);
    // printf("\nname: %s", name);
    // printf("\nmft secs: %u", MFT_sec);
    MFT_sec = search_record_in_dir_and_add(MFT_sec, token); // MFT_of this directory
    if(MFT_sec == -1){
      // printf("\npq ta dando essa bagaca?");
      return -1;
    }
    // printf("\nmft secs depois: %u", MFT_sec);
    
  }
  // if(MFT_sec == -1)
  //   return -1;

  return 1;
}

// return sector of desired record_name
unsigned int search_record_in_dir_and_add(unsigned int sector, char *name)
{
  unsigned char bufferMFT[SECTOR_SIZE];
  unsigned char bufferBD[SECTOR_SIZE];
  struct t2fs_4tupla t;
  struct t2fs_record record;
  int error;
  unsigned int s;
  unsigned int MFT_sec = sector;
  // search at MFT for desired info
  int i;
  int j;
  int k;
  int p;
  int r;
  int g;
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
        return -1;
      }
      else if(t.atributeType == 2){ // if reached the end of the MFT register (the last tuple) go to the next register
        i = 0;
        MFT_sec = t.virtualBlockNumber * 2 + 4; // recebe o registro e o converte pro setor desse registro
        break;
      }
        
      else{
        for(k = 0; k < t.numberOfContiguosBlocks; k++)
        { // reading BD
          s = (unsigned int)((t.logicalBlockNumber + k) * 4); // find the sector of the respective blocks
          for(p = 0; p<4; p++) // read the whole block
          {
            error = read_sector(s + p, bufferBD); // reading BD register
            if(error){
              return -1;
            }
            for (r = 0; r < 4; r++)
            { //each sector has max 4 records

              record = fill_directory(bufferBD, r);
              for(g = (int)strlen(record.name) + 1; g< MAX_FILE_NAME_SIZE; g++)
                record.name[g] = 0x00;
              if(strcmp(name, record.name)==0){
                record.bytesFileSize = record.bytesFileSize + 64;
                int directory_start =  r * RECORD_SIZE;

                if(write_record_in_dir(s+p, directory_start, record) != 1)
                  return -1;

                return record.MFTNumber*2 + 4;
                
              }
            }
          }
        }
      }

    }
  }


  return -1;

}

int check_recordPosition_valid(unsigned int record_position, unsigned int writeBlock, struct t2fs_4tupla lastT, unsigned int lastTPositionSector, unsigned int lastTPosition)
{
  unsigned int wBlock = writeBlock;
  if(record_position == -1) // if didn't find an empty position -> dir is full
  {
    // search for the first empty block if contigous
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
        wBlock = newTuple.logicalBlockNumber;

      }
      else if(getBitmap2(lastT.logicalBlockNumber + 1) == 0)// if next block is empty
      {
        lastT.numberOfContiguosBlocks = lastT.numberOfContiguosBlocks + 1;
        setBitmap2(lastT.logicalBlockNumber + 1, 1);
        if(write_on_last_tuple_MFT_and_set_0_second(lastTPositionSector, lastT, lastTPosition) != 1) // sobrescribe lastTuple
          return -1;
        wBlock = lastT.logicalBlockNumber + 1;
      }
      else 
        return -1;

  }

  return wBlock;
}

// write and set the new sector in the BD
// if it works returns 1, else return error value = -1
int write_new_arq(unsigned int record_position, char *isolated_filename, unsigned int writeBlock, DWORD type)
{
  unsigned int empty_record_sector_aux = take_sector_from_position(record_position);
  unsigned int empty_record_pos_aux = take_right_position(record_position);
  unsigned int empty_MFTRegister = (unsigned int)find_empty_MFT_reg();
  unsigned int empty_MFTsector = 4 + 2*empty_MFTRegister;
  
  struct t2fs_record record;
  record.TypeVal = type;
  record.MFTNumber = empty_MFTRegister;
  for(int i; i<MAX_FILE_NAME_SIZE; i++)
    record.name[i] = 0x00;
  memcpy(record.name, isolated_filename, strlen(isolated_filename)+1); // copying pathname to record
  record.blocksFileSize = 1;
  record.bytesFileSize = 0;

  struct t2fs_4tupla emptyTupla;
  emptyTupla.atributeType = 1;
  emptyTupla.virtualBlockNumber = 0;
  emptyTupla.numberOfContiguosBlocks = 1;
  unsigned int empty_block = (unsigned int)searchBitmap2(0);
  emptyTupla.logicalBlockNumber = empty_block;
  
  if(setBitmap2(empty_block, 1))
    return -1;

  if(clear_block(empty_block) != 1)
    return -1;

  //write empty tuple in the MFT
  if(write_first_tuple_MFT_and_set_0_second(empty_MFTsector, 0, emptyTupla) != 1)
    return -1;
  
  //write the record in the dir
  if(write_record_in_dir(writeBlock*4 + empty_record_sector_aux, empty_record_pos_aux, record) != 1)
    return -1;


  return 1;

}

int generic_create(char *filename, DWORD type)
{
  char *token, *filenamecopy, *fatherRecord, *tokenRecord;

  // unsigned int MFT_fatherDirSector = 6; // initial father sector is root sector

  char *copyName = strdup(filename);
  char *isol_fn = (strrchr(copyName, '/'));
  char *isolated_filename = strtok(isol_fn, "/");
  // isolated_filename = isolated_filename + 1;

  filenamecopy = strdup(filename);
  fatherRecord = strdup(filename);

  // tokenRecord = strtok(fatherRecord, "/");

  unsigned int MFT_father = (unsigned int)get_parent_dir_MFT_sector(filename);
  if(MFT_father == 4)
    return -1; // wrong path

  // printf("\nMFT_father: %u\n", MFT_father);

  token = strtok(filenamecopy, "/");
  int cont = -1;
  while(strcmp(token, isolated_filename) != 0)
  {
    // printf("\nToken: %s", token);
    // printf("\ntokenRecord: %s", tokenRecord);
    token = strtok(NULL, "/");
    cont++;
    // if(strcmp(token,isolated_filename) == 0)
    //   break;
    // tokenRecord = strtok(fatherRecord, "/");
  }
  // printf("\nchegou aqui :O");

  tokenRecord = strtok(fatherRecord, "/");
  for (int o = 0; o < cont; o++)
  {
    tokenRecord = strtok(NULL, "/");
  }
  // ATE AQUI TA OKAY

  // check if root
  // if tokenRecord == isolated filename, it means that is on Root dir
  // printf("\ntoken: %s\nisolated: %s",tokenRecord, isolated_filename);
  if(strcmp(tokenRecord, isolated_filename) != 0)
  {
    if(MFT_father >= 6){
      // this shall add 64bytes at the record father, cuz it's adding a new arq, that is 64 bytes
        if(find_father_record_and_add_byteRecord(filename, tokenRecord) != 1){
          // printf("\nTA CRASHANO AQUI");
            return -1;
        }
        
    }
    else
    {
      // printf("\nSE N AQUI AQUI");
      // if get_parent_dir_MFT_sector(filename) crashes
      return -1;
    }
  }

  // 
  // now gotta search for the last usable tuple to find the proper block to add info
  struct t2fs_4tupla lastT; // the last tuple of the MFT_

  unsigned int lastTPositionSector = MFT_father;
  lastT = find_last_tuple_MFT_register(lastTPositionSector); // last tuple with usable info, not type 0
  unsigned int lastTPosition = 0;
  // printf("\nlast T position A: %u", lastTPosition);
  lastTPosition = (unsigned int)find_position_last_tuple_MFT_register(lastTPositionSector);
  // printf("\nlast T position D: %u", lastTPosition);
  // printf("\nlast T position sector: %u", lastTPositionSector);
  // printf("\nLastT.atrType: %u\nLast.logicalBlockNumber: %u", lastT.atributeType, lastT.logicalBlockNumber);
  
  
  // this shall go to the last tuple in the sequence of registers if necessary
  // while(lastT.atributeType == 2)
  // {
  //   lastTPositionSector = lastT.virtualBlockNumber*2 + 4;
  //   lastT = find_last_tuple_MFT_register(lastTPositionSector); // find last not 0 atributeType tuple
  //   lastTPosition = find_position_last_tuple_MFT_register(lastTPositionSector);
  // }

  // now gotta find the position of an empty record in the dir
  unsigned int empty_record_position = find_empty_record_info(lastT.logicalBlockNumber, lastT.numberOfContiguosBlocks);
  unsigned int writeBlock = lastT.logicalBlockNumber; // this is the right block position
  writeBlock = check_recordPosition_valid(empty_record_position, writeBlock, lastT, lastTPositionSector, lastTPosition);
  if(empty_record_position == -1)
  {
    empty_record_position = 0;
  }

  if(write_new_arq(empty_record_position, isolated_filename, writeBlock, type) != 1)
    return -1;
  else
    return 1;
}

struct t2fs_record *find_record(char *filename, char *name)
{
  unsigned int MFT_sec = 6;
  struct t2fs_record *record = NULL;
  record = malloc(sizeof(struct t2fs_record));

  char *filenamecopy = strdup(filename);
  char *token;
  token = strtok(filenamecopy, "/");
  record = search_record_in_dir(MFT_sec, token);
  if(record == NULL){
      // printf("\npq ta dando essa bagaca?");
      return NULL;
  }
  MFT_sec = record->MFTNumber*2 + 4;

  while(strcmp(token, name) != 0)
  {
    token = strtok(NULL, "/");
    // printf("\ntoloko: %s",token);
    // printf("\nname: %s", name);
    // printf("\nmft secs: %u", MFT_sec);
    record = search_record_in_dir(MFT_sec, token); // MFT_of this directory
    if(record == NULL){
      // printf("\npq ta dando essa bagaca?");
      return NULL;
    }
    MFT_sec = record->MFTNumber*2 + 4;
    // printf("\nmft secs depois: %u", MFT_sec);
    
  }
  // if(MFT_sec == -1)
  //   return -1;

  
// }

//Given a filename and an mft_sector, creates a FILE_DESCRIPTOR with the info and returns it
FILE_DESCRIPTOR create_descriptor (char * filename, int file_mft_sector){
    
  FILE_DESCRIPTOR file_descriptor;

  //Initializes the file handler
  file_descriptor.current_pointer = 0;

  //Puts the name of the file on the handler
  file_descriptor.file_path = strdup(filename);

  //The loop ends with current_dir_sector at the MFT of the file
  file_descriptor.first_MFT_tuple = file_mft_sector;

  //Declares the file as valid
  file_descriptor.is_valid = 1;

  return file_descriptor;
}

//Opens a new directory or file, based on the mode
//1 - opens file
//2 - opens dir
//returns the file handle if it has success, or 0
//Returns -1 if it can't find the path to the file
//Returns -2 if the file doesn't exist on the directory it is supposed to be in
//Returns -3 if invalid mode was selected
//Returns -4 if tried to open root as a file
FILE2 open_file (char *filename, int mode){

  if (mode != 1 && mode != 2){
    return -3;
  }


  //If the file to be opened is the root file
  if (strcmp(filename, "/") == 0){
    
    if (mode == 2){
      int index = open_root_file();
      return index;
    }

    else return -4;

  }


  
  //isolated_filename is the filename without the subdirectories it is in
  char *isolated_filename = (strrchr(filename, '/'));
    isolated_filename = isolated_filename + 1;

  int parent_sector = get_parent_dir_MFT_sector(filename);

  //Couldn't reach the filename directory
  if (parent_sector == -1)
    return -1;


  int file_mftnumber = get_MFTnumber_of_file_with_directory_number(isolated_filename, parent_sector, mode);

  //File is not on the directory
  if(file_mftnumber == -1)
    return -2;
  

  int file_mft_sector = (file_mftnumber * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


  FILE_DESCRIPTOR file_descriptor =  create_descriptor(filename, file_mft_sector);

//Allocates file on opened_files or opened_directories
  int index = allocate_handler(file_descriptor, mode);

  

  return index;

}

//Allocates a handler in opened_files or opened_directories, depending on mode
//returns the handler it has allocated, or -1 if it failed
int allocate_handler(struct file_descriptor file_descriptor, int mode){

    int index = -1;


  if (mode == 1){
  //Allocates file
    index = first_free_file_position();
    opened_files[index] = file_descriptor;

    
  }
  else if (mode == 2) {
  //Allocates directory on opened_directories
    index = first_free_dir_position();
    opened_directories[index] = file_descriptor;

  }

  return index;

}

int open_root_file(){

  FILE_DESCRIPTOR desc = create_descriptor ("/", ROOT_MFT);
  int handle = allocate_handler(desc, 2);
  return handle;
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
  return record;
}

struct t2fs_record *search_record_in_dir(unsigned int sector, char *name)
{
  unsigned char bufferMFT[SECTOR_SIZE];
  unsigned char bufferBD[SECTOR_SIZE];
  struct t2fs_4tupla t;
  struct t2fs_record *record = NULL;
  record = malloc(sizeof(struct t2fs_record));
  // struct t2fs_record *nullRecord = NULL;
  // nullRecord = malloc(sizeof(struct t2fs_record));
  int error;
  unsigned int s;
  unsigned int MFT_sec = sector;
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
        return NULL;
      }
      else if(t.atributeType == 2){ // if reached the end of the MFT register (the last tuple) go to the next register
        i = 0;
        MFT_sec = t.virtualBlockNumber * 2 + 4; // recebe o registro e o converte pro setor desse registro
        break;
      }
        
      else{
        for(k = 0; k < t.numberOfContiguosBlocks; k++)
        { // reading BD
          s = (unsigned int)((t.logicalBlockNumber + k) * 4); // find the sector of the respective blocks
          for(p = 0; p<4; p++) // read the whole block
          {
            error = read_sector(s + p, bufferBD); // reading BD register
            if(error){
              return NULL;
            }
            for (r = 0; r < 4; r++)
            { //each sector has max 4 records

              *record = fill_directory(bufferBD, r);
              // for(g = (int)strlen(record.name) + 1; g< MAX_FILE_NAME_SIZE; g++)
              //   record.name[g] = 0x00;
              if(strcmp(name, record->name)==0){
                // int directory_start =  r * RECORD_SIZE;
                // printf("\nrecord.bytesFileSize: %u", record.bytesFileSize);

                // if(write_record_in_dir(s+p, directory_start, record) != 1)
                //   return -1;

                return record;
                
              }
            }
          }
        }
      }

    }
  }


  return NULL;

}


struct t2fs_record *path_return_record2(char* path)
{
  char *filenamecopy;
  struct t2fs_record *record = NULL;
  record = malloc(sizeof(struct t2fs_record));

  filenamecopy = strdup(path);
  char *isol_filename = (strrchr(filenamecopy, '/'));
  char *isolated_filename = strtok(isol_filename, "/");

  record = find_record(path, isolated_filename);
  if(record == NULL)
    return NULL; // NULL record

  return record;
}