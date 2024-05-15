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
bool is_create_rectangle = false;
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

	virtual double perimeter() = 0;
	virtual double area() = 0;
	virtual bool operator==(const Shape& another) = 0;
	virtual bool isCongruentTo(const Shape& another) = 0;
	//virtual bool isSimilarTo(const Shape& another) = 0;

} *obj[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

class Polygon : public Shape {
protected:
	std::vector <Point> vertices;
public:
	Polygon() = default;
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
///////////////////////////////////////
	double perimeter() override {
		double P = 0;
		for (int i = 0; i < vertices.size() - 1; ++i) {
			P += sqrt(pow(vertices[i + 1].x - vertices[i].x, 2) + pow(vertices[i + 1].y - vertices[i].y, 2));
		}
		
		P += sqrt(pow(vertices[0].x - vertices[vertices.size() - 1].x, 2) + pow(vertices[0].y - vertices[vertices.size() - 1].y, 2));

		return P;
	}

	double area() override {
		int n = vertices.size();
		double area = 0;
		// формула √аусса вычислени€ площади многоугольника
		for (int i = 0; i < n; i++) {
			int j = (i + 1) % n;
			area += vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
		}
		area = std::abs(area) / 2.0;
		return area;
	}

	bool operator==(const Shape& another) override {
		if (dynamic_cast<const Polygon*>(&another) == nullptr) {
			return false;
		}
		else {
			const Polygon& another_polygon = dynamic_cast<const Polygon&>(another);

			if (this->vertices.size() != another_polygon.vertices.size()) return false;

			for (int i = 0; i < vertices.size(); ++i) {
				bool is_equal = true;
				for (int j = 0; j < vertices.size(); ++j) {
					if (this->vertices[j].x != another_polygon.vertices[(j + i) % this->vertices.size()].x ||
						this->vertices[j].y != another_polygon.vertices[(j + i) % this->vertices.size()].y) {
						is_equal = false;
						break;
					}
				}
				if (is_equal) return true;
			}
		}
		return false;
	}

	bool isCongruentTo(const Shape& another) override {
		if (dynamic_cast<const Polygon*>(&another) == nullptr) {
			return false;
		}
		else {
			const Polygon& another_polygon = dynamic_cast<const Polygon&>(another);

			if (this->vertices.size() != another_polygon.vertices.size()) return false;

			for (int i = 0; i < vertices.size(); ++i) {
				bool is_equal = true;
				for (int j = 0; j < vertices.size(); ++j) {
					if (abs(this->vertices[j].x - this->vertices[(j+1) % this->vertices.size()].x) !=
						abs(another_polygon.vertices[(j + i) % this->vertices.size()].x - another_polygon.vertices[(j + i + 1) % this->vertices.size()].x) ||
						abs(this->vertices[j].y - this->vertices[(j + 1) % this->vertices.size()].y) !=
						abs(another_polygon.vertices[(j + i) % this->vertices.size()].y - another_polygon.vertices[(j + i + 1) % this->vertices.size()].y)) {
						is_equal = false;
						break;
					}
				}
				if (is_equal) return true;
			}
		}
		return false;
	}

	//bool isSimilarTo(const Shape& another) override {
	//	if (dynamic_cast<const Polygon*>(&another) == nullptr) {
	//		return false;
	//	}
	//	const Polygon& another_polygon = dynamic_cast<const Polygon&>(another);

	//	if (this->vertices.size() != another_polygon.vertices.size()) return false;

	//	for (int i = 0; i < vertices.size(); ++i) {
	//		bool is_equal = true;
	//		double simular_k = sqrt(pow(this->vertices[0].x - this->vertices[1].x, 2) + pow(this->vertices[0].y - this->vertices[1].y, 2)) /
	//			sqrt(pow(another_polygon.vertices[0].x - another_polygon.vertices[1].x, 2) + pow(another_polygon.vertices[0].y - another_polygon.vertices[1].y, 2)); // отношение сторон фигур
	//		for (int j = 0; j < vertices.size(); ++j) {
	//			
	//		}
	//		if (is_equal) return true;
	//	}
	//	
	//}
///////////////////////////////////////
	size_t verticesCount() {
		return vertices.size();
	}

	std::vector<Point> getVertices() const {
		return vertices;
	}

	bool isConvex() {
		double SumAngle = 0;
		for (int i = 0; i < vertices.size(); ++i) {
			// угол A B C ( B в центре, A предыдуша€ точка, ¬ следующа€ )
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
protected:
	std::pair<Point, Point> F;
	double r;
public:
	Ellipse(const Point& P1, const Point& P2, const double r) : F({ P1, P2 }), r(r) {};

	void Draw() override {
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		double x, y;
		double a = r/2 - sqrt(pow(abs((F.first.x - F.second.x) / 2), 2) + pow(abs((F.first.y - F.second.y) / 2), 2)); // big axis
		double b = sqrt(pow(r / 2, 2) - (sqrt(pow(abs((F.first.x - F.second.x) / 2), 2) + pow(abs((F.first.y - F.second.y) / 2), 2)))); // small axis
		double angle = 0;
		if (F.second.x != 0)
			angle = atan(F.second.y / F.second.x);
		
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
////////////////////////////////////////
	double perimeter() override {
		double a = r / 2 - sqrt(pow(abs((F.first.x - F.second.x) / 2), 2) + pow(abs((F.first.y - F.second.y) / 2), 2)); // big axis
		double b = sqrt(pow(r / 2, 2) - (sqrt(pow(abs((F.first.x - F.second.x) / 2), 2) + pow(abs((F.first.y - F.second.y) / 2), 2)))); // small axis

		return 4 * (3.14 * a * b + pow(a - b, 2)) / (a + b);
	}

	double area() override {
		double a = r / 2 - sqrt(pow(abs((F.first.x - F.second.x) / 2), 2) + pow(abs((F.first.y - F.second.y) / 2), 2)); // big axis
		double b = sqrt(pow(r / 2, 2) - (sqrt(pow(abs((F.first.x - F.second.x) / 2), 2) + pow(abs((F.first.y - F.second.y) / 2), 2)))); // small axis

		return 3.14 * a * b;
	}

	bool operator==(const Shape& another) override {
		if (dynamic_cast<const Ellipse*>(&another) == nullptr) {
			return false;
		}
		
		const Ellipse& another_ellipse = dynamic_cast<const Ellipse&>(another);

		if (this->F.first.x == another_ellipse.F.first.x &&
			this->F.first.y == another_ellipse.F.first.y &&
			this->F.second.x == another_ellipse.F.second.x &&
			this->F.second.x == another_ellipse.F.second.x)
			return true;

		return false;

	}

	bool isCongruentTo(const Shape& another) override {
		return false;
	}

///////////////////////////////////////
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

class Circle : public Ellipse {
public:
	Circle(const Point& center, const double r) : Ellipse(center, center, r) {}
	double radius() {
		return r;
	}
};

class Rectangle : public Polygon {
public:
	Rectangle(const Point& P1, const Point& P2) {
		Point P3(P1.x, P2.y);
		Point P4(P2.x, P1.y);
		vertices = {P1, P3, P2, P4}; // P1 left down point и по часовой
	}

	Point center() {
		return Point(vertices[0].x + abs(vertices[3].x - vertices[0].x), vertices[2].y - abs(vertices[2].y - vertices[3].y)); // [0] индекс нижн€€ точка слева и далее по часовой пронумерованы
	}

	std::pair<Line, Line> diagonals() {
		return { Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3]) };
	}
};

class Square : public Rectangle {
public:
	Square(const Point& P1, const Point& P2) : Rectangle(P1, P2) {
		if (abs(P1.x - P2.x) != abs(P1.y - P2.y))
			throw 1;
	}
};

class Triangle : public Polygon {
public:
	Triangle(const Point& P1, const Point& P2, const Point& P3) : Polygon(std::vector <Point> {P1, P2, P3}) {}
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
		is_create_circle = false;
		is_create_rectangle = false;
		is_create_square = false;
		is_create_triangle = false;
		
	}
	//////////////////// POLYGON ////////////////////////////
	if (value == 10) {
		std::vector <Point> points;
		int count = 0;
		char off = ' ';
		while (off != 'e') {
			int x, y;
			std::cout << "Enter Point coordinate (x, y): \n";
			std::cin >> x;
			std::cin >> y;
			points.push_back(Point(x, y));
			++count;
			if (count > 2) {
				std::cout << "enter any key to continue, enter (e) to exit: \n";
				std::cin >> off;
				value = -1;
			}
		}
		Polygon* shape_polygon = new Polygon(points);
		obj[0] = shape_polygon;
		is_create_polygon = true;

		obj[0]->Draw();
	}

	else if (is_create_polygon) {

		obj[0]->Draw();

		if (value == 11) {
			int x, y;
			std::cout << "Enter rotate coordinate x, y: \n";
			std::cin >> x >> y;
			int angle;
			std::cout << "Enter Angle: \n";
			std::cin >> angle;
			obj[0]->Rotate(Point(x, y), angle);
			value = -1;
		}
		else if (value == 12) {
			int x, y;
			std::cout << "Enter reflex coordinate x, y: \n";
			std::cin >> x >> y;
			obj[0]->Reflex(Point(x, y));
			value = -1;
		}
		else if (value == 13) {
			int x1, y1, x2, y2;
			std::cout << "Enter Points of Line: x1, y1 and x2, y2: \n";
			std::cin >> x1 >> y1 >> x2 >> y2;
			obj[0]->Reflex(Line(Point(x1, y1), Point(x2, y2)));
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
		double r = 40;
		Ellipse* shape_ellypse = new Ellipse(F1, F2, r); // как удалить?
		obj[1] = shape_ellypse;
		is_create_ellypse = true;

		shape_ellypse->Draw();
	}

	else if (is_create_ellypse) {

		obj[1]->Draw();

		if (value == 21) {
			obj[1]->Rotate(Point(0, 0), 15);
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
	/////////////////////////// CIRCLE ////////////////////////////////////
	if (value == 30) {
		Point center(0, 0);
		double r = 60;
		Circle* shape_circle = new Circle(center, r); // как удалить?
		obj[2] = shape_circle;
		is_create_circle = true;

		shape_circle->Draw();
	}

	else if (is_create_circle) {

		obj[2]->Draw();

		if (value == 31) {
			obj[2]->Rotate({ 0, 0 }, 15);
			value = -1;
		}
		else if (value == 32) {
			obj[2]->Reflex(Point(0, 0));
			value = -1;
		}
		else if (value == 33) {
			obj[2]->Reflex(Line(Point(-30, 0), Point(30, 0)));
			value = -1;
		}
		else if (value == 34) {
			obj[2]->Scale(Point(0, 0), 0.7);
			value = -1;
		}
	}
	///////////////////////// RECTANGLE ////////////////////////
	if (value == 40) {
		Rectangle* shape_rectangle = new Rectangle(Point(-60,-30), Point(60, 30)); // как удалить?
		obj[3] = shape_rectangle;
		is_create_rectangle = true;

		shape_rectangle->Draw();
	}

	else if (is_create_rectangle) {

		obj[3]->Draw();

		if (value == 41) {
			obj[3]->Rotate({ 0, 0 }, 15);
			value = -1;
		}
		else if (value == 42) {
			obj[3]->Reflex(Point(0, 0));
			value = -1;
		}
		else if (value == 43) {
			obj[3]->Reflex(Line(Point(-30, 0), Point(30, 0)));
			value = -1;
		}
		else if (value == 44) {
			obj[3]->Scale(Point(0, 0), 0.7);
			value = -1;
		}
	}
	///////////////////////// SQUARE ////////////////////////////

	if (value == 50) {
		Square* shape_square = new Square(Point(-30, -30), Point(30, 30)); // как удалить?
		obj[4] = shape_square;
		is_create_square = true;

		shape_square->Draw();
	}

	else if (is_create_square) {

		obj[4]->Draw();

		if (value == 51) {
			obj[4]->Rotate({ 0, 0 }, 15);
			value = -1;
		}
		else if (value == 52) {
			obj[4]->Reflex(Point(0, 0));
			value = -1;
		}
		else if (value == 53) {
			obj[4]->Reflex(Line(Point(-30, 0), Point(30, 0)));
			value = -1;
		}
		else if (value == 54) {
			obj[4]->Scale(Point(0, 0), 0.7);
			value = -1;
		}
	}
	//////////////////////////// TRIANGLE //////////////////////

	if (value == 60) {
		Triangle* shape_triangle = new Triangle(Point(-30, -30), Point(30, -30), Point(0, 30)); // как удалить?
		obj[5] = shape_triangle;
		is_create_triangle = true;

		shape_triangle->Draw();
	}

	else if (is_create_triangle) {

		obj[5]->Draw();

		if (value == 61) {
			obj[5]->Rotate({ 0, 0 }, 15);
			value = -1;
		}
		else if (value == 62) {
			obj[5]->Reflex(Point(0, 0));
			value = -1;
		}
		else if (value == 63) {
			obj[5]->Reflex(Line(Point(-30, 0), Point(30, 0)));
			value = -1;
		}
		else if (value == 64) {
			obj[5]->Scale(Point(0, 0), 0.7);
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
	glutAddMenuEntry("Rotate", 31);
	glutAddMenuEntry("Reflex point", 32);
	glutAddMenuEntry("Reflex line", 33);
	glutAddMenuEntry("Scale", 34);

	submenu_rectangle_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 40);
	glutAddMenuEntry("Rotate", 41);
	glutAddMenuEntry("Reflex point", 42);
	glutAddMenuEntry("Reflex line", 43);
	glutAddMenuEntry("Scale", 44);

	submenu_square_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 50);
	glutAddMenuEntry("Rotate", 51);
	glutAddMenuEntry("Reflex point", 52);
	glutAddMenuEntry("Reflex line", 53);
	glutAddMenuEntry("Scale", 54);

	submenu_triangle_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 60);
	glutAddMenuEntry("Rotate", 61);
	glutAddMenuEntry("Reflex point", 62);
	glutAddMenuEntry("Reflex line", 63);
	glutAddMenuEntry("Scale", 64);

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
	glutCreateWindow("Laboratory є10");
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