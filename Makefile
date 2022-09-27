build:
	g++ rtree.cpp -o rtree.out -g
run: build
	./rtree.out
clean:
	rm rtree.out sfml-app

build_gui:
	g++ -std=c++11 main.cpp -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
run_gui: build_gui
	./sfml-app

test_tree:
	g++ -std=c++11 nada.cpp -o test.o
	./test.o

debug_tree:
	g++ -std=c++11 -g nada.cpp -o debug.o