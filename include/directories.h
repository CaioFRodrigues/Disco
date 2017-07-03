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


#ifndef __DIRECTORIESH__
#define __DIRECTORIESH__

#define RECORD_SIZE 64 //Size of the struct t2fs_record in bytes
#define NAME_SIZE 51

#define NAME_START 1
#define BLOCKS_FILE_SIZE_START 52
#define BYTES_FILE_SIZE_START 56
#define MFT_NUMBER_START 60

#define RECORDS_PER_SECTOR 4

#include <stdio.h>
#include <string.h>

#include "../include/auxiliar.h"

struct t2fs_record fill_directory(unsigned char* buffer, int directory_number);

int get_MFT_from_filename_of_dir(int block, char *name, int mode);


#endif

