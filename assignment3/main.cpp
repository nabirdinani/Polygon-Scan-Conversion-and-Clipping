#include <GLUT/GLUT.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>
#include <math.h>
#include <algorithm>

using namespace std;

#define ImageW 400
#define ImageH 400

float framebuffer[ImageH][ImageW][3];

struct color {
    
    float r, g, b;
    
    color(GLfloat red, GLfloat green, GLfloat blue){ r = red; g = green; b = blue; }
};

//color colors[10];

struct point {
    
    GLfloat x_cord;
    GLfloat y_cord;
};

//redo these
static vector<color> Colors =
{
    color(1.0, 0.0, 0.0),
    color(0.0, 1.0, 0.0),
    color(0.0, 0.0, 1.0),
    color(1.0, 1.0, 0.0),
    color(0.0, 1.0, 1.0),
    color(0.5, 0.5, 0.5),
    color(1.0, 0.0, 0.5),
    color(1.0, 0.5, 1.0),
    color(0.5, 0.2, 0.2),
    color(1.0, 0.5, 0.0)
};

static int vertices_count  = 0;

static GLfloat last_max_Y ;
static GLfloat last_current_x ;
static GLfloat last_x_increase ;
static GLfloat last_min_Y ;

static GLfloat first_x_cord;
static GLfloat first_y_cord;
static GLfloat previous_X;
static GLfloat previous_Y;

static GLfloat current_X;
static GLfloat current_Y;

struct edge {
    
    point p1;
    point p2;
    
    GLfloat x1 = p1.x_cord;
    GLfloat y1 = p1.y_cord;
    GLfloat x2 = p2.x_cord;
    GLfloat y2 = p2.y_cord;

    GLfloat current_X;
    GLfloat max_Y;
    GLfloat x_increase;
    
    edge(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
    {
        if (y1 != y2){
            
            GLfloat temp = max(y1, y2);
    
            max_Y = temp;
            last_max_Y  = max_Y;
            
            if (temp != y2){
                
                current_X = x2;
                x_increase = ((x1 - x2) / (y1 - y2));
                
            }
            else if(temp == y2){
            
                current_X = x1;
                x_increase = ((x2 - x1) / (y2 - y1));
                
            }
            GLfloat t = current_X;
            GLfloat t2 = t;
            last_current_x  = t2;
            GLfloat t3 = x_increase;
            GLfloat t4 = t3;
            last_x_increase  = t4;
        }
    }
};

bool is_Ordered(edge& edge1, edge& edge2){
    
    if (edge1.current_X == edge2.current_X){
        
        bool temp = (edge1.x_increase < edge2.x_increase);
        return temp;
    }
    else if (edge1.x_increase == edge2.x_increase){
        bool temp = true || false;
        return temp;
    }
    else{
        bool temp = (edge1.current_X < edge2.current_X);
        return temp;
    }
}

struct polygon {
    
    map<GLfloat, list<edge> > edge_table_sorted;

    polygon(multimap<GLfloat, edge> edges_new){
        
        std::multimap<GLfloat, edge>::iterator iter = edges_new.begin();

        
        while (iter != edges_new.end()){
            
            GLfloat first = (iter)->first;
            GLfloat temp = (iter)->first;
            list<edge> list_Edges;
            
            while (first == (*iter).first && iter != edges_new.end()){
                
                list_Edges.push_back((*iter).second);
                ++iter;
            }
            list_Edges.sort(is_Ordered);
            edge_table_sorted.emplace(first, list_Edges);
        }
    }
};

static vector<polygon> Polygons;

void drawit(void)
{
    glDrawPixels(ImageW,ImageH,GL_RGB,GL_FLOAT,framebuffer);
    glFlush();
}

void clearFramebuffer()
{
    int i,j;
    
    for(i=0;i<ImageH;i++) {
        for (j=0;j<ImageW;j++) {
            framebuffer[i][j][0] = 0.0;
            framebuffer[i][j][1] = 0.0;
            framebuffer[i][j][2] = 0.0;
        }
    }
}


void setFramebuffer(int x, int y, float R, float G, float B)
{
    y = ImageH - 1 - y;
    if (R<=1.0)
        if (R>=0.0)
            framebuffer[y][x][0]=R;
        else
            framebuffer[y][x][0]=0.0;
        else
            framebuffer[y][x][0]=1.0;
    if (G<=1.0)
        if (G>=0.0)
            framebuffer[y][x][1]=G;
        else
            framebuffer[y][x][1]=0.0;
        else
            framebuffer[y][x][1]=1.0;
    if (B<=1.0)
        if (B>=0.0)
            framebuffer[y][x][2]=B;
        else
            framebuffer[y][x][2]=0.0;
        else
            framebuffer[y][x][2]=1.0;
}

struct clipWindow {
    float minX, maxX, minY, maxY;
    
    void createWindow(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
        if (x1 < x2) { minX = x1; maxX = x2; }
        else { minX = x2; maxX = x1; }
        
        if (y1 < y2) { minY = y1; maxY = y2; }
        else { minY = y2; maxY = y1; }
    }

    
};

static multimap<GLfloat, edge> Edges;
static map<GLfloat, list<edge> > edges_used;

void showWindow() {
    float minX, maxX, minY, maxY;
    
    if (first_x_cord < current_X) { minX = first_x_cord; maxX = current_X; }
    else { minX = current_X; maxX = first_x_cord; }
    
    if (first_y_cord < current_Y) { minY = first_y_cord; maxY = current_Y; }
    else { minY = current_Y; maxY = first_y_cord; }
    
    int i = minX, j = minY;
    while (i < maxX) {
        
        setFramebuffer(i,minY,1.0,1.0,1.0);
        setFramebuffer(i,maxY,1.0,1.0,1.0);
        
        i = i + 1;
    }
    
    
    while(j < maxY) {
        setFramebuffer(minX,i,1.0,1.0,1.0);
        setFramebuffer(maxX,i,1.0,1.0,1.0);
        
        j = j + 1;
    }
}

struct window{
    GLfloat min_X, min_Y, max_X, max_Y;
    
    void makeWindow(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2){
        
        GLfloat t1 = min(x1, x2);
        GLfloat t2 = t1;
        min_X = t2;
        
        GLfloat r1 = max(x1, x2);
        GLfloat r2 = r1;
        min_X = r2;
        
        GLfloat f1 = min(y1, y2);
        GLfloat f2 = f1;
        min_X = f2;
        
        GLfloat g1 = max(y1, y2);
        GLfloat g2 = g1;
        min_X = g2;
    }
};

static window Window;
static bool is_clipping_on = false;


map<GLfloat, list<edge> > clipping_edges(map<GLfloat, list<edge> > unclippedEdges){
    
    std::map<GLfloat, list<edge> >::iterator unclippedIt; map<GLfloat, list<edge> > clippedEdges;
    
    unclippedIt = unclippedEdges.begin();
   
    
    while( unclippedIt != unclippedEdges.end()){
        
        if ((unclippedIt)->first > Window.max_Y){ break; }
        
       
        if ((unclippedIt)->first < Window.min_Y){
            
            
            for (std::list<edge>::iterator edgeListIt = (unclippedIt)->second.begin(); edgeListIt != (unclippedIt)->second.end(); ){
                
                if ((edgeListIt)->max_Y < Window.min_Y){
                    
                    edgeListIt = (unclippedIt)->second.erase(edgeListIt);
                    continue;
                }
                
                else{
                    
                    GLfloat y = (unclippedIt)->first;
                    
                    while ( y <= Window.min_Y){
                        
                        (edgeListIt)->current_X += (edgeListIt)->x_increase;
                        ++y;
                    }
                    ++edgeListIt;
                }
            }
            clippedEdges.emplace(Window.min_Y, (unclippedIt)->second);
        }
        
        
        for (std::list<edge>::iterator edgeListIt = (*unclippedIt).second.begin(); edgeListIt != (unclippedIt)->second.end(); ++edgeListIt){
            
            if ((edgeListIt)->max_Y > Window.max_Y){
                (edgeListIt)->max_Y = Window.max_Y;
            }
        }
        ++unclippedIt;
    }
    
    return clippedEdges;
}


void display(void)
{
    if (vertices_count == 1){
        
        GLfloat x = first_x_cord;
        GLfloat y = first_y_cord;
        setFramebuffer(x, y, Colors[Polygons.size()].r, Colors[Polygons.size()].g, Colors[Polygons.size()].b);
    }
    
    else if (vertices_count != 0){
       
        GLfloat count = 0;
        GLfloat temp = count;
        GLfloat y_value = last_min_Y;
       
        while (y_value < last_max_Y){
            
            setFramebuffer((last_current_x + (last_x_increase * count)), y_value, Colors[Polygons.size()].r, Colors[Polygons.size()].g, Colors[Polygons.size()].b);
            y_value = y_value + 1;
            count = count + 1;
        }
    }
    
    else if (vertices_count == 0){

        int p = 0;
        list<edge> edge_list;
        std::map<GLfloat, list<edge> >::iterator table_iter;
        
        
        while(p < Polygons.size()){
            
            if (is_clipping_on == true){
                
                showWindow();
                edges_used = clipping_edges(Polygons[p].edge_table_sorted);
            }
            
            else{
                edges_used = Polygons[p].edge_table_sorted;
            }
            
            if(edges_used.empty()){
                cout << "Empty after clipping\n";
                break;
            }
            
            table_iter = edges_used.begin();
            
            edge_list = (table_iter)->second;
            
            std::list<edge>::iterator list_iter = edge_list.begin();
            
            for (GLfloat y = (table_iter)->first; edge_list.empty() == false;){
               
                while (list_iter != edge_list.end() && next(list_iter) != edge_list.end()){
                    
                    GLfloat i = 0;
                    while(((list_iter)->current_X + i) < (*next(list_iter)).current_X){
                       
                        setFramebuffer((list_iter)->current_X + i, y, Colors[p].r, Colors[p].g, Colors[p].b);
                        
                        i = i + 1;
                    }
                    
                    ++list_iter;
                    ++list_iter;
                }
                
               
                list_iter = edge_list.begin();
                
                ++y;
                
                while (list_iter != edge_list.end()){
                    
                    if (y == ((list_iter)->max_Y)){
                        
                        list_iter = edge_list.erase(list_iter);
                        continue;
                    }
                    
                    else{
                        
                        (list_iter)->current_X += (list_iter)->x_increase;
                        ++list_iter;
                    }
                }
            
                list_iter = edge_list.begin();
                
                table_iter = edges_used.find(y);
                
                if (table_iter != edges_used.end()){
                    
                    edge_list.insert(list_iter, (table_iter)->second.begin(), (table_iter)->second.end());
                    edge_list.sort(is_Ordered);
                
                }
                
                list_iter = edge_list.begin();
                
            }
            ++p;
        }
    }
    drawit();
}


void init(void)
{
//    for(int i = 0; i < 10; i++)
//    {
//        colors[i].r = (float)(rand()%1000)/1000;
//        colors[i].g = (float)(rand()%1000)/1000;
//        colors[i].b = (float)(rand()%1000)/1000;
//    }
    
    clearFramebuffer();
}

void key_pressed(unsigned char key, int x, int y) { if (key == 'c') { is_clipping_on = true; } }

void mouse(int button, int state, int x, int y)
{
    if (is_clipping_on != true){
        if (Polygons.size() < 10 && Polygons.size() != 10){
            
            switch (button) {
                case GLUT_LEFT_BUTTON:
                    current_X = x;
                    current_Y = y;
                    if (state == GLUT_DOWN && state != GLUT_UP){
                        
                        if (vertices_count  == 0){
                            
                            GLfloat t1 = x;
                            GLfloat t2 = t1;
                            first_x_cord = t2;
                            
                            GLfloat r1 = y;
                            GLfloat r2 = r1;
                            first_y_cord = r2;

                        }
                        else{
                            
                            GLfloat t1 = fmin(previous_Y, y);
                            GLfloat t2 = t1;
                            last_min_Y = t2;
                            
                            if (previous_Y != y){
                                Edges.emplace(last_min_Y , edge(previous_X, previous_Y, x, y));
                            }
                        }
                        
                        GLfloat t1 = x;
                        GLfloat t2 = t1;
                        previous_X = t2;
                        
                        GLfloat r1 = y;
                        GLfloat r2 = r1;
                        previous_Y = r2;
                        
                        
                        vertices_count = vertices_count + 1;
                        
                        
                        if (vertices_count  == 10){
                            
                            GLfloat n1 = first_x_cord;
                            GLfloat n2 = first_y_cord;
                            GLfloat n3 = previous_X;
                            GLfloat n4 = previous_Y;
                            
                            GLfloat small = fmin(n2, n4);
                            Edges.emplace(small, edge(n1, n2, n3, n4));
                            
                            vertices_count  = 0;
                            
                            Polygons.push_back(polygon(Edges));
                            Edges.clear();
                        
                        }
                        glutPostRedisplay();
                        break;
                    }
                    
                    
                case GLUT_RIGHT_BUTTON:
                    
                    current_X = x;
                    current_Y = y;
                    
                    if (state == GLUT_DOWN && state != GLUT_UP){
                        
                        if (vertices_count > 0 && vertices_count  < 2){
                            break;
                        }
                        
                        else if (vertices_count > 0 && vertices_count > 2){
                            
                            last_min_Y  = fmin(previous_Y, y);
                            
                            if (previous_Y == y){
                                
                                
                            }
                            else if (previous_Y != y){
                                Edges.emplace(last_min_Y , edge(previous_X, previous_Y, x, y));
                            }
                            
                            GLfloat r1 = x;
                            GLfloat r2 = r1;
                            previous_X = r2;
                            
                            GLfloat t1 = y;
                            GLfloat t2 = t1;
                            previous_Y = t2;
                            
                            vertices_count  = 0;
                            
                            Edges.emplace(min(first_y_cord, previous_Y), edge(first_x_cord, first_y_cord, previous_X, previous_Y));
                            
                            Polygons.push_back(polygon(Edges));
                            
                            Edges.clear();
                        }
                        
                        glutPostRedisplay();
                        break;
                    }
                    
                default:
                    break;
            }
        }
    }
    else{
        switch (button) {
                
            case GLUT_LEFT_BUTTON:
                
                current_X = x;
                current_Y = y;
                
                if (state == GLUT_DOWN){
                    
                    
                    GLfloat t1 = x;
                    GLfloat t2 = t1;
                    first_x_cord = t2;
                    
                    GLfloat r1 = y;
                    GLfloat r2 = r1;
                    first_y_cord = r2;
                }
                
                else if (state != GLUT_DOWN && state == GLUT_UP){
                    
                    Window.makeWindow(first_x_cord, first_y_cord, x, y);
                    clearFramebuffer();
                    glutPostRedisplay();
                }
                break;
                
            default:
                break;
        }
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(ImageW,ImageH);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Nabir Dinani - Assignment 3");
    init();	
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(key_pressed);
    glutMainLoop();
    return 0;
}