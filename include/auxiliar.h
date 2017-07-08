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
#ifndef __AUXILIARH__
#define __AUXILIARH__
#include <stdio.h>
#include <string.h>

#include "../include/apidisk.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/conversion.h"
#include "../include/MFT_list.h"
#include "../include/MFT.h"
#include "../include/directories.h"

#define ID 0
#define VERSION 4
#define BLOCKSIZE 6
#define MFTBLOCKSSIZE 8
#define DISKSECTORSIZE 10
#define MAX_OPENED_FILES 20
#define ROOT_BLOCK 2050
#define BOOT_BLOCK_SIZE 4
#define ROOT_MFT 6

extern struct t2fs_bootBlock boot_block;

//File Descriptor - Holds the info of an opened file
typedef struct file_descriptor {

    int current_pointer; //Has the virtual block of the position in the current file
    int first_MFT_tuple; // Has the MFT position of the file
    char *file_path; //Current file_path with the format: /dir1/dir2/dir3/file
    int is_valid; //Checks whether the current descriptor is actually opened or not
    int fileSizeBytes;
   
} FILE_DESCRIPTOR;

// Mock structures
int number_files_open;
struct file_descriptor opened_files[MAX_OPENED_FILES];

//Ana
//Initializes the boot block with the needed information
int init();

void initialize_open_files();

int first_free_file_position();

int virtual_block_to_logical_block(DWORD current_virtual_block, MFT* mft_list);

int find_byte_position_in_logical_block(MFT* mft,int bytes);

#endif
