#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <cmath>
#include "hilbert_tree.h"

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
void insert_HBtree(){

    //3. crear vector de vectores
    for(int a=0;a<100;a++){
          vector<double>times;

        HB_Tree* rt=new HB_Tree();
        //2.vector para guardar los 500 tiempos
        for(int i=0;i<500;i++){
            vector<Polygon*>polygons;
            vector<Point> point;
            // 1. tiempo inicial para los 10 poligonos
            for(int j=0;j<10;j++){
                point=crear_poligono();
                t0=clock(); // tiempo inicial

                Polygon* pol = new Polygon(point);
                Point poi = pol->center();
                                                    //(a  = bloque de x , b = bloque de y , k = 2*orden)
                rt->insert(pol,Status::polygon,hindex(poi.x/8,poi.y/8,16));
                t1=clock(); // tiempo final

                ttotal=ttotal+(double(t1-t0)/CLOCKS_PER_SEC);
            }
            times.push_back(ttotal);
            ttotal=0;
            // 1.tiempo final para los 10 poligonos
            // 1.pushear al vector
            point.clear();
            polygons.clear();

        }
        // rt->show_tree();
        for(int t=0;t<500;t++){
            cout<<times[t]<<" ";
        }

        cout<<endl;
        
        //rt->show_rtree();
        delete rt;
        times.clear();
        //2. almacenar ese vector en otro vector de vectores

    }
}



void search_HBtree(){
    

    for(int b=0;b<100;b++){
        //2. crear vector de vectores
      vector<double>times;
        HB_Tree* rt=new HB_Tree();

        for(int i=0; i<5;i++){
            vector<Polygon*>polygons;
            vector<Point> point;

            for(int j=0;j< 1000; j++){
                point = crear_poligono();

                Polygon* pol = new Polygon(point);
                Point poi = pol->center();
                rt->insert(pol,Status::polygon,hindex(poi.x/8,poi.y/8,16));
                polygons.push_back(pol);
                // rt->insert(new Polygon(point),Status::polygon,H);
            }
            //1. tiempo inicial busqueda 1000 poligonos
            t0=clock();
            for(int a=0;a<polygons.size();a++){
                rt->knn(polygons[a],1);
            }
            t1=clock();
            //1. tiempo final busqueda 1000 poligonos
            //1.pushear el tiempo al vector
            ttotal=double(t1-t0)/CLOCKS_PER_SEC;
            times.push_back(ttotal);
            ttotal=0;
            polygons.clear();
            point.clear();
        }
        cout<<endl;
        //cout<<"Experimento "<<b+1<<":";
        for(int t=0;t<5;t++){
            cout<<times[t]<<" ";
        }
        //2. almacenar ese vector en otro vector de vectores
        delete rt;
        times.clear();

    }
    //rt->show_rtree();

}

// hay un error no borra todos, anteriormente borraba todo. reviso mañana
void remove_HBtree(){
    
    int rnd=0;
    for(int b=0;b<100;b++){
        vector<double>times;
        HB_Tree* rt=new HB_Tree();
        vector<Polygon*>Ptotal;

        for(int j=0;j< 5000; j++){
            vector<Polygon*>polygons;
            vector<Point> point;
            point = crear_poligono();
            Polygon* pol = new Polygon(point);
            Point poi = pol->center();
            rt->insert(pol,Status::polygon,hindex(poi.x/8,poi.y/8,16));

            // polygons.push_back(pol);
            // Ptotal.push_back(pol);
            polygons.push_back(new Polygon(point));
            Ptotal.push_back(new Polygon(point));
            polygons.clear();
            point.clear();
        }
        //rt->show_rtree();
        // 2.vector de vectores
        for(int a=0;a<5;a++){
            int temp=1000;
            //1.tiempo inicial borrado 1000
            t0=clock();
            for(int b=0;b<1000;b++){
                rnd=rand()%temp;
                vector<Point>p = Ptotal[rnd]->points;

                for(int c=0;c<p.size();c++){
                    rt->remove_spatialobj(&p[c],Ptotal[rnd]);
                }
                Ptotal.erase(Ptotal.begin()+rnd);
                temp=temp-1;
            }
            t1=clock();
            ttotal=double(t1-t0)/CLOCKS_PER_SEC;
            times.push_back(ttotal);
            ttotal=0;
            //1.tiempo final borrado 1000
            // 1.push tiempo a vector
        }
        //2. pushear el vector en un vector de vectores
        //rt->show_rtree();

        cout<<endl;
       // cout<<"Experimento "<<b+1<<":";
        for(int t=0;t<5;t++){
            cout<<times[t]<<" ";
        }
        Ptotal.clear();
        delete rt;
        times.clear();

        //point.clear();
        //polygons.clear();
    }

}

void knn_HBtree(){
    
    int px=0;
    int py=0;
    int k=20;
    for(int a=0;a<100;a++){
      vector<double>times;
        HB_Tree* rt=new HB_Tree();

        for(int i=0;i<5;i++){
            vector<Polygon>polygons;
            vector<Point> point;

            for(int j=0;j<1000;j++){
                point = crear_poligono();
                Polygon* pol = new Polygon(point);
                Point poi = pol->center();
                rt->insert(pol,Status::polygon,hindex(poi.x/8,poi.y/8,16));

            }
            px=rand()%2048;
            py=rand()%2048;
            Point* pnt = new Point(px,py);
            //1.tiempo inicial knn
            t0=clock();
            rt->knn(pnt,k);
            t1=clock();
            ttotal=double(t1-t0)/CLOCKS_PER_SEC;
            times.push_back(ttotal);
            ttotal=0;
            //1.tiempo final knn
            //1.push tiempo al vector
            point.clear();
            polygons.clear();
            delete pnt;
        }
        cout<<endl;
        //cout<<"Experimento "<<a+1<<":";
        for(int t=0;t<5;t++){
            cout<<times[t]<<" ";
        }
        delete rt;
        times.clear();

    }
    //rt->show_rtree();

}

int main(){


    srand(time(NULL));


    insert_HBtree();
    // search_HBtree();
    // remove_HBtree();
    // knn_HBtree();


    return 0;
}
