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
#include "../include/files.h"


extern struct t2fs_bootBlock boot_block;

//File Descriptor - Holds the info
typedef struct file_descriptor {

    int current_pointer; //Has the virtual block of the position in the current file
    int first_MFT_tuple; // Has the MFT position of the file
    char *file_path; //Current file_path with the format: /dir1/dir2/dir3/file
    int is_valid; //Checks whether the current descriptor is actually opened or not
    
} FILE_DESCRIPTOR;

//Ana
//Initializes the boot block with the needed information
int init();
