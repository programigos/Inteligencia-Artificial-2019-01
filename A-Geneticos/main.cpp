#define GLUT_DISABLE_ATEXIT_HACK
#include <windows.h>
#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <utility>
//#include <quadTree.h>
#define KEY_ESC 27
#define KEY_TAB 9 ///tab
#define KEY_D 100
#define KEY_A 97

using namespace std;
float var_x = 0.0;
bool pain = 1;
struct Point{
    int x,y;
    Point(int a, int b):x(a),y(b){}
};

template <class T>
void Swap(T &a, T &b){
    T temp=a;
    a=b;
    b=temp;
}

template <class T>
void printVector(vector<T> vec){
    cout<<"[";
    for(size_t i=0;i<vec.size();++i)
        cout<<vec[i]<<" ";
    cout<<"]"<<endl;
}
unsigned int population;

template <class T>
bool find(vector<T> vec, T val){
    for(size_t i=0;i<vec.size();++i)
        if(vec[i]==val)
            return 1;
    return 0;
}

int pointNum, actual_route=0, iteration=0;

vector<Point> points;

vector< pair<float,vector<int> > > routes;

pair<float,vector<int> > best_route,worse;
vector<int> fdx,fdx2,fdx3;

void generate_points(){
    points.assign(pointNum,Point(0,0));
    for(size_t i=0;i<pointNum;++i){
        points[i].x=rand()%100;
        points[i].y=rand()%100;
    }
}

float euclidean(Point u, Point v){
    return sqrt(pow(u.x-v.x,2)+pow(u.y-v.y,2));
}

float route_size(vector<int> route){
    float result=0;
    for(int i=0;i<pointNum;++i)
        result+=euclidean(points[route[i]],points[route[(i+1)%pointNum]]);
    return result;
}

pair<float,vector<int> > generate_route(){
    vector<int> result(pointNum,0);
    for(int i=1;i<pointNum;++i)
        result[i]+=i;
    int iterations = rand()%30+20;
    for(int i=0;i<iterations;++i)
        Swap(result[rand()%pointNum],result[rand()%pointNum]);
    //iteration = 0;/**mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/
    return make_pair(route_size(result),result);
}

class compare{
public:
    bool operator() (pair<float,vector<int> > a, pair<float,vector<int> > b){
        return a.first<b.first;
   }
};

vector<int> mergeRoutes(vector<int> routeA, vector<int> routeB){
    vector<int> result;
    result.insert(result.end(),routeA.begin(),routeA.begin()+(pointNum/2));
    for(size_t i=0;i<routeB.size();++i)
        if(!find(result,routeB[i]))
            result.push_back(routeB[i]);
    return result;
}

void mutateRoute(pair<float,vector<int> > &route){
    Swap(route.second[rand()%pointNum],route.second[rand()%pointNum]);
    route.first=route_size(route.second);
}
int primer;
vector< pair<float,vector<int> > > nextGeneration(vector< pair<float,vector<int> > > vec){
    vector< pair<float,vector<int> > > result;
    result.push_back(vec[0]);///#############################################3
    vector<int> aux;
    for(int i=0; i<2000; ++i){
        aux=mergeRoutes(vec[rand()%population].second,vec[rand()%population].second);
        result.push_back(make_pair(route_size(aux),aux));
    }
    for(int i=0; i<10; ++i)
        mutateRoute(result[rand()%1999+1]);
    sort(result.begin(),result.end(),compare());
    result.erase(result.begin()+population,result.end());
    ++iteration;
    float xx=best_route.first / primer;
    xx*=90.0;
    fdx.push_back((int)xx);///-----------------------------------------
    worse = routes[routes.size()-1];
    float xx2=worse.first / primer;
    xx2*=90.0;
    fdx2.push_back((int)xx2);
    fdx3.push_back((int)(xx+xx2)/2);
    return result;
}

void drawPoints(){
    glPushMatrix();
    glPointSize(8.0f);
    glBegin(GL_POINTS);
        glColor3d(1.0f,1.0f,1.0f);
        for(size_t i=0;i<pointNum;++i)
            glVertex2d(points[i].x,points[i].y);
        glColor3d(0.0f,1.0f,0.0f);
        glVertex2d(points[best_route.second[0]].x,points[best_route.second[0]].y);
        glColor3d(0.0f,0.0f,1.0f);
        glVertex2d(points[best_route.second[pointNum-1]].x,points[best_route.second[pointNum-1]].y);
    glEnd();
    glPopMatrix();
}

void drawRoute(){
    glPushMatrix();
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glColor3d(1.0f,0.0f,0.0f);
        for(size_t i=0;i<pointNum;++i)
            glVertex2d(points[best_route.second[i]].x,points[best_route.second[i]].y);

    glEnd();
    glPopMatrix();
}

void OnMouseClick(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
    if(actual_route<routes.size()-1)
        actual_route+=1;
    best_route=routes[actual_route];
    //convertir x,y
	//insertar un nuevo punto en el quadtree
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
      if(actual_route>0)
        actual_route-=1;
      best_route=routes[actual_route];
    //convertir x,y
	//insertar un nuevo punto en el quadtree
  }
  cout<<actual_route<<" "<<routes[actual_route].first<<endl;
}

void OnMouseMotion(int x, int y)
{
     //opcional
	 //hacer algo x,z cuando se mueve el mouse
}

void idle(){ // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}
void displayFunction()
{
//    const int cant = 20;
//    worse = routes[routes.size()-1];
    //int aux = -1 *(best_route.first - worse.first);
    glPushMatrix();
    glLineWidth(2.5f);
    glColor3f(0.1f,0.7f,0.1f);
    size_t ss = fdx.size();
    glBegin(GL_LINE_STRIP);
        for(size_t i=0;i<ss;++i){
//            glVertex2i(2*i,0);
            glVertex2i(2*i,fdx[i]);
        }
    glEnd();
    glColor3f(0.7f,0.1f,0.1f);
    size_t ss2 = fdx2.size();
    glBegin(GL_LINE_STRIP);
        for(size_t i=0;i<ss2;++i){
//            glVertex2i(2*i,0);
            glVertex2i(2*i,fdx2[i]);
        }
    glEnd();
    glColor3f(0.1f,0.1f,0.7f);
    size_t ss3 = fdx3.size();
    glBegin(GL_LINE_STRIP);
        for(size_t i=0;i<ss3;++i){
//            glVertex2i(2*i,0);
            glVertex2i(2*i,fdx3[i]);
        }
    glEnd();
    glPopMatrix();
}
//funcion llamada a cada imagen
void glPaint(void) {

	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	glOrtho(-20.0f,  120.0f,-20.0f, 120.0f, -1.0f, 1.0f);

	if(pain){
        drawPoints();
        drawRoute();
	}else{
	    glPushMatrix();
        glBegin(GL_LINES);
            glLineWidth(1.0f);
            glColor3f(0.5f,0.1f,0.8f);
            glVertex2i(-15,0);
            glVertex2i(-15,115);
            glColor3f(0.2f,0.7f,0.2f);
            glVertex2i(-15,0);
            glVertex2i(115,0);
        glEnd();
	    glTranslatef(var_x, 0.0f,0.0f);
        displayFunction();
        glPopMatrix();
	}
	routes=nextGeneration(routes);
	best_route=routes[0];
	cout<<best_route.first<<" "<<iteration<<endl;
	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
	var_x-=1.0;
//	glFlush();
}

//
//inicializacion de OpenGL
//
void init_GL(void) {
	//Color del fondo de la escena
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //(R, G, B, transparencia) en este caso un fondo negro

	//modo projeccion
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

//en el caso que la ventana cambie de tamaño
GLvoid window_redraw(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


}

GLvoid window_key(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_ESC:
		exit(0);
		break;
    case KEY_TAB:
        pain = !pain;
        break;
    case KEY_D:
        var_x -= 8.0;
        break;
    case KEY_A:
        var_x += 8.0;
        break;
	default:
		break;
	}

}
//
//el programa principal
//
int main(int argc, char** argv) {
    srand(time(NULL));
	//Inicializacion de la GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("TP2 bis OpenGL : Bresenham"); //titulo de la ventana

	init_GL(); //funcion de inicializacion de OpenGL

	glutDisplayFunc(glPaint);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
    population=1000;
	pointNum=20;
	generate_points();
	for(int i=0;i<population;++i)
        routes.push_back(generate_route());
	sort(routes.begin(),routes.end(),compare());
	best_route=routes[0];
	primer = best_route.first;
	glutMainLoop(); //bucle de rendering
	//no escribir nada abajo de mainloop
	return 0;
}
