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
 	

	int director_sector = get_parent_dir_MFT_sector(filename);

	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));

	int file_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, director_sector, SEARCHING_DIRECTORY);
	

	int file_mft_sector = (file_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(filename);

	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.first_MFT_tuple = file_mft_sector;

	//Declares the file as valid
	file_descriptor.is_valid = 1;
	


	opened_files[first_free_file_position()] = file_descriptor;

	return 0;




}


DIR2 opendir(char *filename){

	int director_sector = get_parent_dir_MFT_sector(filename);

	//isolated_filename is the filename without the subdirectories it is in
	char *isolated_filename = (strrchr(filename, '/'));

	int dir_position = get_MFTnumber_of_file_with_directory_number(isolated_filename, director_sector, SEARCHING_DIRECTORY);
	

	int dir_mft_sector = (dir_position * SECTOR_PER_MFT) + BOOT_BLOCK_SIZE;


	FILE_DESCRIPTOR file_descriptor;

	//Initializes the file handler
	file_descriptor.current_pointer = 0;

	//Puts the name of the file on the handler
	file_descriptor.file_path = strdup(filename);

	//The loop ends with current_dir_sector at the MFT of the file
	file_descriptor.first_MFT_tuple = dir_mft_sector;

	//Declares the file as valid
	file_descriptor.is_valid = 1;
	
	int index = first_free_dir_position();

	opened_directories[index] = file_descriptor;

	return 0;


}

