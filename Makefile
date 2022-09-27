build:
	g++ rtree.cpp -o rtree.out -g
run: build
	./rtree.out
clean:
	rm rtree.out sfml-app

build_gui:
	g++ main.cpp -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -g
run_gui: build_gui
	./sfml-app
