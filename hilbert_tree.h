#include "spatial_objects.h"




// HilbertNode


struct HilbertNode{
    int h_index;

    void hilbert_node__set_as_children(Node* u){
        set_as_children(u);
        
    }
};


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


HilbertNode * insert_hbtree(HilbertNode*& u, SpatialObj* p,Status _status, int k){
    if(u->status == Status::leaf_mbb){
        // Add p to u
        HilbertNode* new_node = new HilbertNode;
        new_node->obj = p;
        new_node->status = _status;
        u->set_as_children(new_node);
        //ordenar hijos del nodo actual con el hindex

        sort(u->children.begin(),u->children.end(),[&](HilbertNode*a,HilbertNode*b)->bool{
            return 
                    <= ;
        });
        // If u overflows, then handle
        if(u->is_overflown()){
            return handle_overflow_hb(u);
        }
        else {
            return nullptr;
        }
    }else{
        // Descide which subtree under u should we insert p into
        // Node* v = choose_predecesor(u,p);
        // return insert(v,p,_status);
        return nullptr;
    }
}





Node* handle_overflow_hb(Node*& u) {
    Node* v = split(u); // u is left, v is right
    if (u->is_root) {
        //hacer algo
        //return new_root;
    }
    else {
        
        
    }
}






Node* choose_predecesor(Node*actual, SpatialObj* object,int bits_for_order){

    int index_object = hindex(object->getHighX(),object->getHighY(),bits_for_order);
    Node* retorno;
    for(int i = 0; i < actual->children.size() ; ++i){
        SpatialObj* actual_children_obj = actual->children[i]->obj;
        int index_actual = hindex(actual_children_obj->getHighX(),actual_children_obj->getHighY(),bits_for_order);
        if(index_actual <=  index_object){
            return actual->children[i];
        }
    }
    return actual->children.back();
}





