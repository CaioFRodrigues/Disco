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
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/bitmap2.h"
#include "../include/files.h"

// Mock array to represent the pointers to the open files
int file_handles[20];
t2fs_4tupla t2fs_directories[20];

int seek2(FILE2 handle, DWORD offset){

  if (offset != -1){
    file_handles[handle] += offset;
  }
  else{
    file_handles = t2fs_directories[handle].logicalBlockNumber + t2fs_directories[handle].numberOfContiguosBlocks + 1;
  }

  /* Se o valor de "offset" for "-1", o current_pointer deverá ser posicionado no byte seguinte ao final do arquivo,
		Isso é útil para permitir que novos dados sejam adicionados no final de um arquivo já existente*/

  return 0;
}
