main:
	echo Compiling graphics ...
	$(CC) --version
	$(CC) `pkg-config --cflags gtk+-3.0 gtkmm-3.0` -o graphics main.cpp `pkg-config --libs gtk+-3.0 gtkmm-3.0` -rdynamic -lstdc++ -std=c++11 -lm -Werror -Wall -Wextra -Wno-non-virtual-dtor -Wno-padded -Wno-old-style-cast -Wno-unknown-pragmas -Wno-type-limits -Wno-pragmas -Wno-return-type

test:
	echo Running unit tests...
	$(foreach test_file,$(subst .cpp,,$(shell find ./tests -name \*_tests.cpp -printf "%f\n")),$(CC) `pkg-config --cflags gtk+-3.0 gtkmm-3.0` -o ./tests/$(test_file) ./tests/$(test_file).cpp ./tests/min_unit.cpp ./tests/timer.cpp `pkg-config --libs gtk+-3.0 gtkmm-3.0` -rdynamic -lstdc++ -std=c++11 -lm -Werror -Wall -Wextra -Wno-non-virtual-dtor -Wno-padded -Wno-old-style-cast -Wno-unknown-pragmas -Wno-type-limits -Wno-pragmas -Wno-return-type || exit;)
	$(foreach test_file,$(subst .cpp,,$(shell find ./tests -name \*_tests.cpp -printf "%f\n")),./tests/$(test_file) || exit;)

