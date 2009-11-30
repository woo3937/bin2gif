PFILES = ./src/main.cpp ./src/util_visualize.cpp ./src/util_fs.cpp
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
		LIBS_STATIC += /usr/lib/libgd.a
	else
		HOSTTITLE = SKIF MSU cluster
		INCLUDE_DIRS = -I/home/$(USER)/local/include -L/home/$(USER)/local/lib
		LIBS_STATIC += /home/$(USER)/local/lib/libgd.a
	endif
endif

# Information messages

MSG_COMPILE = Compile on $(HOSTTITLE), $(USER)@$(HOSTNAME)
MSG_INSTALL = Install on $(HOSTTITLE), $(USER)@$(HOSTNAME)


all: bin2gif

bin2gif: $(PFILES)
	@echo $(MSG_COMPILE)
	$(CXX) $(PFILES) -o ./bin2gif $(INCLUDE_DIRS) $(LIBS) $(CFLAGS)

bin2gif-static: $(PFILES)
	@echo $(MSG_COMPILE)
	$(CXX) $(PFILES) -o ./bin2gif-static $(INCLUDE_DIRS) $(LIBS_DIRS) $(LIBS_STATIC) $(CFLAGS)

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

tests: bin2gif make_test_files
	@./tests/make_test_files
	@echo ""
	@./bin2gif ./tests/*.bin
