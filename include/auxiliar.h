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
#include "../include/files.h"


extern struct t2fs_bootBlock boot_block;

struct file_descriptor {
    int current_pointer;
    int first_MFT_tuple;
}

int init();
