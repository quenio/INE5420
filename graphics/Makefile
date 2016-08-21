all:
	echo Compiling $(CLION_EXE_DIR)/graphics ...
	gcc `pkg-config --cflags gtk+-3.0 gtkmm-3.0` -o $(CLION_EXE_DIR)/graphics $(SOURCE_FILES) `pkg-config --libs gtk+-3.0 gtkmm-3.0` -rdynamic -lstdc++ -std=c++11 -lm

main:
	echo Compiling graphics ...
	$(CC) `pkg-config --cflags gtk+-3.0 gtkmm-3.0` -o graphics graphics.cpp `pkg-config --libs gtk+-3.0 gtkmm-3.0` -rdynamic -lstdc++ -std=c++11 -lm

ui:
	echo Compiling graphics ...
	$(CC) `pkg-config --cflags gtk+-3.0 gtkmm-3.0` -o graphics ui.cpp `pkg-config --libs gtk+-3.0 gtkmm-3.0` -rdynamic -lstdc++ -std=c++11 -lm
