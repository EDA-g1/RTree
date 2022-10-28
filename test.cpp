#include <iostream>
#include <fstream>
#include <vector>
#include "rtree.h"


int main() {
    RTree rt;


    rt.insert_spatialobj(new Point(1094, 360), Status::point);
    rt.insert_spatialobj(new Point(906, 542), Status::point);
    rt.insert_spatialobj(new Point(264, 1166), Status::point);
    rt.insert_spatialobj(new Point(272, 760), Status::point);
    rt.insert_spatialobj(new Point(616, 1150), Status::point);
    rt.insert_spatialobj(new Point(896, 1096), Status::point);
    rt.insert_spatialobj(new Point(304, 372), Status::point);
    rt.insert_spatialobj(new Point(982, 714), Status::point);
    rt.insert_spatialobj(new Point(692, 834), Status::point);
    rt.insert_spatialobj(new Point(632, 554), Status::point);

    rt.show_rtree();

    auto knn = rt.df_knn(new Point(448, 760), 3);
    reverse(knn.begin(), knn.end());
    auto knn2 = rt.knn(new Point(448, 760), 3);

    for (auto val: knn) val->obj->display();
    cout << endl;
    for (auto val: knn2) val->obj->display();
}