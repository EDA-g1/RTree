build:
	g++ rtree.cpp -o rtree.out
run: build
	./rtree.out
clean:
	rm rtree.out
