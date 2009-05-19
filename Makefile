PFILES = ./src/main.cpp ./src/util_visualize.cpp ./src/util_fs.cpp
CXX = icpc
CFLAGS = -openmp
LIBS = -lgd -lm
LIBS_STATIC = -lm -openmp-link=static


HOSTNAME = $(shell hostname)
ifeq ($(HOSTNAME),ktg1.phys.msu.su)
    INSTALL_DIR_HOME = ~/bin
    INSTALL_DIR_USR = /usr/bin
    INCLUDE_DIRS =
    LIBS_STATIC += /usr/lib/libgd.a
    MSG_COMPILE = Compile on ILC cluster
    MSG_INSTALL = Install on ILC cluster
else
    INSTALL_DIR_HOME = ~/bin
    INSTALL_DIR_USR = $(INSTALL_DIR_HOME)
    INCLUDE_DIRS = -I/home/kosareva/local/include -L/home/kosareva/local/lib
    LIBS_STATIC += /home/kosareva/local/lib/libgd.a
    MSG_COMPILE = Compile on SKIF cluster
    MSG_INSTALL = Install on SKIF cluster
endif

all: $(PFILES)
	@echo $(MSG_COMPILE)
	$(CXX) $(PFILES) -o ./bin2gif $(INCLUDE_DIRS) $(LIBS) $(CFLAGS)

static: $(PFILES)
	@echo $(MSG_COMPILE)
	$(CXX) $(PFILES) -o ./bin2gif-static $(INCLUDE_DIRS) $(LIBS_DIRS) $(LIBS_STATIC) $(CFLAGS)

clean:
	rm -f ./$(PNAME)

clean-pbs:
		rm -f ./*.rep-*
		rm -f ./*.out-*
		rm -f ./machinefile-*

install: bin2gif
	@echo $(MSG_INSTALL)
	cp ./$(PNAME) $(INSTALL_DIR_HOME)/$(PNAME)
	cp ./$(PNAME) $(INSTALL_DIR_USR)/$(PNAME)

uninstall:
	rm -f $(INSTALL_DIR_HOME)/$(PNAME)
	rm -f $(INSTALL_DIR_USR)/$(PNAME)

make_test_files: ./tests/tests.cpp
	$(CXX) ./tests/tests.cpp -o ./tests/make_test_files -openmp

tests: bin2gif make_test_files
	@./tests/make_test_files
	@echo""
	@./bin2gif ./tests/*.bin
