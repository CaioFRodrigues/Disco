#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
#

CC=gcc
LIB_DIR=./lib/
INC_DIR=./include/
BIN_DIR=./bin/
SRC_DIR=./src/
TST_DIR=./testes/
CONDITIONS=-Wall -m32 -std=gnu99 -lm

all: cechin.o auxiliar.o t2fs.o conversion.o MFT_list.o MFT.o directories.o
	$(CC) -o exec -g $(LIB_DIR)cechin.o $(LIB_DIR)t2fs.o $(LIB_DIR)auxiliar.o $(LIB_DIR)apidisk.o $(LIB_DIR)bitmap2.o $(LIB_DIR)conversion.o $(LIB_DIR)MFT_list.o  $(LIB_DIR)MFT.o $(LIB_DIR)directories.o $(CONDITIONS)

cechin.o: $(SRC_DIR)cechin.c
	$(CC) -o $(LIB_DIR)cechin.o -c $(SRC_DIR)cechin.c $(CONDITIONS)

auxiliar.o: $(SRC_DIR)auxiliar.c
	$(CC) -o $(LIB_DIR)auxiliar.o -c $(SRC_DIR)auxiliar.c $(CONDITIONS)

t2fs.o: $(SRC_DIR)files.c
	$(CC) -o $(LIB_DIR)t2fs.o -c $(SRC_DIR)t2fs.c $(CONDITIONS)

conversion.o:
	$(CC) -o $(LIB_DIR)conversion.o -c $(SRC_DIR)conversion.c $(CONDITIONS)

MFT_list.o:
	$(CC) -o $(LIB_DIR)MFT_list.o -c $(SRC_DIR)MFT_list.c $(CONDITIONS)

MFT.o:
	$(CC) -o $(LIB_DIR)MFT.o -c $(SRC_DIR)MFT.c $(CONDITIONS)

directories.o:
	$(CC) -o $(LIB_DIR)directories.o -c $(SRC_DIR)directories.c $(CONDITIONS)

conversion.o:
	$(CC) -o $(LIB_DIR)conversion.o -c $(SRC_DIR)conversion.c $(CONDITIONS)


MFT_list.o:
	$(CC) -o $(LIB_DIR)MFT_list.o -c $(SRC_DIR)MFT_list.c $(CONDITIONS)

MFT.o:
	$(CC) -o $(LIB_DIR)MFT.o -c $(SRC_DIR)MFT.c $(CONDITIONS)

directories.o:
	$(CC) -o $(LIB_DIR)directories.o -c $(SRC_DIR)directories.c $(CONDITIONS)
clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~ exec
