main:
	echo Compiling graphics ...
	$(CC) --version
	$(CC) `pkg-config --cflags gtk+-3.0 gtkmm-3.0` -o graphics main.cpp `pkg-config --libs gtk+-3.0 gtkmm-3.0` -rdynamic -lstdc++ -std=c++11 -lm

