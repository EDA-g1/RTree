#include "spatial_objects.h"

// HilbertHilbertNode

struct HilbertNode
{
    HilbertNode *parent = nullptr;
    vector<HilbertNode *> children;
    SpatialObj *obj = nullptr;
    bool is_root = false;
    Status status;
    int hindex;

    bool is_overflown() const
    {
        return this->children.size() > M;
    }

    void adjustMBB()
    {
        MBB *mbb = (MBB *)obj;
        int max_y = 0, max_x = 0, min_y = numeric_limits<int>::max(), min_x = numeric_limits<int>::max();

        for (auto &c : this->children)
        {
            max_y = max(c->obj->getHighY(), max_y);
            max_x = max(c->obj->getHighX(), max_x);
            min_x = min(c->obj->getLowX(), min_x);
            min_y = min(c->obj->getLowY(), min_y);
        }

        mbb->low.x = min_x;
        mbb->low.y = min_y;
        mbb->high.x = max_x;
        mbb->high.y = max_y;
    }

    bool find_and_delete(HilbertNode* del){

        int pos = -1;
        for (int i = 0; i < children.size(); i++) {
            // v[i]->obj->display();
            // cout<<endl;
            // encontrar primer nodo contenido por el objeto p 
            if(children[i] == del){ 
                    pos = i;
                    break;
            }
            
        }
        if (pos == -1) return false;

        children.erase(children.begin() + pos);
        delete del->obj;
        delete del;
        return true;

    }

    void updateMBB(HilbertNode *u) const
    {
        MBB *mbb = (MBB *)obj;
        mbb->low.x = min(u->obj->getLowX(), mbb->low.x);
        mbb->high.x = max(u->obj->getHighX(), mbb->high.x);
        mbb->low.y = min(u->obj->getLowY(), mbb->low.y);
        mbb->high.y = max(u->obj->getHighY(), mbb->high.y);

        if (parent)
        {
            parent->updateMBB(u);
        }
    }

    void set_as_children(HilbertNode *u)
    {
        // Set as parent
        u->parent = this;
        u->is_root = false;
        // Update MBB
        if (obj == nullptr)
        {
            // Create if none
            obj = (SpatialObj *)new MBB(
                Point(u->obj->getLowX(), u->obj->getLowY()),
                Point(u->obj->getHighX(), u->obj->getHighY()));
        }
        else
        {
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

    void sort_nodes()
    {
        sort(children.begin(), children.end(), [](HilbertNode *a, HilbertNode *b)
             { return a->hindex <= b->hindex; });
        if(children.size()) hindex = children.back()->hindex;
    }
};

void show(HilbertNode *node, string prefix)
{
    if (node->children.empty())
    {
        cout << "EMPTY TREE" << endl;
        return;
    }

    cout << prefix << "NODE: {";
    cout << " (" << node->obj->getLowX() << "," << node->obj->getLowY() << ") ,";
    cout << " (" << node->obj->getHighX() << "," << node->obj->getHighY() << ") ";
    cout << "[children = " << node->children.size() << ", " << node->hindex << "]" << endl;
    prefix += "\t";
    if (node->status == Status::leaf_mbb)
    {
        for (auto &child : node->children)
        {
            cout << prefix;
            child->obj->display();
            cout << " h_index: " << child->hindex;
            cout << endl;
            // << "POINT: ";
            // cout << " (" << child->obj->getLowX() << "," << child->obj->getLowY() << ") " << endl;
        }
    }
    else
    {
        for (auto &child : node->children)
        {
            show(child, prefix);
        }
    }
}

int hindex(int x, int y, int k)
{
    // (x,y) = coords
    // k = numero de bits para el orden actual

    // Exit condition
    if (k == 0)
    {
        return 0;
    }

    // Variable
    int size = 1 << (k / 2); // size = 2 pow (k/2)
    int bits;
    auto next_k = k - 2;
    auto next_size = size / 2;
    int next_x;
    int next_y;

    // QUAD 0 (SW)
    if (x < size / 2 && y < size / 2)
    {
        next_x = y;
        next_y = x;
        bits = 0; // 00
    }
    // QUAD 1 (NW)
    else if (x < size / 2 && y >= size / 2)
    {
        next_x = x;
        next_y = y - (size / 2);
        bits = 1; // 01
    }
    // QUAD 2 (NE)
    else if (x >= size / 2 && y >= size / 2)
    {
        next_x = x - (size / 2);
        next_y = y - (size / 2);
        bits = 2; // 10
    }
    // QUAD 3 (SE)
    else
    { // (x >= size/2 && y < size/2)
        next_x = (size / 2 - 1) - y;
        next_y = (size - 1) - x;
        bits = 3; // 11
    }
    return (bits << next_k) + hindex(next_x, next_y, next_k);
}

bool contains(SpatialObj* obj, SpatialObj* other) {
    // ver si un nodo contiene al spatial object
    // confirmar que significa contener un objeto
    return ((obj->getLowX() <= other->getLowX() && other->getLowX() <= obj->getHighX())
            && (obj->getLowX() <= other->getHighX() && other->getHighX() <= obj->getHighX())
            && (obj->getLowY() <= other->getLowY() && other->getLowY() <= obj->getHighY())
            && (obj->getLowY() <= other->getHighY() && other->getHighY() <= obj->getHighY()));
}

class HB_Tree
{
    HilbertNode *root;

    HilbertNode *choose_leaf(HilbertNode *actual, int h_index)
    {

        if (actual->status == Status::leaf_mbb)
        {
            return actual;
        }

        for (int i = 0; i < actual->children.size(); ++i)
        {
            if (h_index <= actual->children[i]->hindex)
                return choose_leaf(actual->children[i], h_index);
        }

        // 4
        // 1 2 3

        return choose_leaf(actual->children.back(), h_index);
    }

    vector<HilbertNode *> get_S(HilbertNode *N)
    {

        vector<HilbertNode *> E;
        HilbertNode *parent_N = N->parent;

        if (parent_N)
        {
            for (auto c : parent_N->children)
            {
                E.push_back(c);

                if (c == N)
                {
                    break;
                }
            }
        }
        else
            E.push_back(N);

        return E;
    }

    // N es un padre para insert_node
    // retorno: un hermano para N

    HilbertNode *handle_overflow(HilbertNode *N, HilbertNode *insert_node)
    {
        HilbertNode *NN = nullptr;
        bool posible = 0;

        vector<HilbertNode *> hermanos_n = get_S(N);

        for (auto &e : hermanos_n)
        {
            if (e->children.size() < M)
            {
                posible = 1;
            }
        }

        N->children.push_back(insert_node);
        N->sort_nodes();

        if (!posible)
        {
            NN = new HilbertNode();
            NN->status = N->status;
            int max_int = numeric_limits<int>::max();
            NN->obj = new MBB(Point(max_int, max_int), Point(max_int, max_int));
            hermanos_n.push_back(NN);
        }

        vector<HilbertNode *> total_children;

        for (auto &p : hermanos_n)
        {
            for (auto &c : p->children)
            {
                total_children.push_back(c);
            }
        }

        int it = 0;
        int children_per_node = total_children.size() / hermanos_n.size(); // 3
        int leftovers = total_children.size() % hermanos_n.size();         // 2

        for (int rep = 0; rep < hermanos_n.size(); ++rep)
        {

            int children_in_node = children_per_node;

            if (leftovers)
            {
                children_in_node++;
                leftovers--;
            }

            hermanos_n[rep]->children.clear();

            for (int i = 0; i < children_in_node; ++i)
            {
                // insert_node->parent = hermanos_n[rep];
                hermanos_n[rep]->children.push_back(total_children[it]);
                total_children[it]->parent = hermanos_n[rep];
                ++it;
            }

            hermanos_n[rep]->sort_nodes();
            hermanos_n[rep]->adjustMBB();
        }

        return NN;
    }

    // NN es un hermano para N (estan al mismo nivel)
    void adjustTree(HilbertNode *N, HilbertNode *NN)
    {
        // status
        //
        if (N->is_root)
        {
            if (NN)
            {
                N->is_root = false;
                root = new HilbertNode;
                root->is_root = true;
                root->status = Status::mbb;
                root->obj = new MBB(Point(0, 0), Point(0, 0));
                N->parent = root;
                NN->parent = root;
                root->children.push_back(N);
                root->children.push_back(NN);
                root->sort_nodes();
                root->adjustMBB();
            }

            return;
        }

        HilbertNode *n_parent = N->parent;

        if (NN)
        {
            if (n_parent->children.size() < M)
            {
                n_parent->children.push_back(NN);
                NN->parent = n_parent;
                NN = nullptr;
            }
            else
            {
                NN = handle_overflow(n_parent, NN);
                // ahora NN es un hermano para el padre de N
            }
        }

        n_parent->adjustMBB();
        n_parent->sort_nodes();

        adjustTree(n_parent, NN);
    }

    HilbertNode* find_next_sibling(HilbertNode*parent,HilbertNode*N){

        for(int i = 0 ; i < parent->children.size();++i){
            HilbertNode*c  = parent->children[i];
            if(c == N)
                return parent->children[i+1];
        }

        return nullptr;

    }


    HilbertNode* handle_underflow(HilbertNode* N){
        HilbertNode *parent = N->parent;
        bool posible = 0;

        vector<HilbertNode *> hermanos_n = get_S(N);

        if(hermanos_n.size() == 1){
            hermanos_n.push_back(find_next_sibling(parent,N));
        }

        for (auto &e : hermanos_n)
        {
            if (e->children.size() > m)
            {
                posible = 1;
            }
        }


        vector<HilbertNode *> total_children;

        for (auto &p : hermanos_n)
        {
            for (auto &c : p->children)
            {
                total_children.push_back(c);
            }
        }

        if(!posible){
            parent->find_and_delete(hermanos_n.back());
            hermanos_n.pop_back();
        }

        int it = 0;
        int children_per_node = total_children.size() / hermanos_n.size(); // 3
        int leftovers = total_children.size() % hermanos_n.size();         // 2

        for (int rep = 0; rep < hermanos_n.size(); ++rep)
        {

            int children_in_node = children_per_node;

            if (leftovers)
            {
                children_in_node++;
                leftovers--;
            }

            hermanos_n[rep]->children.clear();

            for (int i = 0; i < children_in_node; ++i)
            {
                // insert_node->parent = hermanos_n[rep];
                hermanos_n[rep]->children.push_back(total_children[it]);
                total_children[it]->parent = hermanos_n[rep];
                ++it;
            }

            hermanos_n[rep]->sort_nodes();
            hermanos_n[rep]->adjustMBB();
        }

        return hermanos_n.back();


    }


    void fix_tree(HilbertNode*L){

        if(L->is_root){
            if(L->status != Status::leaf_mbb && L->children.size() == 1){
                root = root->children[0];
                root->is_root = 1;
                root->parent = nullptr;
                delete L;
            }
            return;
        }

        if(L->children.size() < m){

            L = handle_underflow(L);
            // L->sort_nodes();
            // L->adjustMBB();

            adjustTree(L,nullptr);
        }

        fix_tree(L->parent);
    }

public:
    HB_Tree()
    {
        root = new HilbertNode();
        root->is_root = true;
        root->status = Status::leaf_mbb;
        // int max_int = numeric_limits<int>::max();
        root->obj = new MBB(Point(0, 0), Point(0, 0));
    }

    void insert(SpatialObj *new_obj, Status s, int h_index)
    {

        HilbertNode *L = choose_leaf(root, h_index);
        HilbertNode *obj_wrapper = new HilbertNode();
        HilbertNode *NN = nullptr;

        obj_wrapper->obj = new_obj;
        obj_wrapper->status = s;
        obj_wrapper->hindex = h_index;

        if (L->children.size() < M)
        {
            L->children.push_back(obj_wrapper);
            obj_wrapper->parent = L;
            L->sort_nodes();
            L->adjustMBB();
            adjustTree(L, NN);
        }
        else
        {
            // NN es un hermano de L que no esta insertado en el mismo nodo
            NN = handle_overflow(L, obj_wrapper);
            adjustTree(L, NN);
        }
    }


    void remove_spatialobj(SpatialObj* click,SpatialObj* click_box ) {
        if(root->obj == nullptr || (root->status == Status::leaf_mbb && root->children.size() == 0))
            return;
        // eliminar punto y reinsertar nodos que hicieron underflow
        HilbertNode* supposed_node_to_delete = knn(click,1)[0];
        // cout<<"resultado"<<endl;
        // supposed_node_to_delete->obj->display();
        // cout<<endl;


        if((supposed_node_to_delete->status == Status::polygon &&supposed_node_to_delete->obj->intersection(click_box) > 0) ||
            (supposed_node_to_delete->status == Status::point && contains(click_box,supposed_node_to_delete->obj))){ 


            HilbertNode* parent = supposed_node_to_delete->parent;

            // cout<<"||||||antes||||||\n";
            // show_tree();
            parent->find_and_delete(supposed_node_to_delete);
            parent->adjustMBB();
            parent->sort_nodes();
            adjustTree(parent,nullptr);
            // cout<<"|||||despues|||"<<endl;
            // show_tree();

            
            fix_tree(parent);

        }

    }

    void show_tree()
    {
        show(root, "");
    }

    HilbertNode *get_root()
    {
        return root;
    }

    vector<HilbertNode *> knn(SpatialObj *source, int n)
    {
        if (root->obj == nullptr)
            return vector<HilbertNode *>{};

        priority_queue<pair<double, HilbertNode *>> pq;
        pq.push({-source->getDistanceTo(root->obj), root});
        vector<HilbertNode *> result;

        // int iter = 0;
        while (!pq.empty())
        {
            pair<double, HilbertNode *> front = pq.top();
            pq.pop();
            // cout<<"iter: "<<iter<<endl;
            // ++iter;
            // front.second->obj->display();
            // cout<<" "<<front.first<<endl;
            if (front.second->status == Status::polygon || front.second->status == Status::point)
            {
                result.push_back(front.second);
                // cout<<front.first<<endl;
                if (result.size() == n)
                    break;
            }
            else
            {
                // cout<<"adding childs: "<<endl;
                for (auto &c : front.second->children)
                {
                    pq.push({-source->getDistanceTo(c->obj), c});
                    // cout<<(-source->getDistanceTo(c->obj))<<" ";
                    // c->obj->display();
                    // cout<<endl;
                }
                // cout<<"finished"<<endl;
            }
        }
        return result;
    }
};
