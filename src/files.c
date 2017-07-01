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
#include "../include/files.h"

int delete2(char *filename){

  /*
  *   Code to find the file_descriptor instance that belongs to the file name
  *   Must return error code when a struct isn't found (i.e. invalid file name)
  * return -1;
  */
  struct t2fs_4tupla file;  // Mock struct
  iterateMFT(file);

  return 0;
}

void iterateMFT(struct t2fs_4tupla file){
  if (file.atributeType == 1){
    file.atributeType = -1;
    /*
    *   Code to get the next MFT tuple structure
    */
    struct t2fs_4tupla next_tuple; // Mock struct
    iterateMFT(next_tuple);
  }
  else if (file.atributeType == 0){ // Last MFT tuple for the file, end recursion
    file.atributeType = -1;
  }

}
