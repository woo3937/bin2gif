###
# Variables
###

CXX = icpc

INSTALL_DIR = ~/bin

PWD = $(shell pwd)

# Determine OpenMP flags for compiler
ifeq ($(shell $(CXX) --version 2>&1 | grep Intel | sed -e 's/.*Intel.*/Intel/g'),Intel)
	OPENMP_FLAG = -openmp
	OPENMP_FLAG += -openmp-link=static
else
	OPENMP_FLAG = -fopenmp
endif

LIBS = -lgd -lm

# Host specific variables
HOSTNAME = $(shell hostname)

ifeq ($(HOSTNAME),ktg1.phys.msu.ru)
	HOSTTITLE = ILC MSU cluster
	LIBS_STATIC += /usr/lib/libgd.a
else
	ifeq ($(HOSTNAME),oleg-pc)
		HOSTTITLE = Oleg home PC
	else
		HOSTTITLE = SKIF MSU cluster
		INCLUDES += -I/home/$(USER)/local/include
		LIBS += -L/home/$(USER)/local/lib
		LIBS_STATIC += /home/$(USER)/local/lib/libgd.a
	endif
endif

# Information messages
MSG_BUILD = Build on $(HOSTTITLE), $(USER)@$(HOSTNAME)
MSG_INSTALL = Install on $(HOSTTITLE), $(USER)@$(HOSTNAME)


###
# Targets
###

all: bin2gif bin2gif-static

bin2gif: main.o util_visualize.o util_fs.o
	@echo $(MSG_BUILD)
	$(CXX) ./*.o -o ./bin2gif $(OPENMP_FLAG) $(LIBS) $(CFLAGS)

bin2gif-static: main.o util_visualize.o util_fs.o
	@echo $(MSG_BUILD)
	$(CXX) ./*.o -o ./bin2gif-static $(OPENMP_FLAG) $(LIBS) $(LIBS_STATIC) $(CFLAGS)

main.o: ./src/main.cpp ./src/parameters.h
	$(CXX) -c ./src/main.cpp $(OPENMP_FLAG) $(INCLUDES) $(CFLAGS)

util_visualize.o: ./src/util_visualize.cpp ./src/util_visualize.h
	$(CXX) -c ./src/util_visualize.cpp $(OPENMP_FLAG) $(INCLUDES) $(CFLAGS)

util_fs.o: ./src/util_fs.cpp ./src/util_fs.h
	$(CXX) -c ./src/util_fs.cpp $(OPENMP_FLAG) $(INCLUDES) $(CFLAGS)

clean:
	rm -f ./bin2gif
	rm -f ./bin2gif-static
	rm -f ./*.o

clean-pbs:
	rm -f ./*.rep-*
	rm -f ./*.out-*
	rm -f ./machinefile-*
	rm -f ./.cleo-*
	rm -f ./.panfs.*

link: bin2gif bin2gif-static uninstall
	@echo $(MSG_INSTALL)
	mkdir -p $(INSTALL_DIR)
	ln -s $(PWD)/bin2gif $(INSTALL_DIR)/
	ln -s $(PWD)/bin2gif-static $(INSTALL_DIR)/

install: bin2gif bin2gif-static uninstall
	@echo $(MSG_INSTALL)
	mkdir -p $(INSTALL_DIR)
	cp $(PWD)/bin2gif $(INSTALL_DIR)/
	cp $(PWD)/bin2gif-static $(INSTALL_DIR)/

uninstall:
	rm -f $(INSTALL_DIR)/bin2gif
	rm -f $(INSTALL_DIR)/bin2gif-static

make_test_files: ./tests/tests.cpp
	$(CXX) ./tests/tests.cpp -o ./tests/make_test_files -openmp

test: all make_test_files
	@./tests/make_test_files
	@echo ""
	@./bin2gif ./tests/*.dbl ./tests/*.cpl
	@./bin2gif-static --force ./tests/*.dbl ./tests/*.cpl

