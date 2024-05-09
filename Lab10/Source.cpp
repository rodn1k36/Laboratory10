#include <iostream>
#include "glut.h"
#include <vector>

static int menu_id;
static int submenu_shape_id;
static int submenu_polygon_id;
static int submenu_ellipse_id;
static int submenu_circle_id;
static int submenu_rectangle_id;
static int submenu_square_id;
static int submenu_triangle_id;

static int value = 0;

bool is_create_polygon = false;
bool is_create_ellypse = false;
bool is_create_circle = false;
bool is_create_square = false;
bool is_create_triangle = false;


struct Point {
	double x = 0;
	double y = 0;
	Point() = default;
	Point(double x, double y) : x(x), y(y) {};
	bool operator== (const Point& obj) const {
		return x == obj.x && y == obj.y;
	}
	bool operator!= (const Point& obj) const {
		return !(*this == obj);
	}
};

class Line {
private:
	Point P1;
	Point P2;
public:
	Line(const Point& P1, const Point& P2) : P1(P1), P2(P2) {
		if (P1.x == P2.x && P1.y == P2.y)
			throw 1;
	}
	Line(const double k, const double b) {
		P1.x = 0;
		P1.y = k * P1.x + b;
	}
	Line(const Point& P, const double k) : P1(P) {
		double b = P.y - k * P.x;
		P2.x = P1.x + 10;
		P2.y = k * P2.x + b;
	}
	bool operator== (const Line& L) const {
		if ((L.P1.x - P1.x) / (P2.x - P1.x) == ((L.P1.y - P1.x) / (P2.y - P1.y))
			&& (L.P2.x - P1.x) / (P2.x - P1.x) == ((L.P2.y - P1.x) / (P2.y - P1.y)))
			return true;
		return false;
	}
	bool operator!= (const Line& L) const {
		return !(*this == L);
	}
	std::pair<Point, Point> getPoints() {
		return {P1, P2};
	}
};

class Shape {
public:
	virtual void Draw() = 0;
	virtual void Rotate(Point center, double angle) = 0;
	virtual void Reflex(Point center) = 0;
	virtual void Reflex(Line axis) = 0;
	virtual void Scale(Point center, double coefficient) = 0;
} *obj[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

class Polygon : public Shape {
protected:
	std::vector <Point> vertices;
public:
	Polygon(std::vector <Point> vertices) : vertices(vertices) { };
	/*Polygon(...) {
		va_list args;
	}*/
	void Draw() override {
		glColor3f(1, 0, 0);
		glBegin(GL_POLYGON);
		for (int i = 0; i < vertices.size(); ++i)
			glVertex2f(vertices[i].x, vertices[i].y);
		glEnd();
	}

	void Rotate(Point center, double angle) override {

		angle *= -180 / 3.14;

		for (int i = 0; i < vertices.size(); ++i) {
			double newX = vertices[i].x * cos(angle) - vertices[i].y * sin(angle);
			double newY = vertices[i].x * sin(angle) + vertices[i].y * cos(angle);
			vertices[i].x = newX;
			vertices[i].y = newY;
		}
	}

	void Reflex(Point center) override {
		for (int i = 0; i < vertices.size(); ++i) {
			double deltaX = vertices[i].x - center.x;
			double deltaY = vertices[i].y - center.y;
			vertices[i] = Point(center.x - deltaX, center.y - deltaY);
		}
	}

	void Reflex(Line axis) override {
		for (int i = 0; i < vertices.size(); i++) {
			double x = vertices[i].x;
			double y = vertices[i].y;

			Point P1(axis.getPoints().first);
			Point P2(axis.getPoints().second);
			
			double A = P2.y - P1.y;
			double B = P1.x - P2.x;
			double C = -A * P1.x - B * P1.y;

			double new_x = x - 2 * (A * x + B * y + C) / (A * A + B * B) * A;
			double new_y = y - 2 * (A * x + B * y + C) / (A * A + B * B) * B;

			vertices[i] = Point(new_x, new_y);
		}
	}

	void Scale(Point center, double coefficient) override {
		for (int i = 0; i < vertices.size(); i++) {
			vertices[i].x = center.x + (vertices[i].x - center.x) * coefficient;
			vertices[i].y = center.y + (vertices[i].y - center.y) * coefficient;
		}
	}

	size_t verticesCount() {
		return vertices.size();
	}
	std::vector<Point> getVertices() const {
		return vertices;
	}

	bool isConvex() {
		double SumAngle = 0;
		for (int i = 0; i < vertices.size(); ++i) {
			// угол A B C ( B в центре, A предыдушая точка, В следующая )
			if (i == 0)
			{
				double BA[2] = { vertices[vertices.size() - 1].x - vertices[i].x,
				vertices[vertices.size() - 1].y - vertices[i].y };
				double BC[2] = { vertices[i + 1].x - vertices[i].x,
				vertices[i + 1].y - vertices[i].y };
				double scalar_BA_BC = BA[0] * BC[0] + BA[1] * BC[1];
				double lenBA = sqrt(BA[0] * BA[0] + BA[1] * BA[1]);
				double lenBC = sqrt(BC[0] * BC[0] + BC[1] * BC[1]);
				SumAngle += acos(scalar_BA_BC / lenBA * lenBC);
			}
			else if (i == vertices.size())
			{
				double BA[2] = { vertices[i - 1].x - vertices[i].x,
				vertices[i - 1].y - vertices[i].y };
				double BC[2] = { vertices[0].x - vertices[i].x,
				vertices[0].y - vertices[i].y };
				double scalar_BA_BC = BA[0] * BC[0] + BA[1] * BC[1];
				double lenBA = sqrt(BA[0] * BA[0] + BA[1] * BA[1]);
				double lenBC = sqrt(BC[0] * BC[0] + BC[1] * BC[1]);
				SumAngle += acos(scalar_BA_BC / lenBA * lenBC);
			}
			else {
				double BA[2] = { vertices[i - 1].x - vertices[i].x,
				vertices[i - 1].y - vertices[i].y };
				double BC[2] = { vertices[i + 1].x - vertices[i].x,
				vertices[i + 1].y - vertices[i].y };
				double scalar_BA_BC = BA[0] * BC[0] + BA[1] * BC[1];
				double lenBA = sqrt(BA[0] * BA[0] + BA[1] * BA[1]);
				double lenBC = sqrt(BC[0] * BC[0] + BC[1] * BC[1]);
				SumAngle += acos(scalar_BA_BC / lenBA * lenBC);
			}
		}
		return SumAngle * 180 / 3.1415 == 180;
	}
};

class Ellipse : public Shape {
private:
	std::pair<Point, Point> F;
	double r;
public:
	Ellipse(const Point& P1, const Point& P2, double r) : F({ P1, P2 }), r(r) {};

	void Draw() override {
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		double x, y;
		double a = sqrt(pow(abs((F.first.x - F.second.x) / 2), 2) + pow(abs((F.first.y - F.second.y) / 2), 2)); // big axis
		double b = sqrt(pow(r / 2, 2) - pow(a, 2)); // small axis
		double angle = atan(F.second.y / F.second.x);
		if (r < 2 * b)
			throw 1;

		for (int i = 1; i < 360; ++i) {
			x = a * cos(static_cast<double>(i) / 180 * 3.15);
			y = b * sin(static_cast<double>(i) / 180 * 3.15);

			double s = sin(angle);
			double c = cos(angle);

			double new_x = x * c - y * s;
			double new_y = x * s + y * c;

			glVertex2f(new_x, new_y);
		}

		glEnd();
	}

	void Rotate(Point center, double angle) override {

		F.first.x -= center.x;
		F.first.y -= center.y;
		F.second.x -= center.x;
		F.second.y -= center.y;

		double s = sin(angle / 180 * 3.15);
		double c = cos(angle / 180 * 3.15);

		double xnew_first = F.first.x * c - F.first.y * s;
		double ynew_first = F.first.x * s + F.first.y * c;
		F.first.x = xnew_first;
		F.first.y = ynew_first;

		double xnew_second = F.second.x * c - F.second.y * s;
		double ynew_second = F.second.x * s + F.second.y * c;
		F.second.x = xnew_second;
		F.second.y = ynew_second;

		F.first.x += center.x;
		F.first.y += center.y;
		F.second.x += center.x;
		F.second.y += center.y;
	}

	void Reflex(Point center) override {
		double deltaX = F.first.x - center.x;
		double deltaY =	F.first.y - center.y;
		F.first = Point(center.x - deltaX, center.y - deltaY);

		deltaX = F.second.x - center.x;
		deltaY = F.second.y - center.y;
		F.second = Point(center.x - deltaX, center.y - deltaY);
	}

	void Reflex(Line axis) override {
		double x = F.first.x;
		double y = F.first.y;

		Point P1(axis.getPoints().first);
		Point P2(axis.getPoints().second);

		double A = P2.y - P1.y;
		double B = P1.x - P2.x;
		double C = -A * P1.x - B * P1.y;

		double new_x = x - 2 * (A * x + B * y + C) / (A * A + B * B) * A;
		double new_y = y - 2 * (A * x + B * y + C) / (A * A + B * B) * B;

		F.first = Point(new_x, new_y);

		x = F.second.x;
		y = F.second.y;

		A = P2.y - P1.y;
		B = P1.x - P2.x;
		C = -A * P1.x - B * P1.y;

		new_x = x - 2 * (A * x + B * y + C) / (A * A + B * B) * A;
		new_y = y - 2 * (A * x + B * y + C) / (A * A + B * B) * B;

		F.second = Point(new_x, new_y);
	}

	void Scale(Point center, double coefficient) override {
		F.first.x = center.x + (F.first.x - center.x) * coefficient;
		F.first.y = center.y + (F.first.y - center.y) * coefficient;
		
		F.second.x = center.x + (F.second.x - center.x) * coefficient;
		F.second.y = center.y + (F.second.y - center.y) * coefficient;

		r *= coefficient;
	}

	std::pair<Point, Point> focuses() {
		return { F.first, F.second };
	}
	/*std::pair<Line, Line> directrices() {

	}
	double eccentricity() {

	}
	Point center() {

	} */
};

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(-200, 0);
	glVertex2f(200, 0);
	glVertex2f(0, -200);
	glVertex2f(0, 200);
	glEnd();

	if (value == 1) {
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex2f(-200, 0);
		glVertex2f(200, 0);
		glVertex2f(0, -200);
		glVertex2f(0, 200);
		glEnd();
		is_create_polygon = false;
		is_create_ellypse = false;

	}
	//////////////////// POLYGON ////////////////////////////
	if (value == 10) {
		Point a(-50, -50);
		Point b(-50, 0);
		Point c(0, 50);
		Point d(50, 0);
		Point e(50, -50);
		std::vector <Point> points = { a, b, c, d, e };
		Polygon* shape_polygon = new Polygon(points);
		obj[0] = shape_polygon;
		is_create_polygon = true;

		obj[0]->Draw();
	}

	else if (is_create_polygon) {

		obj[0]->Draw();

		if (value == 11) {
			obj[0]->Rotate({ 0,0 }, 15);
			value = -1;
		}
		else if (value == 12) {
			obj[0]->Reflex(Point(0,0));
			value = -1;
		}
		else if (value == 13) {
			obj[0]->Reflex(Line(Point(-30, 0), Point(30, 0)));
			value = -1;
		}
		else if (value == 14) {
			obj[0]->Scale(Point(0, 0), 0.5);
			value = -1;
		}
	}
	//////////////// ELLYPSE /////////////////////////////
	if (value == 20) {
		Point F1(-60, 0);
		Point F2(60, 0);
		double r = 140;
		Ellipse* shape_ellypse = new Ellipse(F1, F2, r); // как удалить?
		obj[1] = shape_ellypse;
		is_create_ellypse = true;

		shape_ellypse->Draw();
	}

	else if (is_create_ellypse) {

		obj[1]->Draw();

		if (value == 21) {
			obj[1]->Rotate({ 0, 0 }, 15);
			value = -1;
		}
		else if (value == 22) {
			obj[1]->Reflex(Point(0,0));
			value = -1;
		}
		else if (value == 23) {
			obj[1]->Reflex(Line(Point(-30, 0), Point(30, 0)));
			value = -1;
		}
		else if (value == 24) {
			obj[1]->Scale(Point(0,0), 0.7);
			value = -1;
		}
	}

	glutSwapBuffers();
}
///////////////////////// MENU /////////////////////
void menu(int num) {
	if (num == 0) {

		exit(0);
	}
	else {
		value = num;
	}
	glutPostRedisplay();
}

void createMenu(void) {

	submenu_polygon_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 10);
	glutAddMenuEntry("Rotate", 11);
	glutAddMenuEntry("Reflex point", 12);
	glutAddMenuEntry("Reflex line", 13);
	glutAddMenuEntry("Scale", 14);

	submenu_ellipse_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 20);
	glutAddMenuEntry("Rotate", 21);
	glutAddMenuEntry("Reflex point", 22);
	glutAddMenuEntry("Reflex line", 23);
	glutAddMenuEntry("Scale", 24);

	submenu_circle_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 30);
	glutAddMenuEntry("Radius", 31);

	submenu_rectangle_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 40);
	glutAddMenuEntry("Center", 41);
	glutAddMenuEntry("Diagonals", 42);

	submenu_square_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 50);
	glutAddMenuEntry("Circumscribed Circle", 51);
	glutAddMenuEntry("Inscribed Circle", 52);

	submenu_triangle_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 60);
	glutAddMenuEntry("Circumscribed Circle", 61);
	glutAddMenuEntry("Inscribed Circle", 62);
	glutAddMenuEntry("Centroid", 63);
	glutAddMenuEntry("Orthocenter", 64);
	glutAddMenuEntry("Euler Line", 65);
	glutAddMenuEntry("Euler Circle", 66);

	submenu_shape_id = glutCreateMenu(menu);
	glutAddSubMenu("Polygon", submenu_polygon_id);
	glutAddSubMenu("Ellipse", submenu_ellipse_id);
	glutAddSubMenu("Circle", submenu_circle_id);
	glutAddSubMenu("Rectangle", submenu_rectangle_id);
	glutAddSubMenu("Square", submenu_square_id);
	glutAddSubMenu("Triangle", submenu_triangle_id);

	menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Clear", 1);
	glutAddSubMenu("Shape edit", submenu_shape_id);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
////////////////////////////////////////////////////
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Laboratory №10");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-200, 200, -200, 200);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	createMenu();
	glClearColor(1, 1, 1, 0);
	glutMainLoop();

	return 0;
}