#include <bits/stdc++.h>
#include "rtree.h"
#define loop(a, b) for (int a = 0; a < b; ++a)
//#define is_on_local_machine 1
using namespace std;
using ll = long long;


int main(){
	ios_base::sync_with_stdio(true);
	cin.tie(nullptr);
	cout.tie(nullptr);

	RTree tree;

	tree.insert_spatialobj(new Point(2, 0), Status::point);
	tree.insert_spatialobj(new Point(3, 0), Status::point);
	tree.insert_spatialobj(new Point(5, 5), Status::point);
	tree.insert_spatialobj(new Point(4, 4), Status::point);
	tree.insert_spatialobj(new Point(6, 4), Status::point);
	tree.insert_spatialobj(new Point(3, 1), Status::point);
	tree.show_rtree();

	tree.remove_spatialobj(new Point(5, 5));
	tree.show_rtree();

	return 0;

}
