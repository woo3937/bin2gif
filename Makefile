CXX = icpc
CFLAGS = -openmp
LIBS = -lgd -lm

# For warning with feupdateenv in libimf.so
LIBS += -shared-intel

LIBS_STATIC = -lm -openmp-link=static

INSTALL_DIR_HOME = ~/bin

HOSTNAME = $(shell hostname)

ifeq ($(HOSTNAME),ktg1.phys.msu.ru)
	HOSTTITLE = ILC MSU cluster
	LIBS_STATIC += /usr/lib/libgd.a
else
	ifeq ($(HOSTNAME),oleg-pc)
		HOSTTITLE = Oleg home PC
		#LIBS_STATIC += /usr/lib/libgd.a
		LIBS_STATIC += -lgd
	else
		HOSTTITLE = SKIF MSU cluster
		INCLUDE_DIRS = -I/home/$(USER)/local/include -L/home/$(USER)/local/lib
		LIBS_STATIC += /home/$(USER)/local/lib/libgd.a
	endif
endif

# Information messages

MSG_BUILD = Build on $(HOSTTITLE), $(USER)@$(HOSTNAME)
MSG_INSTALL = Install on $(HOSTTITLE), $(USER)@$(HOSTNAME)


all: bin2gif bin2gif-static
	rm -f ./*.o

bin2gif: compile-main compile-util_visualize compile-util_fs
	@echo $(MSG_BUILD)
	$(CXX) ./*.o -o ./bin2gif $(INCLUDE_DIRS) $(LIBS) $(CFLAGS)

bin2gif-static: compile-main compile-util_visualize compile-util_fs
	@echo $(MSG_BUILD)
	$(CXX) ./*.o -o ./bin2gif-static $(INCLUDE_DIRS) $(LIBS_DIRS) $(LIBS_STATIC) $(CFLAGS)

compile-main: ./src/main.cpp ./src/parameters.h
	$(CXX) -c ./src/main.cpp -o ./main.o $(INCLUDE_DIRS) $(CFLAGS)

compile-util_visualize: ./src/util_visualize.cpp
	$(CXX) -c ./src/util_visualize.cpp -o ./util_visualize.o $(INCLUDE_DIRS) $(CFLAGS)

compile-util_fs: ./src/util_fs.cpp
	$(CXX) -c ./src/util_fs.cpp -o ./util_fs.o $(INCLUDE_DIRS) $(CFLAGS)

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

install: bin2gif
	@echo $(MSG_INSTALL)
	cp ./bin2gif $(INSTALL_DIR_HOME)/

uninstall:
	rm -f $(INSTALL_DIR_HOME)/bin2gif

make_test_files: ./tests/tests.cpp
	$(CXX) ./tests/tests.cpp -o ./tests/make_test_files -openmp

test: all make_test_files
	@./tests/make_test_files
	@echo ""
	@./bin2gif ./tests/*.bin
	@./bin2gif-static --force ./tests/*.bin

