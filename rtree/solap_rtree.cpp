#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <cmath>
#include "hilbert_tree.h"
// #include "rtree.h"
// #include "star_rtree.h"
#define H 8

using namespace std;
unsigned t0=0;
unsigned t1=0;
double ttotal=0;
//5-17
//#include "star_rtree.h"
float diagonal(float angulo,float tam){
    float diagonal;

    diagonal = (tam/2.00)/ sin(angulo) ;

    return abs(diagonal);

}


vector<Point> crear_poligono(){
    // 4.3< tam < 17.2

    int negativo = 1;

    vector<Point> point_pol;
    while(negativo){
        int n = 3+rand()%7;
        float ang = 360.00/n;
        float tam = 4.3 + rand()%129/10.00;
        float diag=diagonal(ang,tam);
        float xrandom=0;
        float yrandom=0;
        do{
            xrandom= (rand()%2048)-diag;
            yrandom= (rand()%2048)-diag;
        }while(xrandom<0 && yrandom<0);


        negativo = 0;

        for(int i=0;i<n;i++){
            int x,y;
            x= xrandom + diag* sin((ang+ang*i)*0.01745);
            y= yrandom + diag* cos((ang+ang*i)*0.01745);
            if(x < 0 || y < 0){
                // cout<<x<<" "<<y<<endl;
                negativo = 1;
                point_pol.clear();
                break;
            }
            point_pol.push_back(Point(x,y));
        }


    }

    // for(int i=0;i<point_pol.size();i++){
    //     if(point_pol[i].x < 0 || point_pol[i].x > 2048 
    //        || point_pol[i].y < 0 || point_pol[i].y > 2048) 
    //         cout<<point_pol[i].x<<" "<<point_pol[i].x<<endl;
    // }



    return point_pol;
}
// insert
void test_solap(){

    //3. crear vector de vectores
    int it = 100;
    double sum = 0;
    int div = 1;
    int length = 2048;
    for(int a=0;a<it;a++){
        // cout<<a<<endl;
          vector<double>times;

        HB_Tree* rt=new  HB_Tree();
        // RTree* rt=new  RTree();
        // StarRTree* rt=new  StarRTree();
        //2.vector para guardar los 500 tiempos
        for(int i=0;i<5000;i++){
            vector<Point> point=crear_poligono();

            Polygon* pol = new Polygon(point);
            Point poi = pol->center();

            rt->insert(pol,Status::polygon,hindex(poi.x/div,poi.y/div,log2(length/div)*2));
            // rt->insert_spatialobj(pol,Status::polygon);
            // rt->show_tree();

        }

        rt->show_tree();

        sum += rt->coeficienteSolapamiento();
        
        delete rt;
        times.clear();
        //2. almacenar ese vector en otro vector de vectores

    }

    cout<<sum/it<<endl;
}




int main(){


    srand(time(NULL));


    test_solap();

    // for(int i = 0; i < 10000000;++i)
    // crear_poligono();


    // insert_HBtree();
    // search_HBtree();
    // remove_HBtree();
    // knn_HBtree();


    return 0;
}
