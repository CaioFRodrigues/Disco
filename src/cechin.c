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


int main(int argc, char *argv[]) {
	init();

	char test[2052];
	// create2("/file3");
	// create2("/file4");
	// create2("/file5");
	// mkdir2("/arateus");
	// mkdir2("/arateus/eh");
	// mkdir2("/arateus/eh/deuso");
	// mkdir2("/arateus/eh/deuso/metaleiro");
	// mkdir2("/MyUltimateIsReady");
	// create2("/MyUltimateIsReady/takeThis");
	printf("%d", open2("/MyUltimateIsReady/takeThis"));
	char buffer[] = "STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF STUFF ";
	// write2(0, buffer, 2052);
	read2(0, test, 2052);
	printf("%s", test);
	mkdir2("/arateus");
	mkdir2("/arateus/eh");
	mkdir2("/arateus/eh/deuso");
	create2("/arateus/eh/deuso/EOQ");
	// mkdir2("/MyUltimateIsReady");
	// create2("/MyUltimateIsReady/takeThis");
	// struct t2fs_record *record = NULL;
	// record = malloc(sizeof(struct t2fs_record));
	struct t2fs_record *record;
	record = path_return_record2("/arateus/eh");

	if(record == NULL)
		printf("\nrecord: %p", record);
	else{
		printf("\nrecord: %p", record);
		printf("\ntype: %u", record->TypeVal);
	    printf("\nname: %s", record->name);
	    printf("\nblocksSize: %u", record->blocksFileSize);
	    printf("\nbytesSize: %u", record->bytesFileSize);
	    printf("\nMFTNum: %u", record->MFTNumber);
	}
	printf("\n");


	struct t2fs_record *record2;
	record2 = path_return_record2("/arateus/eh/deuso");

	if(record == NULL)
		printf("\nrecord: %p", record2);
	else{
		printf("\nrecord: %p", record2);
		printf("\ntype: %u", record2->TypeVal);
	    printf("\nname: %s", record2->name);
	    printf("\nblocksSize: %u", record2->blocksFileSize);
	    printf("\nbytesSize: %u", record2->bytesFileSize);
	    printf("\nMFTNum: %u", record2->MFTNumber);
	}
	printf("\n");



	struct t2fs_record *record3;
	record3 = path_return_record2("/arateus/eh/deuso/EOQ");

	if(record == NULL)
		printf("\nrecord: %p", record3);
	else{
		printf("\nrecord: %p", record3);
		printf("\ntype: %u", record3->TypeVal);
	    printf("\nname: %s", record3->name);
	    printf("\nblocksSize: %u", record3->blocksFileSize);
	    printf("\nbytesSize: %u", record3->bytesFileSize);
	    printf("\nMFTNum: %u", record3->MFTNumber);
	}
	printf("\n");


	return 0;
}

