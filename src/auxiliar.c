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

struct t2fs_bootBlock boot_block;

#define ID 0
#define VERSION 4
#define BLOCKSIZE 6
#define MFTBLOCKSSIZE 8
#define DISKSECTORSIZE 10

// Mock structures
int number_files_open;
struct file_descriptor open_files[20];

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

FILE2 findFileHandleByName(char *filepath){
  
  int i = 0;

  for (i=0; i<20; i++){
    if (open_files[i] != NULL && strncmp(filepath, open_files[i].file_path, sizeof(open_files[i].file_path)) == 0){
        return i;
    }
  }

  return -1;  // No open file has the given name name

}
