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
#include "../include/t2fs.h"
#include "../include/bitmap2.h"
#include "../include/files.h"
#include "../include/MFT.h"
#include "../include/MFT_list.h"

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
