main:
	echo Compiling graphics ...
	$(CC) --version
	$(CC) `pkg-config --cflags gtk+-3.0 gtkmm-3.0` -o graphics main.cpp `pkg-config --libs gtk+-3.0 gtkmm-3.0` -rdynamic -lstdc++ -std=c++11 -lm

test:
	echo Running unit tests...
	$(foreach test_file,$(subst .cc,,$(shell find $(APP) -name \*_tests.cpp -printf "%f\n")),$(CC) `pkg-config --cflags gtk+-3.0 gtkmm-3.0` -o $(test_file).exe $(test_file) `pkg-config --libs gtk+-3.0 gtkmm-3.0` -rdynamic -lstdc++ -std=c++11 -lm;)

