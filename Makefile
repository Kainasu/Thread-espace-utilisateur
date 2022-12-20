SRC_DIR=src
TST_DIR=tst
LIB_DIR=lib
BIN_DIR=bin
INSTALL_DIR=install
PTHREAD_DIR=pthread
LIB=libthread.so
BIN=01-main \
        02-switch \
		03-equity \
        11-join \
        12-join-main \
        21-create-many \
        22-create-many-recursive \
        23-create-many-once \
        31-switch-many \
        32-switch-many-join \
        33-switch-many-cascade \
        51-fibonacci \
		61-mutex \
   	    62-mutex  \
   	 	71-preemption \
		81-deadlock

EXEC=echo "TESTING 01-main ..." &&\
		$(INSTALL_DIR)/$(BIN_DIR)/01-main &&\
        echo "TESTING 02-switch ..."&&\
		$(INSTALL_DIR)/$(BIN_DIR)/02-switch &&\
		echo "TESTING 03-equity ..."&&\
		$(INSTALL_DIR)/$(BIN_DIR)/03-equity &&\
		echo "TESTING 11-join ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/11-join &&\
		echo "TESTING 12-join-main ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/12-join-main &&\
		echo "TESTING 21-create-many ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/21-create-many 20 &&\
		echo "TESTING 22-create-many-recursive ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/22-create-many-recursive 20 &&\
		echo "TESTING 23-create-many-once ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/23-create-many-once 20 &&\
		echo "TESTING 31-switch-many ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/31-switch-many 50 30&&\
		echo "TESTING 32-switch-many-join ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/32-switch-many-join 100 30&&\
		echo "TESTING 33-switch-many-cascade ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/33-switch-many-cascade 40 30&&\
		echo "TESTING 51-fibonacci ..."&&\
        $(INSTALL_DIR)/$(BIN_DIR)/51-fibonacci 10 &&\
		echo "TESTING 61-mutex ..."&&\
		$(INSTALL_DIR)/$(BIN_DIR)/61-mutex 10 &&\
		echo "TESTING 62-mutex ..."&&\
   	    $(INSTALL_DIR)/$(BIN_DIR)/62-mutex 10 &&\
		echo "TESTING 71-preemption ..."&&\
   	 	$(INSTALL_DIR)/$(BIN_DIR)/71-preemption 2 &&\
		echo "TESTING 81-deadlock ..."&&\
		$(INSTALL_DIR)/$(BIN_DIR)/81-deadlock


VALEXEC=echo "TESTING 01-main ..." &&\
		valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/01-main &&\
        echo "TESTING 02-switch ..."&&\
		valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/02-switch &&\
		echo "TESTING 03-equity ..."&&\
		valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/03-equity &&\
		echo "TESTING 11-join ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/11-join &&\
		echo "TESTING 12-join-main ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/12-join-main &&\
		echo "TESTING 21-create-many ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/21-create-many 20 &&\
		echo "TESTING 22-create-many-recursive ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/22-create-many-recursive 20 &&\
		echo "TESTING 23-create-many-once ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/23-create-many-once 20 &&\
		echo "TESTING 31-switch-many ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/31-switch-many 50 30&&\
		echo "TESTING 32-switch-many-join ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/32-switch-many-join 100 30&&\
		echo "TESTING 33-switch-many-cascade ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/33-switch-many-cascade 40 30&&\
		echo "TESTING 51-fibonacci ..."&&\
        valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/51-fibonacci 10 &&\
		echo "TESTING 61-mutex ..."&&\
		valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/61-mutex 10 &&\
		echo "TESTING 62-mutex ..."&&\
   	    valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/62-mutex 10 &&\
		echo "TESTING 71-preemption ..."&&\
   	 	valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/71-preemption 2 &&\
		echo "TESTING 81-deadlock ..."&&\
		valgrind $(VFLAGS) $(INSTALL_DIR)/$(BIN_DIR)/81-deadlock


PTHREADS=${BIN:=-pthread}
export LD_LIBRARY_PATH=./$(INSTALL_DIR)/$(LIB_DIR)

CC=gcc
CFLAGS=-W -Wall -Isrc -g -O0
THREADFLAGS = -DUSE_PTHREAD -pthread
PWD=${pwd}
LDFLAGS= -L$(INSTALL_DIR)/$(LIB_DIR) -lthread
VFLAGS= --leak-check=full --show-reachable=yes --track-origins=yes 

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(notdir $(SRC:.c=.o))

SRC_TST=$(wildcard $(TST_DIR)/*.c)
BIN_TST=$(notdir $(SRC_TST:.c=))


.PHONY: all build install clean install

all: install 

build: ${LIB} ${BIN} ${PTHREADS}

install_dir:
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(INSTALL_DIR)/$(LIB_DIR)
	mkdir -p $(INSTALL_DIR)/$(BIN_DIR)
	mkdir -p $(INSTALL_DIR)/pthread


install_bin: $(BIN)


install_lib: $(LIB) 

#install_pthread: build install_dir 
#	mv $(PTHREADS) $(INSTALL_DIR)/pthread	
#	$(PTHREADS)


#install : install_bin install_lib install_pthread
install: install_dir install_lib install_bin

thread.o: ${SRC_DIR}/thread.c
	${CC} ${CFLAGS} -fPIC -c $< -o $(INSTALL_DIR)/$(LIB_DIR)/$@

libthread.so: thread.o
	${CC} -shared -o $(INSTALL_DIR)/$(LIB_DIR)/$@ $(INSTALL_DIR)/$(LIB_DIR)/$^ -lrt

                                                                                                                                                                                                       
%.o: ${TST_DIR}/%.c ${SRC_DIR}/thread.h
	${CC} ${CFLAGS} -c $< -o $@ ${LDFLAGS}

% : %.o libthread.so
	${CC} -o $(INSTALL_DIR)/$(BIN_DIR)/$@ $< ${LDFLAGS}


check:
	@$(EXEC)
#$(INSTALL_DIR)/$(BIN_DIR)/$(BIN)	
	

%-pthread: ${TST_DIR}/%.c
	${CC} -o $(INSTALL_DIR)/$(PTHREAD_DIR)/$@ $< ${CFLAGS} ${THREADFLAGS}

pthreads: $(PTHREADS)	

valgrind:
	$(VALEXEC)	
	
graphs: install
	python3 src/graph.py

clean:
	rm -rf *.o *~ \#*\# *.so ${BIN} ${PTHREADS};
	@rm -rf $(INSTALL_DIR)/* $(INSTALL_DIR)

