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
	
	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));
	isolated_filename = isolated_filename + 1;
	
	//Goes through the subdirectories
	while(strcmp(token,isolated_filename) != 0){
		get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_DIRECTORY);
	}

	int file_position = get_MFTnumber_of_file_with_directory_number(token, current_dir_sector, SEARCHING_FILE);
	

	int file_mft_sector = (file_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(isolated_filename);

	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.first_MFT_tuple = file_mft_sector;

	//Declares the file as valid
	file_descriptor.is_valid = 1;
	
	isolated_filename = isolated_filename - 1;
	free(isolated_filename);
	


	opened_files[first_free_file_position()] = file_descriptor;

	return 0;


}
