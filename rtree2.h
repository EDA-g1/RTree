#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#define M 4 
#define m 2 
#define heuristica 0 


using namespace std;

// Node
struct Node;
// SpatialObj
struct SpatialObj;
// SpatialObj implementations
struct Point;
struct Polygon;
struct MBB;

enum class Status {mbb,leaf_mbb,point,polygon};


struct SpatialObj {
    virtual int getLowX() = 0;
    virtual int getHighX() = 0;
    virtual int getLowY() = 0;
    virtual int getHighY() = 0;

    virtual void display() = 0;

    double getDistanceTo(SpatialObj* obj) {
        int delta_x = min(
                abs(this->getHighX() - obj->getLowX()),
                abs(this->getLowX() - obj->getHighX())
        );
        int delta_y = min(
                abs(this->getHighY() - obj->getLowY()),
                abs(this->getLowY() - obj->getHighY())
        );
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

    MBB(Point _low, Point _high):
        low{std::move(_low)},
        high{std::move(_high)}
        {}

    double getArea() const {
        return (high.x-low.x) * (high.y-low.y);
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

    double requiredMBBIncrease(SpatialObj* s) const {
        //TODO: REVISAR - Es respecto a area o perimetro?
        // La implementacion de aca es asumiendo que es area
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


        double old_h = new_mbb.getArea()-this->getArea();
        double new_h = new_perimeter - old_perimeter;

        return  heuristica ? old_h : new_h;
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
};

//Node
struct Node{
    Node* parent= nullptr;
    vector<Node*> children;
    SpatialObj* obj= nullptr;
    bool is_root=false;
    Status status;

    bool is_overflown() const {
        return this->children.size() > M;
    }

    void updateMBB(Node* u) const {
        MBB* mbb = (MBB*) obj;
        mbb->low.x = min(u->obj->getLowX(), mbb->low.x);
        mbb->high.x = max(u->obj->getHighX(), mbb->high.x);
        mbb->low.y = min(u->obj->getLowY(), mbb->low.y);
        mbb->high.y = max(u->obj->getHighY(), mbb->high.y);
    }

    void set_as_children(Node* u) {
        // Set as parent
        u->parent = this;
        u->is_root = false;
        // Update MBB
        if (obj == nullptr){
            // Create if none
            obj = (SpatialObj*) new MBB(
                    Point(u->obj->getLowX(), u->obj->getLowY()),
                    Point(u->obj->getHighX(), u->obj->getHighY())
            );
        }
        else {
            updateMBB(u);
        }
        // Update parent MBB
        if (parent) {
            parent->updateMBB(this);
        }
        // Insert
        this->children.push_back(u);
    }
};




Node * insert(Node*& u, SpatialObj* p,Status _status);
Node* handle_overflow(Node*& u);
Node* split(Node*& u);
Node* choose_subtree(Node*& u,SpatialObj* p);

double c ;
Node * insert(Node*& u, SpatialObj* p,Status _status){
    if(u->status == Status::leaf_mbb){
        // Add p to u
        Node* new_node = new Node;
        new_node->obj = p;
        new_node->status = _status;
        u->set_as_children(new_node);

        // If u overflows, then handle
        if(u->is_overflown()){
            return handle_overflow(u);
        }
        else {
            return nullptr;
        }
    }else{
        // Descide which subtree under u should we insert p into
        Node* v = choose_subtree(u,p);
        return insert(v,p,_status);
    }
}


Node* handle_overflow(Node*& u) {
    Node* v = split(u); // u is left, v is right
    if (u->is_root) {
        // Create new root with u and v as children
        Node* new_root = new Node;
        new_root->is_root = true;
        new_root->status = Status::mbb;
        new_root->parent = nullptr;
        // u->is_root= false;
        new_root->set_as_children(u);
        new_root->set_as_children(v);
        return new_root;
    }
    else {
        Node* w = u->parent;
        //TODO???: update MBR(u) in w ???
        w->set_as_children(v);
        // If w overflows, then handle
        if (w->is_overflown()) {
            return handle_overflow(w);
        }
        else {
            return nullptr;
        }
    }
}

Node* choose_subtree(Node*& u,SpatialObj* sobj){
    int min_p = numeric_limits<int>::max();
    Node* return_node = nullptr;
    for(auto&child: u->children ){
        MBB* box = (MBB*) child->obj;
        
        int x1 = box->low.x;
        int y1 = box->low.y;
        int x2 = box->high.x;
        int y2 = box->high.y;

        int x1_new = sobj->getLowX();
        int y1_new = sobj->getLowY();
        int x2_new = sobj->getHighX();
        int y2_new = sobj->getHighY();

        // Incremento en semiperimetro
        // Extension por abajo
        int delta_x_low = abs(min(x1_new - x1,0));
        int delta_y_low = abs(min(y1_new - y1,0));
        // Extension por arriba
        int delta_x_high = abs(min(x2 - x2_new ,0));
        int delta_y_high = abs(min(y2 - y2_new,0));
        // Total
        int semiperimeter_inc = delta_x_low+delta_x_high+delta_y_low+delta_y_high;
        // Update
        if(semiperimeter_inc < min_p){
            min_p = semiperimeter_inc;
            return_node = child;
        }
    }
    return return_node;
}


Node *sa,*sb;

Node* split(Node*& u){
    // Elegir las dos semillas tal que esten lo más separadas posible
    Node* sem_a = nullptr;
    Node* sem_b = nullptr;
    double max_d = numeric_limits<double>::lowest();
    /* c = 1; */
    /* sem_a->is_root = 1; */

    for (int i = 0; i < u->children.size(); ++i) {
        Node* a = u->children[i];
        for (int j = i+1; j < u->children.size(); ++j) {
            Node* b = u->children[j];
            double dist = a->obj->getDistanceTo(b->obj);
            if (dist >= max_d) {
                sem_a = a;
                sem_b = b;
                max_d = dist;
            }
        }
    }
    // Inicializar nuevos u y v
    Node* new_u = new Node;
    // new_u->is_root = false;
    new_u->status = u->status;
    new_u->parent = u->parent;
    new_u->is_root = u->is_root;
    new_u->set_as_children(sem_a);
    Node* v = new Node;
    v->is_root = false;
    v->status = u->status;
    v->set_as_children(sem_b);

    // Insertar al que menor ampliación requiera

    for (auto &child : u->children) {
        if (child != sem_a && child != sem_b) {
            double increase_u = ((MBB*)u->obj)->requiredMBBIncrease(child->obj);
            double increase_v = ((MBB*)v->obj)->requiredMBBIncrease(child->obj);
            if(new_u->children.size() >= M-m+1){
                v->set_as_children(child);
            }else if(v->children.size() >= M-m+1){
                new_u->set_as_children(child);
            }
            else if (increase_u < increase_v) {
                new_u->set_as_children(child);
            } else {
                v->set_as_children(child);
            }
        }
    }
    u->children.clear();
    // Set new u and return
    Node* tmp = u;
    u = new_u;
    // UPDATE u IN PARENT
    if (u->parent) {
        for (auto &child : u->parent->children) {
            if (child == tmp) {
                child = u;
                break;
            }
        }
    }
    delete tmp;



    return  v;
}


void show(Node* node, string prefix){
    if (node->children.empty()) {
        cout << "EMPTY TREE" << endl;
        return;
    }

    cout << prefix << "NODE: {";
    cout << " (" << node->obj->getLowX() << "," << node->obj->getLowY() << ") ,";
    cout << " (" << node->obj->getHighX() << "," << node->obj->getHighY() << ") ";
    cout << "[children = " << node->children.size() << "]" << endl;
    prefix += "\t";
    if (node->status == Status::leaf_mbb) {
        for (auto &child: node->children) {
            // if(child->status == Status::polygon)
                // cout<<"SIIII SOY UN POLIGONO"<<endl;
            cout << prefix;
            child->obj->display();
            cout << endl;
            // << "POINT: ";
            //cout << " (" << child->obj->getLowX() << "," << child->obj->getLowY() << ") " << endl;
        }
    }
    else {
        for (auto &child: node->children) {
            show(child, prefix);
        }
    }
}

// RTree
class RTree{
private:
    Node* root;
public:
    RTree() {
        root = new Node;
        root->is_root= true;
        root->status= Status::leaf_mbb;
        root->parent= nullptr;
    }

    void insert_spatialobj(SpatialObj* sobj,Status _status) {
        Node* new_root;
        new_root = insert(root, sobj,_status);
        if (new_root) {
            root = new_root;
        }
    }

    void show_rtree() {
        show(root,"");
    }


    Node* get_root(){
        return root;
    }

};