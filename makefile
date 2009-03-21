PNAME = bin2gif
PFILES = main.cpp ./src/util_visualize.cpp ./src/util_fs.cpp
CXX = icpc
LIBS = -lgd -lm -openmp

HOSTNAME = $(shell hostname)

ifeq ($(HOSTNAME),ktg1.phys.msu.su)
    INSTALL_DIR_HOME = ~/bin
    INSTALL_DIR_USR = /usr/bin
    INCLUDES =
    MSG_COMPILE = Compile on ILC cluster
    MSG_INSTALL = Install on ILC cluster
else
    INSTALL_DIR_HOME = ~/bin
    INSTALL_DIR_USR = $(INSTALL_DIR_HOME)
    INCLUDES = -I/home/kosareva/local/include -L/home/kosareva/local/lib
    MSG_COMPILE = Compile on SKIF cluster
    MSG_INSTALL = Install on SKIF cluster
endif



all:
	@echo $(MSG_COMPILE)
	$(CXX) $(PFILES) -o ./$(PNAME) $(INCLUDES) $(LIBS)

clean:
	rm -f ./$(PNAME)

install:
	@echo $(MSG_INSTALL)
	cp ./$(PNAME) $(INSTALL_DIR_HOME)/$(PNAME)
	cp ./$(PNAME) $(INSTALL_DIR_USR)/$(PNAME)

uninstall:
	rm -f $(INSTALL_DIR_HOME)/$(PNAME)
	rm -f $(INSTALL_DIR_USR)/$(PNAME)

test:
	$(CXX) ./tests/tests.cpp -o ./tests/make_test_files -lm -openmp
	@./tests/make_test_files