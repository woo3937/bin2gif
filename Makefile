###
# Variables
###

INSTALL_DIR = ~/bin

PWD = $(shell pwd)

CFLAGS += -Wall

# Determine OpenMP flags for compiler
ifeq ($(shell $(CXX) --version 2>&1 | grep Intel | sed -e 's/.*Intel.*/Intel/g'),Intel)
	OPENMP_FLAG = -openmp
	OPENMP_FLAG += -openmp-link=static
else
	OPENMP_FLAG = -fopenmp
endif

LIBS = -lgd -lmgl -lm

# Host specific variables
HOSTNAME = $(shell hostname)

ifeq ($(HOSTNAME),ktg1.phys.msu.ru)
	HOSTTITLE = "ILC MSU cluster"
	INCLUDES += -I/opt/mathgl/include
	LIBS += -L/opt/mathgl/lib
	LIBS_STATIC += /usr/lib/libgd.a
	LIBS_STATIC += /opt/mathgl/lib/libmgl.a
else
	ifeq ($(HOSTNAME),t60-2.parallel.ru)
		HOSTTITLE = "SKIF MSU cluster"
		INCLUDES += -I/home/$(USER)/local/include
		LIBS += -L/home/$(USER)/local/lib
		LIBS_STATIC += /home/$(USER)/local/lib/libgd.a
		LIBS_STATIC += /home/$(USER)/local/lib/libmgl.a
	else
		ifeq ($(HOSTNAME),efimovov-pc)
			HOSTTITLE = "Oleg's PC"
			INCLUDES += -I/home/$(USER)/local/include
			LIBS += -L/home/$(USER)/local/lib
			LIBS_STATIC += /home/$(USER)/local/lib/libmgl.a
		else
			HOSTTITLE = "your PC"
			INCLUDES += -I/home/$(USER)/local/include
		endif
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
	$(CXX) $(OPENMP_FLAG) ./*.o -o ./bin2gif $(LIBS) $(CFLAGS)

bin2gif-static: main.o util_visualize.o util_fs.o
	@echo $(MSG_BUILD)
	$(CXX) $(OPENMP_FLAG) ./*.o -o ./bin2gif-static $(LIBS) $(LIBS_STATIC) $(CFLAGS)

main.o: ./src/main.cpp ./src/parameters.h
	$(CXX) $(OPENMP_FLAG) -c ./src/main.cpp $(INCLUDES) $(CFLAGS)

util_visualize.o: ./src/util_visualize.cpp ./src/util_visualize.h ./src/parameters.h
	$(CXX) $(OPENMP_FLAG) -c ./src/util_visualize.cpp $(INCLUDES) $(CFLAGS)

util_fs.o: ./src/util_fs.cpp ./src/util_fs.h
	$(CXX) $(OPENMP_FLAG) -c ./src/util_fs.cpp $(INCLUDES) $(CFLAGS)

clean:
	rm -f ./bin2gif
	rm -f ./bin2gif-static
	rm -f ./*.o
	rm -f ./tests/*.o

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

./tests/make_test_files: ./tests/tests.o
	$(CXX) ./tests/tests.o -o ./tests/make_test_files $(LIBS) $(CFLAGS)

./tests/tests.o: ./tests/tests.cpp
	$(CXX) -c ./tests/tests.cpp -o ./tests/tests.o $(INCLUDES) $(CFLAGS)

test: bin2gif ./tests/make_test_files
	@./tests/make_test_files
	@echo ""
	@./bin2gif --force -t double  --func real ./tests/*.dbl
	@./bin2gif --force -t complex --func norm ./tests/*.cpl
	@./bin2gif --force --axial -t double  --func real ./tests/*.adbl
	@./bin2gif --force --axial -t complex --func norm ./tests/*.acpl

lint:
	 cpplint ./src/*.cpp ./src/*.h

