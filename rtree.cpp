#include "rtree.h"

int main(){
    RTree rt;

    rt.insert_spatialobj(new Point(1,1),Status::point);
    rt.insert_spatialobj( new Point(1,2),Status::point);
    rt.insert_spatialobj( new Point(1,3),Status::point);
    rt.insert_spatialobj( new Point(1,4),Status::point);
    rt.insert_spatialobj( new Point(15,15),Status::point);
    // rt.insert_spatialobj( new Point(15,15));
    // rt.insert_spatialobj(new Point(50,61));
    // rt.insert_spatialobj( new Point(25,82));
    // rt.insert_spatialobj( new Point(40,32));
    // rt.insert_spatialobj( new Point(27,8));
    // rt.insert_spatialobj( new Point(10,6));
    // rt.insert_spatialobj( new Point(99,81));
    // rt.insert_spatialobj( new Point(99,82));
    // rt.insert_spatialobj( new Point(21,82));
    // rt.insert_spatialobj( new Point(20,82));
    // rt.insert_spatialobj( new Point(18,82));
    // rt.insert_spatialobj( new Point(19,82));
    // vector<Point> pol1;
    // pol1.push_back(Point(1,1));
    // pol1.push_back(Point(1,3));
    // pol1.push_back(Point(2,2));
    // rt.insert_spatialobj( new Polygon(pol1));
    rt.show_rtree();
    return 0;
}