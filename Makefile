CC = g++ -std=c++17
DEBUGGER = gdb
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
DEBUG_FLAGS = -g
EXECUTABLE = sfml-app
TARGETS = ./main.cpp

run: build
	./$(EXECUTABLE)
build:
	$(CC) $(TARGETS) -o $(EXECUTABLE) $(SFML_FLAGS) -I C:\mingw32\include
debug: debug_build
	$(DEBUGGER) $(EXECUTABLE)
debug_build:
	$(CC) $(TARGETS) -o $(EXECUTABLE) $(SFML_FLAGS) $(DEBUG_FLAGS)
clean:
	$(RM) $(EXECUTABLE)

#build:
#	g++ rtree.cpp -o rtree.out -g
#run: build
#	./rtree.out
#clean:
#	rm rtree.out sfml-app


