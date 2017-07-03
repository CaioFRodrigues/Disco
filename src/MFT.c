#include "../include/MFT.h"




//Given a sector, returns the first MFT of the file
int first_block_of_MFT(int sector){

  //Buffer that will hold the entire sector
  unsigned char buffer[SECTOR_SIZE];

  //Reads the sector and puts it on the buffer
  read_sector(sector, buffer);

  return swap_local_endianess(buffer, LOGICAL_BLOCK_NUMBER_START);
}

//Caio
//Given a sector, reads the MFT and gets all the info from it
MFT* read_MFT(int sector){
  MFT *MTF_list = NULL;
  int next_sector;

  do{
    next_sector = read_sector_as_MFT(sector, &MTF_list);
  } while(next_sector != 0);

  return MTF_list;
}

//Caio
//Given a sector, reads it as an MFT and returns where the next sector is located

//list: pointer to the current MFT list, everything will be put in the end of that
//sector: the sector that it is reading

//return: Next_list
//If it returns 0, the list has ended
//if it returns another number, the list will continue on that sector
int read_sector_as_MFT(int sector, MFT** list){

  //Buffer that will hold the entire sector
  unsigned char buffer[SECTOR_SIZE];

  //Reads the sector and puts it on the buffer
  read_sector(sector, buffer);

  //current_position will hold which line of the file we will be for each MFT
  int current_position = 0;

  struct t2fs_4tupla current_MFT;

  do{    
    //Gets the current_MFT
    current_MFT = fill_MFT(buffer, current_position);

    switch(current_MFT.atributeType){
      //if current_MFT is valid, then the info must be saved
      case 1:
        //Puts the current info, its sector and the first byte where it is located from the sector
        *list = push_MFT(*list, current_MFT, sector, current_position * MFT_4TUPLA_SIZE); 
      break;

      //If attribute is 2, then the next sector is specified on virtualBlockNumber
      case 2:
        return current_MFT.virtualBlockNumber;
      break;

      //If attribute is 0, then the MFT has ended with success
      case 0:
        return 0;
      break;

      //if none of those happened, continue reading the next as normal
      case -1:
        return 0;
      break;

    }

    current_position++;
  } while (current_MFT.atributeType == 1 && current_position < MFT_4TUPLA_SIZE); //While the MFT is valid and on range


  //If it didn't end with a 2 or a 0, then that means that the next sector will contain one of those
  return sector + 1;

}

//Caio
//Given a buffer and a start, creates a t2fs_4tupla with the information starting from that register
//buffer: Buffer from read_sector(sector)
//MFT_register: register N of that buffer: Register 2 starts at buffer[16], and so on
struct t2fs_4tupla fill_MFT(unsigned char* buffer, int MFT_4tupla_number){
  //Struct where info will be saved
  struct t2fs_4tupla MFT_4tupla;

  //Byte where the the tupla starts on the buffer
  int MFT_4tupla_position = MFT_4TUPLA_SIZE * MFT_4tupla_number;

  //Gets the information translating the bytes to big_endian and to int
  MFT_4tupla.atributeType = swap_local_endianess(buffer,  ATTRIBUTE_TYPE_START + MFT_4tupla_position);
  MFT_4tupla.virtualBlockNumber = swap_local_endianess(buffer, VIRTUAL_BLOCK_NUMBER_START + MFT_4tupla_position);
  MFT_4tupla.logicalBlockNumber = swap_local_endianess(buffer, LOGICAL_BLOCK_NUMBER_START + MFT_4tupla_position);
  MFT_4tupla.numberOfContiguosBlocks = swap_local_endianess(buffer, NUMBER_OF_CONTIGUOS_BLOCKS_START + MFT_4tupla_position);

  return MFT_4tupla;



}

//Caio
//Given a directory name and a MFT describing a directory, goes through the directory and gets the number of the next MFT
//returns -1 if the MFT is not found
//returns 0 if the file was found
int get_next_MFT(char *dirname, MFT* directory, int mode){
	//Auxiliar iterator
	MFT* aux = directory;

	//Goes through every single node
	while (aux != NULL){

		//For every node goes through the contiguous blocks
		for (int current_block = 0; current_block < aux->current_MFT.numberOfContiguosBlocks; current_block++){
			//Gets next position by comparing the strings
			int next = get_MFT_from_filename_of_dir(aux->current_MFT.logicalBlockNumber + current_block, dirname, mode);
			
			//If next is valid
			if (next != 0)
				return next;
		}

		aux = aux->next;
	}

	return 0;


}