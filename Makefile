build:
	g++ rtree.cpp -o rtree
run: build
	./rtree
clean:
	rm rtree
