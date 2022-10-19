#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include "trees.h"

using namespace std;
using namespace sf;


vector<Color> clrs = {Color::Red,Color::Green,Color::Blue,Color::Yellow,Color::Magenta,Color::Cyan};
enum class Mode {insert_points,insert_polygon,delete_obj};


CircleShape create_point(float x, float y){
    CircleShape punto(3);
    punto.setOrigin(Vector2f(3,3));
    punto.setPosition(Vector2f(x,y));
    return punto;
}

ConvexShape create_polygon(Polygon* current,int offset){

    ConvexShape cvx_s;
    int total_points = current->points.size();
    cvx_s.setPointCount(total_points);

    for(int i = 0; i < total_points; ++i){
        int x = current->points[i].x;
        int y = offset - current->points[i].y;
        cvx_s.setPoint(i,Vector2f(x,y));
    }

    return cvx_s;
}

RectangleShape create_mbb_shape(Node* current,int offset,int depth){
    int pos_x_low = current->obj->getLowX();
    int pos_x_high = current->obj->getHighX();
    int pos_y_low = current->obj->getLowY();
    int pos_y_high = current->obj->getHighY();

    RectangleShape r(Vector2f(pos_x_high - pos_x_low,pos_y_high - pos_y_low));
    r.setPosition(Vector2f(pos_x_low, offset - pos_y_high));
    r.setFillColor(Color::Transparent);
    r.setOutlineColor(clrs[depth]);
    r.setOutlineThickness(0.8);

    return r;
}



void create_tree_gui(vector<RectangleShape>&rectangles,vector<CircleShape>&points, vector<ConvexShape>&polygons,Node* current,int offset,int depth){


    if(!current->is_root ){
        if(true){
            if(current->status == Status::mbb || current->status == Status::leaf_mbb){
                RectangleShape r = create_mbb_shape(current,offset,depth);
                rectangles.push_back(r);
            }else if(current->status == Status::point){
                // cout<<depth<<endl;
                CircleShape c = create_point(current->obj->getLowX(),offset - current->obj->getLowY());
                points.push_back(c);
            }else if(current->status == Status::polygon){
                ConvexShape cvx_s = create_polygon((Polygon*) current->obj,offset);
                polygons.push_back(cvx_s);
            }
    
        }
       depth += 1;
    }

    if(current->status != Status::point  && current->status != Status::polygon  ){
        for(auto&node : current->children){

    
            create_tree_gui(rectangles,points,polygons,node,offset,depth);
        }
    }

}

int main(int argc, char** argv)
{
    int length = 800;
    sf::RenderWindow window(sf::VideoMode(length, length), "My window");
    sf::Clock c;
    RTree rt;

    vector<RectangleShape> rectangles;
    vector<CircleShape> points;
    vector<ConvexShape> polygons;
    vector<Point> points_for_polygon;
    vector<Vertex*> knn_lines;
    Mode mode = Mode::insert_points;


    
    int nn = 3;


    
    while (window.isOpen())
    {
        sf::Event event;

        window.clear(sf::Color::Black);
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
           
            else if(event.type == sf::Event::TextEntered){


                int supuesto_numero = event.text.unicode-48;
                if(supuesto_numero >= 0 && supuesto_numero<= 9){
                    cout<<supuesto_numero<<endl;
                    nn = nn*10 + supuesto_numero;
                }
 
            }
            else if(event.type == sf::Event::KeyPressed){

                if(event.key.code == sf::Keyboard::Key::Q){
                    if(mode == Mode::insert_points){
                        mode = Mode::insert_polygon;
                    }else if(mode == Mode::insert_polygon){
                        if(points_for_polygon.size() >= 3){
                            rt.insert_spatialobj(new Polygon(points_for_polygon),Status::polygon);
                            rectangles.clear();
                            polygons.clear();
                            knn_lines.clear();

                            for(auto&v : knn_lines)
                                delete[] v;


                            create_tree_gui(rectangles,points,polygons,rt.get_root(),length,0);

                        }
                        points_for_polygon.clear();
                        mode = Mode::insert_points;
                    }

                }else if(event.key.code == sf::Keyboard::Key::W) {
                    cout<<"El nn actual es "<<nn<<endl;
                }else if(event.key.code == sf::Keyboard::Key::BackSpace){
                    nn = nn == 0 ? 0 : nn/10;
                }
                else if(event.key.code == sf::Keyboard::Key::LShift && mode != Mode::insert_polygon){
                    mode = Mode::delete_obj;
                    // cout<<"MODO: eliminar objeto"<<endl;
                }
            }
            else if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Key::LShift && mode == Mode::delete_obj){
                    mode = Mode::insert_points;
                    // cout<<"MODO: insertar objeto"<<endl;
                }
 
            }
            else if(event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseWheelScrolled){
                float x = event.mouseButton.x;
                float y = length - event.mouseButton.y;
                cout<<x<<" "<<y<<endl;
                if(Mouse::isButtonPressed(Mouse::Left) || event.type == sf::Event::MouseWheelScrolled){
                    if(event.type == sf::Event::MouseWheelScrolled){
                        x = event.mouseWheelScroll.x;
                        y = length - event.mouseWheelScroll.y;
                    }

                    if(mode == Mode::insert_points){
                        Point* insert_point = new Point(x, y);
                        rt.insert_spatialobj(insert_point,Status::point);
                        

                        //TODO: handle garbage  
                        rectangles.clear();
                        points.clear();
                        polygons.clear();
                        knn_lines.clear();

                        create_tree_gui(rectangles,points,polygons,rt.get_root(),length,0);
                    }else if(mode == Mode::insert_polygon){
                        points_for_polygon.push_back(Point(x, y));
                    }else if(mode == Mode::delete_obj){
                        vector<Point> pts;
                        int rad = 4;
                        pts.push_back({(int) x-rad,(int) y-rad});
                        pts.push_back({(int) x+rad,(int) y-rad});
                        pts.push_back({(int) x+rad,(int) y+rad});
                        pts.push_back({(int) x-rad,(int) y+rad});
                        Point* click = new Point(x,y);
                        Polygon* click_box = new Polygon(pts);
                        rt.remove_spatialobj(click,click_box);

                        rectangles.clear();
                        points.clear();
                        polygons.clear();
                        for(auto&v : knn_lines)
                            delete[] v;
                        knn_lines.clear();

                        create_tree_gui(rectangles,points,polygons,rt.get_root(),length,0);
                        delete click;
                        delete click_box;

                    }
                }else if(Mouse::isButtonPressed(Mouse::Right) && nn > 0){
                    Point* pnt = new Point(x,y);
                    // cout<<"resultados: "<<endl;
                    auto result = rt.knn(pnt,nn); 
                    delete pnt;


                    for(auto&n : knn_lines)
                        delete[] n;
                    
                    knn_lines.clear();

                    
                    for(auto n: result){

                        int x_n,y_n;
                        if(n->status == Status::polygon){
                            x_n = (n->obj->getHighX() +  n->obj->getLowX())/2;
                            y_n = (n->obj->getHighY() +  n->obj->getLowY())/2;
   
                        }else if(n->status == Status::point){
                            x_n = n->obj->getHighX();
                            y_n = n->obj->getHighY();
                        }

                        Vertex* new_line = new Vertex[2]{Vector2f(x_n,length - y_n),Vector2f(x,length-y)};
                        new_line[0].color = Color::Yellow;
                        new_line[1].color = Color::Yellow;
                        knn_lines.push_back(new_line);

                    }

                }

                // rt.show_rtree();

            }
        }

        for(auto&v : knn_lines)
            window.draw(v,2,Lines);
        for(auto & c : points)
            window.draw(c);
        for(auto& r: rectangles)
            window.draw(r);
        for(auto&p : polygons)
            window.draw(p);
       
        window.display();
    }
}
