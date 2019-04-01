#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#define Size	600

using namespace std;

typedef float Color[3];  // specifies the color of lines
struct Point{       // creating structure to store x and y coordinates of points.
	int x, y;
};
typedef struct IntersectionPoint {    //this structure stores intersection points.
	int pointFlag;
	int index0, index1;
	Point p;
	bool inFlag;
	int dis;
}IP;

class Polygon{     // This class forms the polygon and displays it onm the screen.
	public:
		vector<Point> pts;  // this vector stores the coordinates of the polygon vertices.
		Polygon(void);
		~Polygon(void);
		void drawPolygonLine(Color c);
};
Polygon::Polygon(void){
}
Polygon::~Polygon(void){
}
void Polygon::drawPolygonLine(Color c) {
	glColor3fv(c);
	glLineWidth(2.0); // specifies the line width
	glBegin(GL_LINE_LOOP);  // this creates a loop of lines to form a closed polygon.
	int size = pts.size(); //stores size of vector
	for (int i = 0; i < size; i++)   //this loop selects the coordinates of vertices of polygon to form loop of lines.
		glVertex2i(pts[i].x, pts[i].y);
	glEnd();
}

bool isPointInsidePolygon(Point p, Polygon& py) {      // This function checks if the point is inside the clipping polygon or not.
	int cnt = 0, size = py.pts.size();
	for (int i = 0; i < size; i++) {
		Point p1 = py.pts[i];
		Point p2 = py.pts[(i + 1) % size];
		if (p1.y == p2.y) continue;
		if (p.y < min(p1.y, p2.y)) continue;
		if (p.y >= max(p1.y, p2.y)) continue;
		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;
		if (x > p.x) cnt++;
	}
	return (cnt % 2 == 1);
}
int cross(Point& p0, Point& p1, Point& p2) {    //this function checks if the point crosses the line segment or not.
	return ((p2.x - p0.x) * (p1.y - p0.y) - (p1.x - p0.x) * (p2.y - p0.y));
}  
bool onSegment(Point& p0, Point& p1, Point& p2) {     // this function checks if the point lies on the line segment or not
	int minx = min(p0.x, p1.x), maxx = max(p0.x, p1.x);
	int miny = min(p0.y, p1.y), maxy = max(p0.y, p1.y);
	if (p2.x >= minx && p2.x <= maxx && p2.y >= miny && p2.y <= maxy) return true;
	return false;
}
bool segmentsIntersect(Point& p1, Point& p2, Point& p3, Point& p4) {    // this function checks if the line segment formed by  given points intersect or not.
	int d1 = cross(p3, p4, p1);
	int d2 = cross(p3, p4, p2);
	int d3 = cross(p1, p2, p3);
	int d4 = cross(p1, p2, p4);
	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
			((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
		return true;
	if (d1 == 0 && onSegment(p3, p4, p1)) return true;
	if (d2 == 0 && onSegment(p3, p4, p2)) return true;
	if (d3 == 0 && onSegment(p1, p2, p3)) return true;
	if (d4 == 0 && onSegment(p1, p2, p4)) return true;
	return false;
}
Point getintersectPoint(Point p1, Point p2, Point p3, Point p4){ //This function finds the point of intersections.
	Point p;
	int b1 = (p2.y - p1.y) * p1.x + (p1.x - p2.x) * p1.y;
	int b2 = (p4.y - p3.y) * p3.x + (p3.x - p4.x) * p3.y;
	int D = (p2.x - p1.x) * (p4.y - p3.y) - (p4.x - p3.x) * (p2.y - p1.y);
	int D1 = b2 * (p2.x - p1.x) - b1 * (p4.x - p3.x);
	int D2 = b2 * (p2.y - p1.y) - b1 * (p4.y - p3.y);
	p.x = D1 / D;
	p.y = D2 / D;
	return p;
}
void generateIntersectPoints(Polygon& pyclip, Polygon& py, list<IP>& iplist) {   // This function finds and stores the intersection points of the clipping polygon and the polygon to be clipped in a list.
	int clipSize = pyclip.pts.size(), pySize = py.pts.size();

	for (int i = 0; i < clipSize; i++) {
		Point p1 = pyclip.pts[i];
		Point p2 = pyclip.pts[(i + 1) % clipSize];
		for (int j = 0; j < pySize; j++) {
			Point p3 = py.pts[j];
			Point p4 = py.pts[(j + 1) % pySize];
			if (segmentsIntersect(p1, p2, p3, p4)) {
				IP ip;
				ip.index0 = j;
				ip.index1 = i;
				ip.p = getintersectPoint(p1, p2, p3, p4);
				iplist.push_back(ip);
			}
		}
	}
}
int getDistance(Point& p1, Point& p2) {   // this function returns the distance between the given two points.
	return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}
bool distanceComparator(IP& ip1, IP& ip2) {    // this function compares two distances and returns the minimum distance.
	return ip1.dis < ip2.dis;
}
void generateList(Polygon& py, list<IP>& iplist, list<IP>& comlist, int index) {   // this function generates a list of points 
	int size = py.pts.size();
	list<IP>::iterator it;

	for (int i = 0; i < size; i++) {
		Point p1 = py.pts[i];
		IP ip;
		ip.pointFlag = 0;
		ip.p = p1;
		comlist.push_back(ip);
		list<IP> oneSeg;
		for (it = iplist.begin(); it != iplist.end(); it++) {
			if ((index == 0 && i == it->index0) ||
					(index == 1 && i == it->index1)) {
				it->dis = getDistance(it->p, p1);
				it->pointFlag = 1;
				oneSeg.push_back(*it);
			}
		}
		oneSeg.sort(distanceComparator);
		for (it = oneSeg.begin(); it != oneSeg.end(); it++)
			comlist.push_back(*it);
	}
}
void getPolygonPointInOut(list<IP>& polygonlist, Polygon& pyclip) {    // this function operates on the list of points' coordinates and srts the inflag to either true or false depending on whether the coordinate is inside the clipping polygon or not.
	bool inFlag;
	list<IP>::iterator it;
	for (it = polygonlist.begin(); it != polygonlist.end(); it++) {
		if (it->pointFlag == 0) {
			if (isPointInsidePolygon(it->p, pyclip))
				inFlag = true;
			else inFlag = false;
		}
		else {
			inFlag = !inFlag;
			it->inFlag = inFlag;
		}
	}
}
bool operator==(Point& p1, Point& p2) {   // This functions checks if two points are same or not.
	return p1.x == p2.x && p1.y == p2.y;
} 
void getClipPointInOut(list<IP>& cliplist, list<IP>& polygonlist) {
	list<IP>::iterator it, it1;
	for (it = cliplist.begin(); it != cliplist.end(); it++) {
		if (it->pointFlag == 0) continue;
		for (it1 = polygonlist.begin(); it1 != polygonlist.end(); it1++) {
			if (it1->pointFlag == 0) continue;
			if (it->p == it1->p) it->inFlag = it1->inFlag;
		}
	}
}
void generateClipArea(list<IP>& polygonlist, list<IP>& cliplist){   // This functions generates the clipped area that is obtained by clipping the given polygon.
	list<IP>::iterator it, it1;
	Polygon py;
	Color c = {0.0, 0.0, 1.0};

	for (it = polygonlist.begin(); it != polygonlist.end(); it++)
		if (it->pointFlag == 1 && it->inFlag) break;
	py.pts.clear();

	while(true) {

		if (it == polygonlist.end()) break;
		py.pts.push_back(it->p);
		for (; it != polygonlist.end(); it++) {
			if (it->pointFlag == 1 && !it->inFlag) break;
			py.pts.push_back(it->p);
		}
		for (it1 = cliplist.begin(); it1 != cliplist.end(); it1++)
			if (it1->p == it->p) break;

		for (; it1 != cliplist.end(); it1++) {
			if (it1->pointFlag == 1 && it1->inFlag) break;
			py.pts.push_back(it1->p);
		}
		if (py.pts[0] == it1->p) {
			py.drawPolygonLine(c);
			py.pts.clear();
			for (; it != polygonlist.end(); it++)
				if (it->pointFlag == 1 && it->inFlag) break;
			continue;
		}
		for (; it != polygonlist.end(); it++)
			if (it->p == it1->p) break;
	}
}
void weilerAtherton(Polygon& pyclip, Polygon& py){   // This function implements the Weiler Atherton Polygon Clipping Algorithm.
	list<IP> iplist, polygonlist, cliplist;
	generateIntersectPoints(pyclip, py, iplist);

	generateList(py, iplist, polygonlist, 0);
	generateList(pyclip, iplist, cliplist, 1);

	getPolygonPointInOut(polygonlist, pyclip);

	getClipPointInOut(cliplist, polygonlist);

	generateClipArea(polygonlist, cliplist);
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, Size - 1, 0.0, Size - 1);
}

void display() // This function displays the polygons on the display screen.
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_POINT_SMOOTH);
	Polygon pyclip, py;
	Point p1, p2, p3, p4;
	p1.x = 100 , p1.y = 100;
        p2.x = 400, p2.y = 378;
        p3.x = 486, p3.y = 170;
        p4.x = 610, p4.y = 86;

/*	p1.x = 553, p1.y = 495;
	p2.x = 351, p2.y = 175;
	p3.x = 486, p3.y = 71;
	p4.x = 61, p4.y = 86;*/
	pyclip.pts.push_back(p1);
	pyclip.pts.push_back(p2);
	pyclip.pts.push_back(p3);
	pyclip.pts.push_back(p4);
    
	Point p5, p6, p7, p8;
	p5.x = 390, p5.y = 424;
        p6.x = 579, p6.y = 585;
        p7.x = 257, p7.y = 112;
        p8.x = 68, p8.y = 245;

	/*
	p5.x = 400, p5.y = 224;
	p6.x = 57, p6.y = 585;
	p7.x = 257, p7.y = 112;
	p8.x = 68, p8.y = 145;*/
	py.pts.push_back(p5);
	py.pts.push_back(p6);
	py.pts.push_back(p7);
	py.pts.push_back(p8);

	int size = pyclip.pts.size();
	for (int i = 0; i < size; ++ i)
		cout << pyclip.pts[i].x << " " << pyclip.pts[i].y << endl;
	cout << endl;
	size = py.pts.size();
	for (int i = 0; i < size; ++ i)
		cout << py.pts[i].x << " " << py.pts[i].y << endl;

	Color a = {1.0, 0.0, 0.0};
	Color b = {0.0, 1.0, 0.0};
	py.drawPolygonLine(a);
	pyclip.drawPolygonLine(b);
    weilerAtherton(pyclip, py);

	glFlush();
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(Size, Size);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Weiler-Atherton Clipping Algorithm");
	//glutKeyboardFunc(KeyboardAction);
	glutDisplayFunc(display); // calls the display function.

	init();

	glutMainLoop();

	return 0;
} 
