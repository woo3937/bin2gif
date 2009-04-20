PNAME = bin2gif
PFILES = ./src/main.cpp ./src/util_visualize.cpp ./src/util_fs.cpp
CXX = icpc
LIBS = -lgd -lm -openmp


HOSTNAME = $(shell hostname)
ifeq ($(HOSTNAME),ktg1.phys.msu.su)
    INSTALL_DIR_HOME = ~/bin
    INSTALL_DIR_USR = /usr/bin
    INCLUDE_DIRS =
    MSG_COMPILE = Compile on ILC cluster
    MSG_INSTALL = Install on ILC cluster
else
    INSTALL_DIR_HOME = ~/bin
    INSTALL_DIR_USR = $(INSTALL_DIR_HOME)
    INCLUDE_DIRS = -I/home/kosareva/local/include -L/home/kosareva/local/lib
    MSG_COMPILE = Compile on SKIF cluster
    MSG_INSTALL = Install on SKIF cluster
endif

all: bin2gif


bin2gif: $(PFILES)
	@echo $(MSG_COMPILE)
	$(CXX) $(PFILES) -o ./$(PNAME) $(INCLUDE_DIRS) $(LIBS)

clean: clean_bin2gif

clean_bin2gif:
	rm -f ./$(PNAME)

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
