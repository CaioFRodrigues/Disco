#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/auxiliar.h"
#include "../include/files.h"
#include "../include/t2fs.h"
#include "../include/MFT_list.h"
#include "../include/MFT.h"
#include "../include/directories.h"
void help() {

	printf ("Testing program - read and write setores do arquivo t2fs_disk.dat\n");
	printf ("   DISPLAY - d <setor>\n");
	printf ("\n");
	printf ("   HELP    - ?\n");
	printf ("   FIM     - f\n");
}

int main(int argc, char *argv[])
{

	// init();

	// int position;
	// position = searchBitmap2(0);

	// printf("Primeiro bloco livre: %d\n", position);

	// int blockNumber = 4;
	// printf("Printando %d bitmap:\n", blockNumber);

	// printf("%d\n", getBitmap2(blockNumber));

	// printf("%s\n", boot_block.id);

	// printf("%#X\n",boot_block.version);

	// printf("%#X\n",boot_block.blockSize);

	// printf("%#X\n",boot_block.MFTBlocksSize);

	// printf("%#X\n",boot_block.diskSectorSize);

	// char	command[128];
	// char	*cmd;
	
	// help();
	// while (1) {
	// 	printf ("CMD> ");
	// 	gets(command);
	// 	if (strlen(command)==0)
	// 		continue;
	
	// 	cmd = strtok(command, " ");
	
	// 	if (strcmp(cmd,"?")==0) {
	// 		help();
	// 		continue;
	// 	}
	
	// 	if (strcmp(cmd,"d")==0) {
	// 		// comando d (display)
	// 		unsigned char buffer[SECTOR_SIZE];
	// 		char *p1 = strtok(NULL, " ");
	// 		if (p1==NULL) {
	// 			printf ("Erro no comando.\n");
	// 			continue;
	// 		}
	// 		int sector = atoi(p1);
	// 		int error = read_sector (sector, buffer);
	// 		if (error) {
	// 			printf ("read_sector (%d) error = %d\n", sector, error);
	// 			continue;
	// 		}
	
	// 		char str[20];
	// 		int linhaBase = SECTOR_SIZE * sector;
	// 		int linha, coluna;
	// 		for (linha=0; linha<16; ++linha) {
	// 		    printf ("%04X  ", linhaBase+16*linha);
	// 		    for (coluna=0; coluna<16; ++coluna) {
	// 			int index = 16*linha+coluna;
	// 			char c = buffer[index];
	// 			if (c>=' ' && c<='z') str[coluna]=c;
	// 			else str[coluna]=' ';
	// 			printf ("%02X ", c&0xFF);
	// 		    }
	// 		    str[16]='\0';
	// 		    printf (" *%s*\n", str);
	// 		}
	// 		continue;
	// 	}
	
	// 	if (strcmp(cmd,"f")==0) {
	// 		printf ("Fim.\n");
	// 		break;
	// 	}
	
	// 	printf("Comando nao reconhecido.\n");
	// }
	init();
	//MFT * my_mft = read_MFT(8);
	//printf ("\n%d\n", get_MFT_from_filename_and_dir(2050, "file1"));


	// char file[6] = "/file2";
 //    open2 (file);
 //    printf ("%d", opened_files[0].file_MFT->sector);


	
	

    return 0;


}
