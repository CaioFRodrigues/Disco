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
#include "../include/apidisk.h"
#include "../include/files.h"
#include "../include/auxiliar.h"

struct file_descriptor open_files[20];

int delete2(char *filename){

  FILE2 handle;

  handle = findFileHandleByName(filename);
  if (handle == -1)
    return handle;

  iterateMFT(open_files[handle]);

  return 0;
}

void invalidateMFT(struct t2fs_4tupla file){
  if (file.atributeType == 1){
    file.atributeType = -1;
    /*
    *   Code to get the next MFT tuple structure
    */
    struct t2fs_4tupla next_tuple; // Mock struct
    invalidateMFT(next_tuple);
  }
  else if (file.atributeType == 0){ // Last MFT tuple for the file, end recursion
    file.atributeType = -1;
  }

}
