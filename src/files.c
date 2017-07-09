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
#include "../include/bitmap2.h"
#include "../include/auxiliar.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/MFT_list.h"
#include "../include/MFT.h"
#include "../include/directories.h"
#include "../include/math.h"

// Ana
int write2(FILE2 handle, char* buffer, int size){

    unsigned char sector_buffer[SECTOR_SIZE]; // Buffer used to read from sector
    unsigned char temp_buffer[SECTOR_SIZE]; // Buffer that will recieve the sliced buffer content
    int current_byte = 0;
    int current_sector_pointer = 0;
    int first_byte = 3;
    int i=0, j=0;
    MFT* last_mft;

        // Update file register sizes (bytes and block)
    // if (size+first_byte > opened_files[handle].fileSizeBytes){
    //     int new_byte_size = size + first_byte;
    //     int new_block_size = ceil(new_byte_size/1024.0);

    //     // Change file descriptor
    //     opened_files[handle].current_pointer = new_byte_size + 1;
    //     opened_files[handle].fileSizeBytes = new_byte_size;

    //     // Change file register
    // }
    // else{
    //     opened_files[handle].current_pointer = size+first_byte;
    // }

    MFT* mft = read_MFT(12);  // For testing only

    // Writes buffer to file
    while (size > 0){
        while (mft != NULL){    // Must be changed to mft-> next !=NULL when mft stores the tuple with attributeType = 0
            int first_block = mft->current_MFT.logicalBlockNumber;
            int number_blocks = mft->current_MFT.numberOfContiguosBlocks;

            int num_sectors = number_blocks * 4;
            int first_sector = first_block * 4;

            // Loops through contiguos sectors from MFT
            for (j=0; j<num_sectors; j++){

                current_sector_pointer = 0;

                int sector_current = ceil(ceil(first_byte/256.0)/4.0)-1;

                if (sector_current == i){
                    current_sector_pointer = first_byte;
                }

                if (size > (256-current_sector_pointer)){   // Fills entire sector

                    for (i=0; i<(256-current_sector_pointer); i++){
                        temp_buffer[i] = buffer[current_byte];
                        current_byte++;
                    }
                    size -= (256-current_sector_pointer);
                    insert_in_sector(j+first_sector, temp_buffer, current_sector_pointer, (256-current_sector_pointer));
                }
                else{   // Writes last bytes into sector
                    for (i=0; i<size; i++){
                        temp_buffer[i] = buffer[current_byte];
                        current_byte++;
                    }
                    insert_in_sector(j+first_sector, temp_buffer, current_sector_pointer, size);
                    return 0;
                }
            }
            last_mft = mft; // Stores previous mft tuple for when the loop ends
            mft = mft->next;
        }


        /*
        *   BASIC LOGIC FOR THIS PART
        *       Allocates new block
        *           Checks if the block is contiguos with the blocks from the last mft tuple from the file
        *               If it is, just update teh mft tuple
        *           If it isn't, create new tuple and write it to block
        *
        *
        */


        int blocks_needed = ceil(size/1024.0);
        int n = 0;
        int new_block = 0;

        for (n=0; n < blocks_needed; n++){

            new_block = searchBitmap2(0);

            if (new_block < 0){

                return -1;
            }

            if (setBitmap2(new_block, 1) != 0){
                return -1;
            }
        }

        if (new_block == last_mft->current_MFT.logicalBlockNumber + last_mft->current_MFT.numberOfContiguosBlocks + 1){
            // Block contiguos to the ones in the last MFT
            last_mft->current_MFT.numberOfContiguosBlocks++;
        }
        else{
            last_mft->current_MFT.atributeType = -1;    // Invalidates what used to be last tuple for file (attributeType = 0)

            // Create new MFT tuple
            struct t2fs_4tupla new_tuple;
            new_tuple.atributeType = 2;
            new_tuple.virtualBlockNumber = -1;  // Needs to point to the next one, probably updated later
            new_tuple.logicalBlockNumber = new_block;
            new_tuple.numberOfContiguosBlocks = 1;

            // mft->current_MFT.virtualBlockNumber = Needs to point to the new_tuple logical block
            if (last_mft->offset == 15){
                // Needs new sector
                mft = push_MFT(mft, new_tuple, last_mft->sector+1, 0);
            }
            else{
                mft = push_MFT(mft, new_tuple, last_mft->sector, last_mft->offset+1);
            }
            last_mft = mft;
                
        }
    }
}

// Ana
// Given a buffer with content to be inserted in a sector,
// inserts it in the given place and writes it to the sector
int insert_in_sector(int sector, char* content, int start, int size){

    char sector_content[SECTOR_SIZE];

    read_sector(sector, sector_content);

    int i=0, k=0;

    k = start;
    for (i=0; i<size; i++){
        sector_content[k] = content[i];
        k++;
    }

    write_sector(sector, sector_content);
}