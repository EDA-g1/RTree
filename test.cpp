#include <iostream>
#include <fstream>
#include <vector>
#include "rtree.h"

void test1() {
    RTree rt;


    rt.insert_spatialobj(new Point(192, 1020), Status::point);
    rt.insert_spatialobj(new Point(166, 414), Status::point);
    rt.insert_spatialobj(new Point(708, 912), Status::point);
    rt.insert_spatialobj(new Point(510, 492), Status::point);
    rt.insert_spatialobj(new Point(1218, 1106), Status::point);
    rt.insert_spatialobj(new Point(1088, 210), Status::point);
    rt.insert_spatialobj(new Point(848, 736), Status::point);

    rt.show_rtree();

    auto knn = rt.df_knn(new Point(714, 1228), 3);
    reverse(knn.begin(), knn.end());
    auto knn2 = rt.knn(new Point(714, 1228), 3);

    for (auto val: knn) val->obj->display();
    cout << endl;
    for (auto val: knn2) val->obj->display();
}

void test2() {
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
    auto knn2 = rt.knn(new Point(448, 760), 3);

    for (auto val: knn) val->obj->display();
    cout << endl;
    for (auto val: knn2) val->obj->display();
    cout << endl;
}

void test3() {
    RTree rt;


    rt.insert_spatialobj(new Point(592, 1180), Status::point);
    rt.insert_spatialobj(new Point(284, 830), Status::point);
    rt.insert_spatialobj(new Point(1028, 772), Status::point);
    rt.insert_spatialobj(new Point(528, 328), Status::point);
    rt.insert_spatialobj(new Point(1026, 1120), Status::point);
    rt.insert_spatialobj(new Point(1092, 324), Status::point);
    rt.insert_spatialobj(new Point(712, 638), Status::point);
    rt.insert_spatialobj(new Point(816, 1026), Status::point);

    rt.show_rtree();

    Point* pn = new Point(1272, 140);
    auto knn = rt.df_knn(pn, 3);
    auto knn2 = rt.knn(pn, 3);

    for (auto val: knn) val->obj->display();
    cout << endl;
    for (auto val: knn2) val->obj->display();
    cout << endl;
}

int main() {
    test3();
}