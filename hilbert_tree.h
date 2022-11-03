#include "spatial_objects.h"




// HilbertNode

struct HilbertNode{
    HilbertNode* parent= nullptr;
    vector<HilbertNode*> children;
    SpatialObj* obj= nullptr;
    bool is_root=false;
    Status status;
    int hindex;

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

    void updateMBB(HilbertNode* u) const {
        MBB* mbb = (MBB*) obj;
        mbb->low.x = min(u->obj->getLowX(), mbb->low.x);
        mbb->high.x = max(u->obj->getHighX(), mbb->high.x);
        mbb->low.y = min(u->obj->getLowY(), mbb->low.y);
        mbb->high.y = max(u->obj->getHighY(), mbb->high.y);

        if(parent){
            parent->updateMBB(u);
        }
    }

    void set_as_children(HilbertNode* u) {
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

    // bool operator==(HilbertNode* other) {
    //     return ((this->obj->getLowX() == other->obj->getLowX())
    //             && (this->obj->getLowY() == other->obj->getLowY())
    //             && (this->obj->getHighX() == other->obj->getHighX())
    //             && (this->obj->getHighY() == other->obj->getHighY())
    //             );
    // }


    void sort_nodes(){
        sort(children.begin(),children.end(),[](HilbertNode* a, HilbertNode* b){
            return a->hindex <= b->hindex;
        });
        hindex = children.back()->hindex;
    }
};



void show(HilbertNode* node, string prefix){
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


int hindex(int x, int y, int k){
    // (x,y) = coords
    // k = numero de bits para el orden actual

    // Exit condition
    if (k==0) {
        return 0;
    }

    // Variable
    int size = 1 << (k/2); // size = 2 pow (k/2)
    int bits;
    auto next_k = k-2;
    auto next_size = size/2;
    int next_x;
    int next_y;

    // QUAD 0 (SW)
    if (x < size/2 && y < size/2) {
        next_x = y;
        next_y = x;
        bits = 0; //00
    }
    // QUAD 1 (NW)
    else if (x < size/2 && y >= size/2) {
        next_x = x;
        next_y = y-(size/2);
        bits = 1; //01
    }
    // QUAD 2 (NE)
    else if (x >= size/2 && y >= size/2) {
        next_x = x-(size/2);
        next_y = y-(size/2);
        bits = 2; //10
    }
    // QUAD 3 (SE)
    else { // (x >= size/2 && y < size/2)
        next_x = (size/2-1)-y;
        next_y = (size-1)-x;
        bits = 3; //11
    }
    return (bits << next_k) + hindex(next_x, next_y, next_k);
}




class HB_Tree{
    HilbertNode* root;


    HB_Tree(){
        root = new HilbertNode();
        root->is_root = true;
        root->status = Status::leaf_mbb;
    }



    HilbertNode* choose_leaf(HilbertNode* actual, int h_index){

        if(actual->status == Status::leaf_mbb){
            return actual;
        }

        for(int i = 0; i < actual->children.size();++i){
            if(h_index > actual->children[i]->hindex)
                return choose_leaf(actual->children[i],h_index);
        }

        //4
        //1 2 3 


        return choose_leaf(actual->children.back(),h_index);

    }



    vector<HilbertNode*> get_S(HilbertNode*N){

        vector<HilbertNode*> E;
        HilbertNode* parent_N = N->parent;

        for(auto& c : parent_N->children){
            E.push_back(c);
            

            if(c == N){
                break;
            }
        }

        return E;
    }


    HilbertNode* handle_overflow(HilbertNode* N, HilbertNode* insert_node){
        HilbertNode* NN = nullptr;
        bool posible = 0;

        vector<HilbertNode*> E = get_S(N);

        for(auto& e : E){
            if(e->children.size() < M){
                posible = 1;
            }
        }

        N->children.push_back(insert_node);
        N->sort_nodes();

        if(!posible){
            NN = new HilbertNode();
            NN->status = N->status;
            NN->obj = new MBB(Point(0,0),Point(0,0));
            E.push_back(NN);
        }


        vector<HilbertNode*> total_children; 


        for(auto& p : E){
            for(auto & c : p->children){
                total_children.push_back(c);
            }
        }

        //M = 4
        //m = 2
        //11
        //
        //z
        //a b-- - || d e f g ||  l m n r

        int it = 0;
        int children_per_node = total_children.size()/E.size();//3
        int leftovers = total_children.size() % E.size();//2


        for(int rep = 0; rep < E.size();++rep){

            int chidren_in_node = children_per_node;

            if(leftovers){
                children_per_node++;
                leftovers--;
            }

            E[rep]->children.clear();

            for(int i ;i < children_per_node;++i){
                if(total_children[it] == insert_node)
                    insert_node->parent = E[rep];

                E[rep]->children.push_back(total_children[it]);
                ++it;
                
            }

            E[rep]->sort_nodes();
        }

        return NN;
    }


    void adjustTree(vector<HilbertNode*> S,HilbertNode* N, HilbertNode* NN){


        if(NN != nullptr){

            if(N->parent->children.size() < M){
                N->parent->children.push_back(NN);
                N->parent->sort_nodes();
            }else{

                NN = handle_overflow(N->parent,NN);

            }
           

        }



    }

    void insert(SpatialObj* new_obj, Status s,int h_index ){

        HilbertNode* L = choose_leaf(root,h_index);
        HilbertNode* obj_wrapper = new HilbertNode();
        HilbertNode* NN = nullptr;
        vector<HilbertNode*> S;
        obj_wrapper->obj = new_obj;
        obj_wrapper->status = s;
        obj_wrapper->hindex = h_index;
        
        if(L->children.size() < M){
            L->children.push_back(obj_wrapper);
            obj_wrapper->parent = L;
            adjustTree(S,L,NN);
        }else{
            NN = handle_overflow(L,obj_wrapper);
            S = get_S(L);
            adjustTree(S,L,NN);
        }
        





    }


    public:



};



