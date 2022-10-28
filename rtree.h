#include "spatial_objects.h"
#include <stack>
// Node
struct Node;





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

    void adjustMBB() {
        MBB* mbb = (MBB*) obj;
        int max_y = 0, max_x = 0, min_y = numeric_limits<int>::max(), min_x = numeric_limits<int>::max();

        for (auto &c: this->children) {
            max_y = max(c->obj->getHighY(), max_y);
            max_x = max(c->obj->getHighX(), max_x);
            min_x = min(c->obj->getLowX(), min_x);
            min_y = min(c->obj->getLowY(), min_y);
        }

        mbb->low.x = max(mbb->low.x, min_x);
        mbb->low.y = max(mbb->low.y, min_y);
        mbb->high.x = min(mbb->high.x, max_x);
        mbb->high.y = min(mbb->high.y, max_y);
    }

    void updateMBB(Node* u) const {
        MBB* mbb = (MBB*) obj;
        mbb->low.x = min(u->obj->getLowX(), mbb->low.x);
        mbb->high.x = max(u->obj->getHighX(), mbb->high.x);
        mbb->low.y = min(u->obj->getLowY(), mbb->low.y);
        mbb->high.y = max(u->obj->getHighY(), mbb->high.y);

        if(parent){
            parent->updateMBB(u);
        }
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
        }else{
            updateMBB(u);
        }
        // Update parent MBB
        // if (parent) {
        //     parent->updateMBB(this);
        // }
        // Insert
        this->children.push_back(u);
    }

    bool operator==(Node* other) {
        return ((this->obj->getLowX() == other->obj->getLowX())
                && (this->obj->getLowY() == other->obj->getLowY())
                && (this->obj->getHighX() == other->obj->getHighX())
                && (this->obj->getHighY() == other->obj->getHighY())
                );
    }
};




void printNode(vector<Node*> v); 
Node * insert(Node*& u, SpatialObj* p,Status _status);
Node* handle_overflow(Node*& u);
Node* split(Node*& u);
Node* choose_subtree(Node*& u,SpatialObj* p);

vector<Node*> remove(SpatialObj* p);
vector<Node*> condenseTree(Node* u);
void getLeaves(vector<Node*> &v, Node* &u);
bool contains(SpatialObj* u, SpatialObj* p);
void eraseNode(vector<Node*> &v, Node* &n);
bool eraseObject(vector<Node*> &v, SpatialObj* p);


void printNode(vector<Node*> v) {
    for (auto &node: v) {
        node->obj->display();
    }
    cout << endl;
}



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



//orden 2 => n = 4


void eraseNode(vector<Node*> &v, Node* &n) {
        int pos = -1;
        for (int i = 0; i < v.size(); i++) {
            if (v[i] == n) pos = i;
        }
        v.erase(v.begin() + pos);
}


bool eraseObject(vector<Node*> &v, SpatialObj* p) {
    int pos = -1;
    for (int i = 0; i < v.size(); i++) {
        // v[i]->obj->display();
        // cout<<endl;
        // encontrar primer nodo contenido por el objeto p 
        if(v[i]->obj == p){ 
                pos = i;
                break;
        }
        
    }
    if (pos == -1) return false;

    v.erase(v.begin() + pos);
    delete p;
    return true;
}

void getLeaves(vector<Node*> &q, Node* &u) {
    if (u->status == Status::leaf_mbb) {
        // agregar nodos por reinseratar a vector Q
        for (auto &p: u->children) q.push_back(p);
    } else {
        for (auto &c: u->children) getLeaves(q, c);
    }
}

bool contains(SpatialObj* obj, SpatialObj* other) {
    // ver si un nodo contiene al spatial object
    // confirmar que significa contener un objeto
    return ((obj->getLowX() <= other->getLowX() && other->getLowX() <= obj->getHighX())
            && (obj->getLowX() <= other->getHighX() && other->getHighX() <= obj->getHighX())
            && (obj->getLowY() <= other->getLowY() && other->getLowY() <= obj->getHighY())
            && (obj->getLowY() <= other->getHighY() && other->getHighY() <= obj->getHighY()));
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



Node* split(Node*& u){
    // Elegir las dos semillas tal que esten lo más separadas posible
    Node* sem_a = nullptr;
    Node* sem_b = nullptr;
    double max_d = numeric_limits<double>::lowest();

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
        //show_rtree();
    }



    void show_rtree() {
        show(root,"");
    }


    Node* get_root(){
        return root;
    }

    vector<Node*> knn(SpatialObj* source, int n){
        if(root->obj == nullptr)
            return vector<Node*>{};

        priority_queue<pair<double,Node*>> pq;
        pq.push({-source->getDistanceTo(root->obj),root});
        vector<Node*> result;

        // int iter = 0;
        while(!pq.empty()) {
            pair<double,Node*> front = pq.top();
            pq.pop();
            // cout<<"iter: "<<iter<<endl;
            // ++iter;
            // front.second->obj->display();
            // cout<<" "<<front.first<<endl;
            if(front.second->status == Status::polygon || front.second->status == Status::point){
                result.push_back(front.second);
                // cout<<front.first<<endl;
                if(result.size() == n)
                    break;
 
            }else{
                // cout<<"adding childs: "<<endl;
                for(auto&c : front.second->children){
                    pq.push({-source->getDistanceTo(c->obj),c});
                    // cout<<(-source->getDistanceTo(c->obj))<<" ";
                    // c->obj->display();
                    // cout<<endl;
                    
                }
                // cout<<"finished"<<endl;
            }
            
        }
        return result;
    }


    vector<Node*> condenseTree(Node* u) {
        Node* n = u;
        vector<Node*> Q; // vector con puntos a reinsertar
        while(!n->is_root) {
            if (n->children.size() < m) {
                getLeaves(Q, n); // conseguir nodos a reinsertar

                Node* p = n->parent;
                eraseNode(p->children, n); // eliminar nodo
            }
            n->adjustMBB(); // ajustar mbb con nuevos limites
            n = n->parent;
        }

        n->adjustMBB(); // ajustar raiz
        if(root->status == Status::mbb && root->children.size() == 1){
            Node* temp = root;
            root = root->children[0];
            root->is_root = 1;
            root->parent = nullptr;
            delete temp;

        }
        return Q;
    }



    void remove_spatialobj(SpatialObj* click,SpatialObj* click_box ) {
        if(root->obj == nullptr || (root->status == Status::leaf_mbb && root->children.size() == 0))
            return;
        // eliminar punto y reinsertar nodos que hicieron underflow
        Node* supposed_node_to_delete = knn(click,1)[0];
        // cout<<"resultado"<<endl;
        // supposed_node_to_delete->obj->display();
        // cout<<endl;


        if((supposed_node_to_delete->status == Status::polygon &&supposed_node_to_delete->obj->intersection(click_box) > 0) ||
            (supposed_node_to_delete->status == Status::point && contains(click_box,supposed_node_to_delete->obj))){ 

            eraseObject(supposed_node_to_delete->parent->children,supposed_node_to_delete->obj);


            vector<Node*> Q = condenseTree(supposed_node_to_delete->parent);
            for (auto &node: Q) {
                    insert_spatialobj(node->obj, node->status);
            }
        }




   }


   vector<Node*> df_knn(SpatialObj* source,int n){
        priority_queue<pair<double,Node*> > pq;
        stack<Node*> s;
        // pq.push({-source->getDistanceTo(root->obj),root});
        s.push(root);



        while(!s.empty()){

            Node* actual = s.top();
            s.pop();


            for(auto&c:actual->children){

                int delta_x = (actual->obj->getHighX() + actual->obj->getLowX())/2;
                int delta_y = (actual->obj->getHighY() + actual->obj->getLowY())/2;

                Point* mp = new Point(delta_x,delta_y);


                if(actual->status == Status::leaf_mbb){
                    if (pq.size() < n) {
                        pq.push({source->getDistanceTo(c->obj), c});
                    } else {
                        auto [Dk,peor] = pq.top();
                        if(Dk + c->obj->getDistanceTo(mp) > source->getDistanceTo(mp)){
                            pq.pop();
                            pq.push({source->getDistanceTo(c->obj),c});
                        } 
                        else if (Dk + source->getDistanceTo(mp) > c->obj->getDistanceTo(mp)) {
                            pq.pop();
                            pq.push({source->getDistanceTo(c->obj), c});
                        } 
                    }
                } else {

                    if (pq.size() < n) {
                        s.push(c);
                    } else {
                        auto [Dk,peor] = pq.top();

                        int vx = (actual->obj->getHighX() - actual->obj->getLowX());
                        int vy = (actual->obj->getHighY() - actual->obj->getLowY());
                        int r = sqrt(vx*vx + vy*vy)/2;


                        double rmin = numeric_limits<int>::max();
                        if (c->status == Status::leaf_mbb) {
                            for (auto point: c->children) {
                                rmin = min(rmin, point->obj->getDistanceTo(mp));
                            }
                        } else {
                            for (auto box: c->children) {
                                int dx = (box->obj->getHighX() + box->obj->getLowX())/2;
                                int dy = (box->obj->getHighY() + box->obj->getLowY())/2;

                                Point* temp_mp = new Point(dx, dy);


                                int vx_2 = (box->obj->getHighX() - box->obj->getLowX());
                                int vy_2 = (box->obj->getHighY() - box->obj->getLowY());
                                int r_2 = sqrt(vx_2*vx_2 + vy_2*vy_2)/2;

                                double diff = mp->getDistanceTo(temp_mp) - r_2;
                                rmin = min(rmin, diff);
                            }
                        }

                        if(Dk + r > source->getDistanceTo(mp)) {
                            s.push(c);
                        } 
                        else if (Dk + source->getDistanceTo(mp) > rmin) {
                            s.push(c);
                        }
                    }

                }

                delete mp;

            }

        }

        vector<Node*> result;

        while(!pq.empty()){
            result.push_back(pq.top().second);
            pq.pop();
        }

        return result;  
   }

};
