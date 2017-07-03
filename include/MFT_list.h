#ifndef __MFTLISTH__
#define __MFTLISTH__



#include <stdlib.h>
#include "../include/t2fs.h"



typedef struct MFT_list{
	struct 	t2fs_4tupla current_MFT; //information from the file
	int sector; //The sector which the node belongs
	int offset; //The offset from the sector where the element belongs
	struct MFT_list *next; //next of the list
} MFT;

int empty_MFT (MFT *list);
MFT* push_MFT(MFT *list, struct t2fs_4tupla info, int sector, int offset);
MFT* append_MFT(MFT *list1, MFT *list2);
void free_MFT(MFT *list);


#endif