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

#define SIZE_of_MFT_blocos 2048
#define SIZE_of_MFT_REGISTER_bytes 512
#define SIZE_of_MFT_REGISTER_sector 2
#define SIZE_of_4TUPLE 16
#define TUPLES_PER_REGISTER 32
#define ROOT_MFT_SECTOR 6
#define ROOT_BD_SECTOR 8200
#define SIZE_of_RECORD 64

#define RECORD_TYPEVAL 0
#define RECORD_NAME 1
#define RECORD_BLOCK_FILESIZE 52
#define RECORD_BYTES_FILESIZE 56
#define RECORD_MFTNUMBER 60

#define ATRIB_TYPE 0
#define VBN 4
#define LBN 8
#define CONTIG_BLOCK 12

#define TUPLE_ATRTYPE 0 
#define TUPLE_VBN 4
#define TUPLE_LBN 8
#define TUPLE_NUMCONTIGBLOCK 12

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

int number_dir_handles;
FILE_DESCRIPTOR * opened_directories;
//Ana
//Initializes the boot block with the needed information
int init();

void initialize_open_files();

int is_file_open(char * file_name);

void initialize_open_directories();

int is_directory_open(char * file_name);

int first_free_file_position();

void read_bytes(int starting_byte, int ending_byte, int bytes_to_copy, char* source, char* destination);

DWORD virtual_block_to_logical_block(DWORD current_virtual_block, MFT* mft_list);

int find_byte_position_in_logical_block(MFT* mft,int bytes);

char* append_buffers( char* final_buffer, char* temp_buffer );

int write_first_tuple_MFT_and_set_0_second(unsigned int sector, int offset, struct t2fs_4tupla t);

int alocate_needed_blocks(int blocks_needed, MFT* mft, MFT* last_mft);

int insert_in_sector(int sector, char* content, int start, int size);

int first_free_dir_position();

int find_empty_MFT_reg();

unsigned int take_sector_from_position(unsigned int record_position);

unsigned int take_right_position(unsigned int record_position);

unsigned int find_empty_record_info(unsigned int lbn, unsigned int contigBlock);

int write_record_in_dir(unsigned int sector, unsigned int byte_pos, struct t2fs_record *record);

int write_on_last_tuple_MFT_and_set_0_second(unsigned int sector, struct t2fs_4tupla t, unsigned int tupleNum);

int clear_sector(unsigned int sector);

int clear_block(int init_sector);

struct t2fs_record path_return_record(char* path);

int find_record_and_add_byteRecord(unsigned int sector, char *name);

int check_recordPosition_valid(unsigned int record_position, unsigned int writeBlock, struct t2fs_4tupla lastT, unsigned int lastTPositionSector, unsigned int lastTPosition);

int write_new_arq(unsigned int record_position, char *isolated_filename, unsigned int writeBlock, DWORD type);

int generic_create(char *filename, DWORD type);

int find_father_record_and_add_byteRecord(char *filename, char *name);

unsigned int search_record_in_dir_and_add(unsigned int sector, char *name);
FILE_DESCRIPTOR create_descriptor (char * filename, int file_mft_sector);

FILE2 open_file (char *filename, int mode);

int allocate_handler(struct file_descriptor file_descriptor, int mode);

int open_root_file();
int clear_file(MFT* mft, int current_pointer);

struct t2fs_record *find_record(char *filename, char *name);

struct t2fs_record *search_record_in_dir(unsigned int sector, char *name);

struct t2fs_record *path_return_record2(char* path);

#endif
