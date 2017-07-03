#include <string.h>
#include <stdio.h>
#include "../include/apidisk.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/auxiliar.h"
#include "../include/MFT.h"

//Caio
//Open2: Searches the filename on the tree of directories, then returns allocates the handler to the array of opened files
FILE2 open2 (char *filename){
 
	char *token, *filenamecopy;

	filenamecopy = strdup(filename);
	
	token = strtok(filenamecopy, "/");
	

	//Initializes the root MFT
	int current_dir_sector = ROOT_MFT;
	
	char *last = (strrchr(filename, '/'));
	last = last + 1;
	

	while(strcmp(token,last) != 0){
		printf("oi %s\n", last);
		//Gets the list of MFT for the file that is needed
		MFT * current_dir_MFT = read_MFT(current_dir_sector);
		printf ("%d\n", current_dir_MFT->current_MFT.numberOfContiguosBlocks);
		//Gets the position based on the current directory
		int current_dir_position = get_next_MFT(token, current_dir_MFT, 2);
		if (current_dir_position != 0){
			//Calculates the sector where the MTF is
			current_dir_sector = (current_dir_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;

			//Frees the MFT
			free_MFT(current_dir_MFT);

			token = strtok(NULL, "/");
		}
		//The file was not found
		else {

			free_MFT(current_dir_MFT);
			return -1;
		}
	}

	MFT *current_dir_MFT = read_MFT(current_dir_sector);
	int file_position = get_next_MFT(token, current_dir_MFT, 1);
	

	int file_sector = (file_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(last);

	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.file_MFT = read_MFT(file_sector);

	//Declares the file as valid
	file_descriptor.is_valid = 1;
	
	opened_files[first_free_file_position()] = file_descriptor;

	return 0;


}
