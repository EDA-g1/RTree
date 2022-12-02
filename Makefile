CC = g++ -std=c++17
DEBUGGER = gdb
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
DEBUG_FLAGS = -g
EXECUTABLE = sfml-app
DEFAULT = ./rtree/main_rtree.cpp
RTREE = ./rtree/main_rtree.cpp
HILBERT = ./hilbert_tree/main_hilbert.cpp
SRTREE = ./srtree/main_star.cpp

run: build
	./$(EXECUTABLE)
build:
	$(CC) $(DEFAULT) -o $(EXECUTABLE) $(SFML_FLAGS) 

run_rtree: build_rtree
	./$(EXECUTABLE)
build_rtree:
	$(CC) $(RTREE) -o $(EXECUTABLE) $(SFML_FLAGS) 

run_hilbert: build_hilbert
	./$(EXECUTABLE)
build_hilbert:
	$(CC) $(HILBERT) -o $(EXECUTABLE) $(SFML_FLAGS) 

run_srtree: build_srtree
	./$(EXECUTABLE)
build_srtree:
	$(CC) $(SRTREE) -o $(EXECUTABLE) $(SFML_FLAGS)  -g


# debug: debug_build
# 	$(DEBUGGER) $(EXECUTABLE)
# debug_build:
# 	$(CC) $(TARGETS) -o $(EXECUTABLE) $(SFML_FLAGS) $(DEBUG_FLAGS)
# clean:
# 	$(RM) $(EXECUTABLE)

#build:
#	g++ rtree.cpp -o rtree.out -g
#run: build
#	./rtree.out
clean:
	rm *.out sfml-app


