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
	virtual bool isSimilarTo(const Shape& another) = 0;
	virtual bool containsPoint(Point point) = 0;
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
		// формула Гаусса вычисления площади многоугольника
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

	bool isSimilarTo(const Shape& another) override {
		if (dynamic_cast<const Polygon*>(&another) == nullptr) {
			return false;
		}
		const Polygon& another_polygon = dynamic_cast<const Polygon&>(another);

		if (this->vertices.size() != another_polygon.vertices.size()) return false;

		for (int i = 0; i < vertices.size(); ++i) {
			bool is_equal = true;
			double simular_k = sqrt(pow(this->vertices[0].x - this->vertices[1].x, 2) + pow(this->vertices[0].y - this->vertices[1].y, 2)) /
				sqrt(pow(another_polygon.vertices[0].x - another_polygon.vertices[1].x, 2) + pow(another_polygon.vertices[0].y - another_polygon.vertices[1].y, 2)); // отношение сторон фигур
			for (int j = 0; j < vertices.size(); ++j) {
				
			}
			if (is_equal) return true;
		}
		
	}

	bool containsPoint(Point point) override {
		int n = verticesCount();
		bool inside = false;

		for (int i = 0, j = n - 1; i < n; j = i++) {
			if ((vertices[i].y > point.y) != (vertices[j].y > point.y) &&
				(point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
				inside = !inside;
			}
		} // если находится справа и имеет другую высоту то флаг инвертируется, луч из точки должен пересечьнечетное кол-во вершин чтобы быть внутри фигуры
		return inside;
	}
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
protected:
	std::pair<Point, Point> F;
	double r;
public:
	Ellipse(const Point& P1, const Point& P2, const double r) : F({ P1, P2 }), r(r) {};

	void Draw() override {
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		double x, y;
		double a = sqrt(pow(abs(F.first.x - F.second.x) / 2, 2) + pow(abs(F.first.y - F.second.y) / 2, 2)); // big axis
		double b = r / 2; // small axis

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

			double new_x = x * c - y * s + (F.first.x + F.second.x) / 2;
			double new_y = x * s + y * c + (F.first.y + F.second.y) / 2;

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
			this->F.second.y == another_ellipse.F.second.y)
			return true;

		return false;
	}

	bool isCongruentTo(const Shape& another) override {
		if (dynamic_cast<const Ellipse*>(&another) == nullptr) {
			return false;
		}
		
		const Ellipse& another_ellipse = dynamic_cast<const Ellipse&>(another);

		if (sqrt(pow(abs(this->F.first.x - this->F.second.x) / 2, 2) + pow(abs(this->F.first.y - this->F.second.y) / 2, 2)) ==
			sqrt(pow(abs(another_ellipse.F.first.x - another_ellipse.F.second.x) / 2, 2) + pow(abs(another_ellipse.F.first.y - another_ellipse.F.second.y) / 2, 2)) &&
			this->r / 2 == another_ellipse.r / 2) { return true; }
		return false;
	}

	bool isSimilarTo(const Shape& another) override {
		if (dynamic_cast<const Ellipse*>(&another) == nullptr) {
			return false;
		}

		const Ellipse& another_ellipse = dynamic_cast<const Ellipse&>(another);

		if (sqrt(pow(abs(this->F.first.x - this->F.second.x) / 2, 2) + pow(abs(this->F.first.y - this->F.second.y) / 2, 2)) /
			sqrt(pow(abs(another_ellipse.F.first.x - another_ellipse.F.second.x) / 2, 2) + pow(abs(another_ellipse.F.first.y - another_ellipse.F.second.y) / 2, 2)) ==
			r / 2 == another_ellipse.r / 2 ) {
			return true; // a относится к а с таким же коэффициентом как b к b
		}
		return false;
	}

	bool containsPoint(Point point) override {
		double a = sqrt(pow(abs(F.first.x - F.second.x) / 2, 2) + pow(abs(F.first.y - F.second.y) / 2, 2)); // big axis
		double b = r / 2; // small axis

		double result = (point.x * point.x) / (a * a) + (point.y * point.y) / (b * b);

		if (result < 1) {
			return true;
		}
		else {
			return false;
		}
	}

///////////////////////////////////////
	std::pair<Point, Point> focuses() {
		return { F.first, F.second };
	}
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
		return Point(vertices[0].x + abs(vertices[3].x - vertices[0].x), vertices[2].y - abs(vertices[2].y - vertices[3].y)); // [0] индекс нижняя точка слева и далее по часовой пронумерованы
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
			double x, y;
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
			double x, y;
			std::cout << "Enter rotate coordinate x, y: \n";
			std::cin >> x >> y;
			double angle;
			std::cout << "Enter Angle: \n";
			std::cin >> angle;
			obj[0]->Rotate(Point(x, y), angle);
			value = -1;
		}
		else if (value == 12) {
			double x, y;
			std::cout << "Enter reflex coordinate x, y: \n";
			std::cin >> x >> y;
			obj[0]->Reflex(Point(x, y));
			value = -1;
		}
		else if (value == 13) {
			double x1, y1, x2, y2;
			std::cout << "Enter Points of Line: x1, y1 and x2, y2: \n";
			std::cin >> x1 >> y1 >> x2 >> y2;
			obj[0]->Reflex(Line(Point(x1, y1), Point(x2, y2)));
			value = -1;
		}
		else if (value == 14) {
			double x, y, k;
			std::cout << "Enter Scale point x, y: \n";
			std::cin >> x >> y;
			std::cout << "Enter coefficient Scale: \n";
			std::cin >> k;
			obj[0]->Scale(Point(x, y), k);
			value = -1;
		}
		else if (value == 15) {
			std::cout << obj[0]->perimeter() << '\n';
			value = -1;
		}
		else if (value == 16) {
			std::cout << obj[0]->area() << '\n';
			value = -1;
		}
		else if (value == 17) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << (obj[0] == obj[num]) << '\n';
			value = -1;
		}
		else if (value == 18) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[0]->isCongruentTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 19) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[0]->isSimilarTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 20) {
			double x, y;
			std::cout << "Enter x, y \n"; 
			std::cin >> x >> y;
			std::cout << obj[0]->containsPoint(Point(x, y)) << '\n';
		}
	}
	////////////////////////// ELLYPSE /////////////////////////////
	if (value == 21) {
		double x1, y1, x2, y2, r;
		std::cout << "Enter Points of F1, F2: x1, y1 and x2, y2: \n";
		std::cin >> x1 >> y1 >> x2 >> y2;
		Point F1(x1, y1);
		Point F2(x2, y2);
		std::cout << "Enter r:'\n";
		std::cin >> r;

		Ellipse* shape_ellypse = new Ellipse(F1, F2, r); 
		obj[1] = shape_ellypse;

		is_create_ellypse = true;
		value = -1;

		shape_ellypse->Draw();
	}

	else if (is_create_ellypse) {

		obj[1]->Draw();

		if (value == 22) {
			double x, y;
			std::cout << "Enter rotate coordinate x, y: \n";
			std::cin >> x >> y;
			double angle;
			std::cout << "Enter Angle: \n";
			std::cin >> angle;
			obj[1]->Rotate(Point(x, y), angle);
			value = -1;
		}
		else if (value == 23) {
			double x, y;
			std::cout << "Enter reflex coordinate x, y: \n";
			std::cin >> x >> y;
			obj[1]->Reflex(Point(x, y));
			value = -1;
		}
		else if (value == 24) {
			double x1, y1, x2, y2;
			std::cout << "Enter Points of Line: x1, y1 and x2, y2: \n";
			std::cin >> x1 >> y1 >> x2 >> y2;
			obj[1]->Reflex(Line(Point(x1, y1), Point(x2, y2)));
			value = -1;
		}
		else if (value == 25) {
			double x, y, k;
			std::cout << "Enter Scale point x, y: \n";
			std::cin >> x >> y;
			std::cout << "Enter coefficient Scale: \n";
			std::cin >> k;
			obj[1]->Scale(Point(x, y), k);
			value = -1;
		}
		
		else if (value == 26) {
			std::cout << obj[1]->perimeter() << '\n';
			value = -1;
		}
		else if (value == 27) {
			std::cout << obj[1]->area() << '\n';
			value = -1;
		}
		else if (value == 28) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << (obj[1] == obj[num]) << '\n';
			value = -1;
		}
		else if (value == 29) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[1]->isCongruentTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 30) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[1]->isSimilarTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 31) {
			double x, y;
			std::cout << "Enter x, y \n";
			std::cin >> x >> y;
			std::cout << obj[1]->containsPoint(Point(x, y)) << '\n';
		}
	}
	/////////////////////////// CIRCLE ////////////////////////////////////
	if (value == 32) {
		double x, y;
		std::cout << "Enter center x, y: \n";
		std::cin >> x >> y;
		double r;
		std::cout << "Enter radius: \n";
		std::cin >> r;
		Circle* shape_circle = new Circle(Point(x, y), r);
		obj[2] = shape_circle;
		is_create_circle = true;
		value = -1;

		shape_circle->Draw();
	}

	else if (is_create_circle) {

		obj[2]->Draw();

		if (value == 33) {
			double x, y;
			std::cout << "Enter rotate coordinate x, y: \n";
			std::cin >> x >> y;
			double angle;
			std::cout << "Enter Angle: \n";
			std::cin >> angle;
			obj[2]->Rotate(Point(x, y), angle);
			value = -1;
		}
		else if (value == 34) {
			double x, y;
			std::cout << "Enter reflex coordinate x, y: \n";
			std::cin >> x >> y;
			obj[2]->Reflex(Point(x, y));
			value = -1;
		}
		else if (value == 35) {
			double x1, y1, x2, y2;
			std::cout << "Enter Points of Line: x1, y1 and x2, y2: \n";
			std::cin >> x1 >> y1 >> x2 >> y2;
			obj[2]->Reflex(Line(Point(x1, y1), Point(x2, y2)));
			value = -1;
		}
		else if (value == 36) {
			double x, y, k;
			std::cout << "Enter Scale point x, y: \n";
			std::cin >> x >> y;
			std::cout << "Enter coefficient Scale: \n";
			std::cin >> k;
			obj[2]->Scale(Point(x, y), k);
			value = -1;
		}
		else if (value == 37) {
			std::cout << obj[2]->perimeter() << '\n';
			value = -1;
		}
		else if (value == 38) {
			std::cout << obj[2]->area() << '\n';
			value = -1;
		}
		else if (value == 39) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << (obj[2] == obj[num]) << '\n';
			value = -1;
		}
		else if (value == 40) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[2]->isCongruentTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 41) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[2]->isSimilarTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 42) {
			double x, y;
			std::cout << "Enter x, y \n";
			std::cin >> x >> y;
			std::cout << obj[2]->containsPoint(Point(x, y)) << '\n';
		}
	}
	///////////////////////// RECTANGLE ////////////////////////
	if (value == 43) {
		double x1, y1, x2, y2;
		std::cout << "Enter Point (left down): \n";
		std::cin >> x1 >> y1;
		std::cout << "Enter Point (right up): \n";
		std::cin >> x2 >> y2;
		Rectangle* shape_rectangle = new Rectangle(Point(x1,y1), Point(x2, y2)); // как удалить?
		obj[3] = shape_rectangle;
		is_create_rectangle = true;
		value = -1;
		
		shape_rectangle->Draw();
	}

	else if (is_create_rectangle) {

		obj[3]->Draw();

		if (value == 44) {
			double x, y;
			std::cout << "Enter rotate coordinate x, y: \n";
			std::cin >> x >> y;
			double angle;
			std::cout << "Enter Angle: \n";
			std::cin >> angle;
			obj[3]->Rotate(Point(x, y), angle);
			value = -1;
		}
		else if (value == 45) {
			double x, y;
			std::cout << "Enter reflex coordinate x, y: \n";
			std::cin >> x >> y;
			obj[3]->Reflex(Point(x, y));
			value = -1;
		}
		else if (value == 46) {
			double x1, y1, x2, y2;
			std::cout << "Enter Points of Line: x1, y1 and x2, y2: \n";
			std::cin >> x1 >> y1 >> x2 >> y2;
			obj[3]->Reflex(Line(Point(x1, y1), Point(x2, y2)));
			value = -1;
		}
		else if (value == 47) {
			double x, y, k;
			std::cout << "Enter Scale point x, y: \n";
			std::cin >> x >> y;
			std::cout << "Enter coefficient Scale: \n";
			std::cin >> k;
			obj[3]->Scale(Point(x, y), k);
			value = -1;
		}
		else if (value == 48) {
			std::cout << obj[3]->perimeter() << '\n';
			value = -1;
		}
		else if (value == 49) {
			std::cout << obj[3]->area() << '\n';
			value = -1;
		}
		else if (value == 50) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << (obj[3] == obj[num]) << '\n';
			value = -1;
		}
		else if (value == 51) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[3]->isCongruentTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 52) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[3]->isSimilarTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 53) {
			double x, y;
			std::cout << "Enter x, y \n";
			std::cin >> x >> y;
			std::cout << obj[3]->containsPoint(Point(x, y)) << '\n';
		}
	}
	///////////////////////// SQUARE ////////////////////////////

	if (value == 54) {
		double x1, y1, x2, y2;
		std::cout << "Enter Point (left down): \n";
		std::cin >> x1 >> y1;
		std::cout << "Enter Point (right up): \n";
		std::cin >> x2 >> y2;
		Square* shape_square = new Square(Point(x1, y1), Point(x2, y2));
		obj[4] = shape_square;
		is_create_square = true;
		value = -1;

		shape_square->Draw();
	}

	else if (is_create_square) {

		obj[4]->Draw();

		if (value == 55) {
			double x, y;
			std::cout << "Enter rotate coordinate x, y: \n";
			std::cin >> x >> y;
			double angle;
			std::cout << "Enter Angle: \n";
			std::cin >> angle;
			obj[4]->Rotate(Point(x, y), angle);
			value = -1;
		}
		else if (value == 56) {
			double x, y;
			std::cout << "Enter reflex coordinate x, y: \n";
			std::cin >> x >> y;
			obj[4]->Reflex(Point(x, y));
			value = -1;
		}
		else if (value == 57) {
			double x1, y1, x2, y2;
			std::cout << "Enter Points of Line: x1, y1 and x2, y2: \n";
			std::cin >> x1 >> y1 >> x2 >> y2;
			obj[4]->Reflex(Line(Point(x1, y1), Point(x2, y2)));
			value = -1;
		}
		else if (value == 58) {
			double x, y, k;
			std::cout << "Enter Scale point x, y: \n";
			std::cin >> x >> y;
			std::cout << "Enter coefficient Scale: \n";
			std::cin >> k;
			obj[4]->Scale(Point(x, y), k);
			value = -1;
		}
		else if (value == 59) {
			std::cout << obj[4]->perimeter() << '\n';
			value = -1;
		}
		else if (value == 60) {
			std::cout << obj[4]->area() << '\n';
			value = -1;
		}
		else if (value == 61) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << (obj[4] == obj[num]) << '\n';
			value = -1;
		}
		else if (value == 62) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[4]->isCongruentTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 63) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[4]->isSimilarTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 64) {
			double x, y;
			std::cout << "Enter x, y \n";
			std::cin >> x >> y;
			std::cout << obj[4]->containsPoint(Point(x, y)) << '\n';
		}
	}
	////////////////////////// TRIANGLE //////////////////////

	if (value == 65) {
		double x1, y1, x2, y2, x3, y3;
		std::cout << "Enter Point (left down): \n";
		std::cin >> x1 >> y1;
		std::cout << "Enter Point (up): \n";
		std::cin >> x2 >> y2;
		std::cout << "Enter Point (right): \n";
		std::cin >> x3 >> y3;
		Triangle* shape_triangle = new Triangle(Point(x1, y1), Point(x2, y2), Point(x3, y3)); // как удалить?
		obj[5] = shape_triangle;
		is_create_triangle = true;
		value = -1;

		shape_triangle->Draw();
	}

	else if (is_create_triangle) {

		obj[5]->Draw();

		if (value == 66) {
			double x, y;
			std::cout << "Enter rotate coordinate x, y: \n";
			std::cin >> x >> y;
			double angle;
			std::cout << "Enter Angle: \n";
			std::cin >> angle;
			obj[5]->Rotate(Point(x, y), angle);
			value = -1;
		}
		else if (value == 67) {
			double x, y;
			std::cout << "Enter reflex coordinate x, y: \n";
			std::cin >> x >> y;
			obj[5]->Reflex(Point(x, y));
			value = -1;
		}
		else if (value == 68) {
			double x1, y1, x2, y2;
			std::cout << "Enter Points of Line: x1, y1 and x2, y2: \n";
			std::cin >> x1 >> y1 >> x2 >> y2;
			obj[5]->Reflex(Line(Point(x1, y1), Point(x2, y2)));
			value = -1;
		}
		else if (value == 69) {
			double x, y, k;
			std::cout << "Enter Scale point x, y: \n";
			std::cin >> x >> y;
			std::cout << "Enter coefficient Scale: \n";
			std::cin >> k;
			obj[5]->Scale(Point(x, y), k);
			value = -1;
		}
		else if (value == 70) {
			std::cout << obj[5]->perimeter() << '\n';
			value = -1;
		}
		else if (value == 71) {
			std::cout << obj[5]->area() << '\n';
			value = -1;
		}
		else if (value == 72) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << (obj[5] == obj[num]) << '\n';
			value = -1;
		}
		else if (value == 73) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[5]->isCongruentTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 74) {
			int num;
			std::cout << "0 - Polygone \n 1 - Ellypse \n 2 - Circle \n 3 - Rectangle \n 4 - Square \n 5 - Triangle \n";
			std::cin >> num;
			std::cout << obj[5]->isSimilarTo(*obj[num]) << '\n';
			value = -1;
		}
		else if (value == 75) {
			double x, y;
			std::cout << "Enter x, y \n";
			std::cin >> x >> y;
			std::cout << obj[5]->containsPoint(Point(x, y)) << '\n';
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
	glutAddMenuEntry("Perimeter", 15);
	glutAddMenuEntry("Area", 16);
	glutAddMenuEntry("Is matches", 17);
	glutAddMenuEntry("Is congruent", 18);
	glutAddMenuEntry("Is simular", 19);
	glutAddMenuEntry("Is contains Point", 20);

	submenu_ellipse_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 21);
	glutAddMenuEntry("Rotate", 22);
	glutAddMenuEntry("Reflex point", 23);
	glutAddMenuEntry("Reflex line", 24);
	glutAddMenuEntry("Scale", 25);
	glutAddMenuEntry("Perimeter", 26);
	glutAddMenuEntry("Area", 27);
	glutAddMenuEntry("Is matches", 28);
	glutAddMenuEntry("Is congruent", 29);
	glutAddMenuEntry("Is simular", 30);
	glutAddMenuEntry("Is contains Point", 31);

	submenu_circle_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 32);
	glutAddMenuEntry("Rotate", 33);
	glutAddMenuEntry("Reflex point", 34);
	glutAddMenuEntry("Reflex line", 35);
	glutAddMenuEntry("Scale", 36);
	glutAddMenuEntry("Perimeter", 37);
	glutAddMenuEntry("Area", 38);
	glutAddMenuEntry("Is matches", 39);
	glutAddMenuEntry("Is congruent", 40);
	glutAddMenuEntry("Is simular", 41);
	glutAddMenuEntry("Is contains Point", 42);

	submenu_rectangle_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 43);
	glutAddMenuEntry("Rotate", 44);
	glutAddMenuEntry("Reflex point", 45);
	glutAddMenuEntry("Reflex line", 46);
	glutAddMenuEntry("Scale", 47);
	glutAddMenuEntry("Perimeter", 48);
	glutAddMenuEntry("Area", 49);
	glutAddMenuEntry("Is matches", 50);
	glutAddMenuEntry("Is congruent", 51);
	glutAddMenuEntry("Is simular", 52);
	glutAddMenuEntry("Is contains Point", 53);

	submenu_square_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 54);
	glutAddMenuEntry("Rotate", 55);
	glutAddMenuEntry("Reflex point", 56);
	glutAddMenuEntry("Reflex line", 57);
	glutAddMenuEntry("Scale", 58);
	glutAddMenuEntry("Perimeter", 59);
	glutAddMenuEntry("Area", 60);
	glutAddMenuEntry("Is matches", 61);
	glutAddMenuEntry("Is congruent", 62);
	glutAddMenuEntry("Is simular", 63);
	glutAddMenuEntry("Is contains Point", 64);

	submenu_triangle_id = glutCreateMenu(menu);
	glutAddMenuEntry("Draw", 65);
	glutAddMenuEntry("Rotate", 66);
	glutAddMenuEntry("Reflex point", 67);
	glutAddMenuEntry("Reflex line", 68);
	glutAddMenuEntry("Scale", 69);
	glutAddMenuEntry("Perimeter", 70);
	glutAddMenuEntry("Area", 71);
	glutAddMenuEntry("Is matches", 72);
	glutAddMenuEntry("Is congruent", 73);
	glutAddMenuEntry("Is simular", 74);
	glutAddMenuEntry("Is contains Point", 75);

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