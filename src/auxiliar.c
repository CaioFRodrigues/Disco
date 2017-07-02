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
#include "../include/apidisk.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/auxiliar.h"

/*
TYPEVAL_REGULAR     0x01 -> 1
TYPEVAL_DIRETORIO   0x02 -> 2
*/
#define SIZE_of_MFT_blocos 2048
#define SIZE_of_MFT_REGISTER_bytes 512
#define SIZE_of_MFT_REGISTER_sector 2

#define ATRIB_TYPE 0
#define VBN 4
#define LBN 8
#define CONTIG_BLOCK 12

#define ID 0
#define VERSION 4
#define BLOCKSIZE 6
#define MFTBLOCKSSIZE 8
#define DISKSECTORSIZE 10

struct t2fs_bootBlock boot_block;


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
  printf("%s\n", boot_block.id);

  boot_block.version = (buffer[VERSION+1] << 8) | buffer[VERSION];
  printf("%#X\n",boot_block.version);

  boot_block.blockSize = (buffer[BLOCKSIZE+1] << 8) | buffer[BLOCKSIZE];
  printf("%#X\n",boot_block.blockSize);

  boot_block.MFTBlocksSize = (buffer[MFTBLOCKSSIZE+1] << 8) | buffer[MFTBLOCKSSIZE];
  printf("%#X\n",boot_block.MFTBlocksSize);

  boot_block.diskSectorSize = (buffer[DISKSECTORSIZE+3] << 16) | (buffer[DISKSECTORSIZE+2] << 12) | (buffer[DISKSECTORSIZE+1] << 8) | buffer[DISKSECTORSIZE];
  printf("%#X\n",boot_block.diskSectorSize);

  return 0;
}


unsigned int swap_4BYTE_endianess(unsigned int hex)
{
    unsigned int swapped = ((hex>>24)&0xff) | // move byte 3 to byte 0
                           ((hex<<8)&0xff0000) | // move byte 1 to byte 2
                           ((hex>>8)&0xff00) | // move byte 2 to byte 1
                           ((hex<<24)&0xff000000); // byte 0 to byte 3

    return swapped;
}

// if size = 2, convert 2 byte string to hex; 
// if size = 4, convert 4 byte string to hex; 
// else error
unsigned int conv_string_to_hex(unsigned char *buffer, unsigned int position, int size)
{
        unsigned int hex;
        if(size == 2){
            hex = (buffer[position+1] << 8) | buffer[position];

        }
        else if(size == 4){
            int aux = (buffer[position] << 24) |
                        (buffer[position+1] << 16) |
                        (buffer[position+2] << 8) |
                        buffer[position+3];

            hex = swap_4BYTE_endianess(aux);
        }
        else
            return -1;

        return hex;
}

int find_empty_MFT_reg()
{
  unsigned char buffer[SECTOR_SIZE];

  int full_MFT = -1;
  int registro = 4;

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

    printf("atrType: %#X\n", realAtrType); 

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