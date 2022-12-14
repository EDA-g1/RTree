#include <iostream>
#include <queue>
#include <utility>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#define M 22 
#define m 11 
#define heuristica 0 




using namespace std;


enum class Status {mbb,leaf_mbb,point,polygon};

struct SpatialObj {
    virtual int getLowX() = 0;
    virtual int getHighX() = 0;
    virtual int getLowY() = 0;
    virtual int getHighY() = 0;

    virtual void display() = 0;

    double intersection(SpatialObj* other){
        double base = max(min(getHighX(),other->getHighX()) - max(getLowX(),other->getLowX()) ,0);
        double height = max(min(getHighY(),other->getHighY()) - max(getLowY(),other->getLowY()) ,0);

        return base*height;
    }


    double getDistanceTo(SpatialObj* other) {
        int delta_x = max(max(getLowX(),other->getLowX()) - min(getHighX(),other->getHighX()) ,0);
        int delta_y = max(max(getLowY(),other->getLowY()) - min(getHighY(),other->getHighY()) ,0);
        // int delta_y = min(
        //         abs(this->getHighY() - obj->getLowY()),
        //         abs(this->getLowY() - obj->getHighY())
        // );

        return sqrt(pow(delta_x,2)+ pow(delta_y,2));
    }




};

struct Point : public SpatialObj{
    int x;
    int y;

    Point(int _x, int _y) {
        x = _x;
        y = _y;
    }

    void display() override {
        cout << "POINT: (" << x << "," << y << ") ";
    }

    int getHighX() override {
        return x;
    }
    int getHighY() override {
        return y;
    }
    int getLowX() override {
        return x;
    }
    int getLowY() override {
        return y;
    }
};

struct MBB : public SpatialObj{
    Point low;
    Point high;

    MBB():
        low{numeric_limits<int>::max(), numeric_limits<int>::max()},
        high{0,0}
        {}

    MBB(Point _low, Point _high):
        low{std::move(_low)},
        high{std::move(_high)}
        {}

    double getArea() const {
        return (high.x-low.x) * (high.y-low.y);
    }

    double getPerimeter() const {
        return 2*((high.x-low.x) + (high.y-low.y));
    }

    void display() override {
        cout << "MBB: { (" << getLowX() << "," << getLowY() << ") , (" << getHighX() << "," << getHighY() << ") }";
    }

    int getHighX() override {
        return high.x;
    }
    int getHighY() override {
        return high.y;
    }
    int getLowX() override {
        return low.x;
    }
    int getLowY() override {
        return low.y;
    }

    Point mid_point(){
        int x = (getHighX() + getLowX())/2;
        int y = (getHighY() + getLowY())/2;

        return Point(x,y);

    }

    // EXPANSION DE PERIMETRO
    double requiredMBBIncrease(SpatialObj* s) const {
        MBB new_mbb(
                Point(
                        min(s->getLowX(), this->low.x),
                        min(s->getLowY(), this->low.y)
                ),
                Point(
                        max(s->getHighX(), this->high.x),
                        max(s->getHighY(), this->high.y)
                )
        );
        
        
        int old_perimeter = 2*(this->high.x - this->low.x) + 2*(this->high.y - this->low.y) ;
        int new_perimeter = 2*(new_mbb.high.x - new_mbb.low.x) + 2*(new_mbb.high.y - new_mbb.low.y) ;

        double new_h = new_perimeter - old_perimeter;

        //heuristic for box expansion
        return  new_h;
    }

    // EXPANSION DE AREA
    double requiredMBBIncreaseArea(SpatialObj* s) const {
        MBB new_mbb(
                Point(
                        min(s->getLowX(), this->low.x),
                        min(s->getLowY(), this->low.y)
                ),
                Point(
                        max(s->getHighX(), this->high.x),
                        max(s->getHighY(), this->high.y)
                )
        );


        int old_perimeter = 2*(this->high.x - this->low.x) + 2*(this->high.y - this->low.y) ;
        int new_perimeter = 2*(new_mbb.high.x - new_mbb.low.x) + 2*(new_mbb.high.y - new_mbb.low.y) ;


        double area_h = new_mbb.getArea()-this->getArea();

        return  area_h;
    }



    // EXAPANDIR PARA INCLUIR UN OBJETO
    void expandMBB(SpatialObj* s) {
        this->low.x = min(s->getLowX(), this->low.x);
        this->high.x = max(s->getHighX(), this->high.x);
        this->low.y = min(s->getLowY(), this->low.y);
        this->high.y = max(s->getHighY(), this->high.y);
    }



    Point center(){
        return Point((getHighX() - getLowX())/2,(getHighY() - getLowY())/2);

    }
    

    
};

struct Polygon : public SpatialObj{
    vector<Point> points;
    MBB box{{0,0},{0,0}};

    Polygon(vector<Point> _points) {
        // Set points
        points = _points;
        // Get MBB
        int min_x = numeric_limits<int>::max();
        int min_y = numeric_limits<int>::max();
        int max_x = numeric_limits<int>::min();
        int max_y = numeric_limits<int>::min();
        for (const auto &p : points) {
            if (p.x < min_x) {min_x = p.x;}
            if (p.y < min_y) {min_y = p.y;}
            if (p.x > max_x) {max_x = p.x;}
            if (p.y > max_y) {max_y = p.y;}
        }
        // Set MBB
        box = MBB(Point(min_x, min_y), Point(max_x, max_y));
    }

    void display() override {
        cout << "POLYGON: { ";
        for (const auto& p : points) {
            cout << "(" << p.x << "," << p.y << ") ";
        }
        cout << "}";
        // Print MBB
        cout << " #"; box.display();
    }

    int getHighX() override {
        return box.getHighX();
    }
    int getHighY() override {
        return box.getHighY();
    }
    int getLowX() override {
        return box.getLowX();
    }
    int getLowY() override {
        return box.getLowY();
    }


    Point center(){
        return Point((getHighX() - getLowX())/2,(getHighY() - getLowY())/2);

    }
};


struct Limits{
    std::size_t begin, end;
};

struct Region{
    Limits x, y;
};

Region getLimits(SpatialObj* mbb, std::vector<double>& xValues, std::vector<double>& yValues){
    Region result;
    for(std::size_t i = 0; i != xValues.size(); ++i){
        if(xValues[i] == mbb->getLowX())
            result.x.begin = i;
        else if(xValues[i] == mbb->getHighX())
            result.x.end = i;
    }
    for(std::size_t i = 0; i != yValues.size(); ++i){
        if(yValues[i] == mbb->getLowY())
            result.y.begin = i;
        else if(yValues[i] == mbb->getHighY())
            result.y.end = i;
    }
    return result;
}
