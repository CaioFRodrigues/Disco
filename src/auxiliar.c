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


#include "../include/auxiliar.h"

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

//Arateus
// write tuple in the first position of the MFT register and set the secon tuple with 0x0000
// given a sector and the desired tuple to insert
// it will return 1 if it worked or -1 if something goes wrong
int write_first_tuple_MFT_and_set_0_second(unsigned int sector, struct t2fs_4tupla t)
{
  unsigned char buffer[SECTOR_SIZE];
  int error = read_sector(sector, buffer);
  unsigned int zero = 0x00;
  if(error)
    return -1;

  unsigned int aux;
  // write AtributeType in the first tuple in the MFT
  int i;
  for (i = 0; i < 4; i++)
  {
    aux = (t.atributeType >> 8*i)&0xff;
    buffer[TUPLE_ATRTYPE+i] = aux;
  }
  // write virtualBlockNumber
  for (i = 0; i < 4; i++)
  {
    aux = (t.virtualBlockNumber >> 8*i)&0xff;
    buffer[TUPLE_VBN+i] = aux;
  }
  // write logicalBlockNumber
  for (i = 0; i < 4; i++)
  {
    aux = (t.logicalBlockNumber >> 8*i)&0xff;
    buffer[TUPLE_LBN+i] = aux;
  }
  // write numberOfContiguousBlocks
  for (i = 0; i < 4; i++)
  {
    aux = (t.numberOfContiguosBlocks >> 8*i)&0xff;
    buffer[TUPLE_NUMCONTIGBLOCK+i] = aux;
  }
  // write 0 in the second tuple
  for (i = 0; i < 4; i++)
  {
    buffer[TUPLE_ATRTYPE+16+i] = zero;
  }

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