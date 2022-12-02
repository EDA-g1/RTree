#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <ctime>
#include "rtree.h"

using namespace std;
int main(){
  int x=0;
  int y=0;
  int n=0;
  srand(time(NULL));
   //time_t instanteInicial, instanteFinal;
    //instanteInicial = time(0);
    //
  RTree rt;
  for(int a=0;a<10;a++){

  
    for(int i=0;i<5;i++){
      /* vector<Polygon>polygons; */
      vector<Point> points;
  
      for(int j=0;j<10;j++){
        n= 3 +  (rand() % (7-3));
        for(int k=3;k<n;k++){
          x=rand()%(2048); //2048
          y=rand()%(2048); //2048
          points.push_back(Point(x,y));
        }  
	Polygon* p = new Polygon(points);
        rt.insert_spatialobj(p,Status::polygon);
	/* p->display(); */
        points.clear();
      } 
      
      /* polygons.clear(); */
    }
    
  }
  //instanteFinal = time(0);
   // cout << "Segundos empleados: " << instanteFinal - instanteInicial << endl;
  rt.show_rtree();
  /////////////////////////////////////////////
  
  return 0;
} 
