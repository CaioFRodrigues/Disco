#include "../include/MFT_list.h"


//Checks whether or not the MFT list is empty or not
int empty_MFT (MFT *list){

	return list == NULL;
}

//Pushes info 
MFT* push_MFT(MFT *list, struct t2fs_4tupla info, int sector, int offset){
	
	MFT *new_MFT = NULL;

	/* Creates new_MFT node */ 
	new_MFT = malloc(sizeof(MFT));
	new_MFT->current_MFT = info;
	new_MFT->next = NULL;
	new_MFT->sector = sector;
	new_MFT->offset = offset;
	
	/* Puts new_MFT node */
	if(empty_MFT(list))
		list = new_MFT;
	else {
		MFT *aux_list = list;

		while(aux_list->next != NULL) 
			aux_list = aux_list->next;

		aux_list->next = new_MFT;
	
	}

	return list;
}


//Caio
//Given two MFT lists, puts the second on the end of the first one
MFT* append_MFT(MFT *list1, MFT *list2){
	MFT *aux_list = list1;

	while (aux_list->next != NULL)
		aux_list = aux_list->next;

	aux_list->next = list2;

	return list1;
}

void free_MFT(MFT *list){
	MFT *aux_list = NULL;

	while (list != NULL){
		aux_list = list;
		list = list->next;
		free(aux_list);
	}
}