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
CONDITIONS=-Wall -m32

all: cechin.o
	$(CC) -o exec -g $(BIN_DIR)cechin.o $(LIB_DIR)apidisk.o $(LIB_DIR)bitmap2.o $(CONDITIONS)

cechin.o: $(SRC_DIR)cechin.c
	$(CC) -o $(BIN_DIR)cechin.o -c $(SRC_DIR)cechin.c $(CONDITIONS)

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

