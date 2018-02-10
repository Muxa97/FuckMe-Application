#include "Polygon.h"

Polygon::Polygon()
{
}

Polygon::Polygon(QVector<QPoint> points)
{
	//Если треугольник, то сразу сортируем точки
	if (points.size() == 3) points = SortPoints(points);

	for (QPoint& p : points) {
		this->point.push_back(p);
	}
}

Polygon::~Polygon()
{
}

void Polygon::SetChildren(QVector<Polygon> children)
{
	this->children.clear();
	for (Polygon& child : children) {
		this->children.push_back(child);
	}
}

void Polygon::SetFillFactors(float a, float b, float c)
{
	this->fill_factors[0] = a;
	this->fill_factors[1] = b;
	this->fill_factors[2] = c;
}

void Polygon::SetType(PolyType t)
{
	this->type = t;
}

QVector<QPoint> Polygon::GetPoints()
{
	return this->point;
}

QVector<Polygon> Polygon::GetChildren()
{
	return this->children;
}

QVector<float> Polygon::GetFillFactors()
{
	return { this->fill_factors[0], this->fill_factors[1], this->fill_factors[2] };
}

PolyType Polygon::GetType()
{
	return this->type;
}

void Polygon::CreateGrid(QImage src, int threshold)
{
	//Если площадь полигона меньше 4, то не разбиваем его (условие выхода из рекурсии)
	if (this->Square() <= 4) return;

	int min = 255, max = 0;

	//Проходим по прямоугольнику, достроенному из исходного полигона (в основном, относится к треугольникам)
	for (int i = this->GetMinY(); i < this->GetMaxY(); i++) {
		for (int j = this->GetMinX(); j < this->GetMaxX(); j++) {
			if (this->ContainsPoint(QPoint(j, i))) {
				QColor clr = src.pixelColor(j, i);
				if (clr.lightness() > max) max = clr.lightness();
				if (clr.lightness() < min) min = clr.lightness();

				if ((max - min) >= threshold) {
					//Разбиваем полигон на части
					this->AddChildren();
					//Каждую из них проверяем
					for (Polygon& child : this->children) {
						child.CreateGrid(src, threshold);
					}
					//Если полигон разбит, его не нужно дальше обрабатывать
					return;
				}
			}
		}
	}
}

//Спизжено из Инета, основана на луче, пущеном влево из точки. Четное количество пересечений со сторонами полигона -- не принадлежит, нечетное -- принадлежит
//Вроде имеет какие-то неточности, будем надеяться, несущественные
bool Polygon::ContainsPoint(QPoint point) {
	bool res = false;
	QPolygon poly;
	if (this->point.size() == 2)
		poly = QPolygon({ this->point[0], QPoint(this->point[1].x(), this->point[0].y()), this->point[1], QPoint(this->point[0].x(), this->point[1].y()) });
	else 
		poly = QPolygon(this->point);

	res = poly.containsPoint(point, Qt::WindingFill);

	if (!res) {
		for (int i = 0; i < poly.size() && !res; i++) {
			for (int j = i + 1; j < poly.size() && !res; j++) {
				if (IsOnLine(point, poly[i], poly[j]))
					res = true;
			}
		}
	}
	
	return res;
}

//Добавляет полигону детей в зависимости от типа
void Polygon::AddChildren() {
	QVector<QPoint> parent_points = this->GetPoints();
	QVector<Polygon> children;

	if (this->type == SQUARE) {
		//Левый верхний квадрат
		Polygon tl_child({ parent_points[0], GetMiddlePoint(parent_points[0], parent_points[1]) });

		//Правый верхний
		Polygon tr_child({ QPoint(parent_points[0].x() + (parent_points[1].x() - parent_points[0].x()) / 2, parent_points[0].y()),
			QPoint(parent_points[1].x(), parent_points[1].y() + (parent_points[0].y() - parent_points[1].y()) / 2) });

		//Левый нижний
		Polygon bl_child({ QPoint(parent_points[0].x(), parent_points[1].y() + (parent_points[0].y() - parent_points[1].y()) / 2),
			QPoint(parent_points[0].x() + (parent_points[1].x() - parent_points[0].x()) / 2, parent_points[1].y())});

		//Правый нижний
		Polygon br_child({ GetMiddlePoint(parent_points[0], parent_points[1]), parent_points[1] });

		children = { tl_child, tr_child, bl_child, br_child };
	}
	else if (this->type == TRIANGLE) {
		Polygon first_child;
		Polygon second_child;
		//Если это корень (всегда квадрат, имеет две точки), то делим его на два треугольника
		if (this->point.size() == 2) {
			first_child = Polygon({ parent_points[0], QPoint(parent_points[1].x(), parent_points[0].y()), QPoint(parent_points[0].x(), parent_points[1].y()) });
			second_child = Polygon({ parent_points[1], QPoint(parent_points[1].x(), parent_points[0].y()), QPoint(parent_points[0].x(), parent_points[1].y()) });
		}
		//Иначе делим треугольник на два других треугольника
		else {
			first_child = Polygon({ parent_points[1], parent_points[0], GetMiddlePoint(parent_points[1], parent_points[2]) });
			second_child = Polygon({ parent_points[2], parent_points[0], GetMiddlePoint(parent_points[1], parent_points[2]) });
		}

		children = { first_child, second_child };
	}
	else if (this->type == RECTANGLE) {
		Polygon first_child;
		Polygon second_child;
		//Если квадрат или dx больше, чем dy, то делим прямоугольник на два вертикально
		if (parent_points[1].x() - parent_points[0].x() >= parent_points[1].y() - parent_points[0].y()) {
			first_child = Polygon({ parent_points[0], QPoint((parent_points[1].x() + parent_points[0].x()) / 2, parent_points[1].y()) });
			second_child = Polygon({ QPoint((parent_points[1].x() + parent_points[0].x()) / 2, parent_points[0].y()), parent_points[1] });
		}
		//Иначе делим горизонтально
		else {
			first_child = Polygon({ parent_points[0], QPoint(parent_points[1].x(), (parent_points[1].y() + parent_points[0].y()) / 2) });
			second_child = Polygon({ QPoint(parent_points[0].x(), (parent_points[1].y() + parent_points[0].y()) / 2), parent_points[1] });
		}
		children = { first_child, second_child };
	}

	for (Polygon& child : children) {
		child.SetType(this->type);
	}
	this->SetChildren(children);
}

//Средняя точка -- изи
QPoint GetMiddlePoint(QPoint p1, QPoint p2) {
	int x = (p1.x() + p2.x()) / 2;
	int y = (p1.y() + p2.y()) / 2;

	return QPoint(x, y);
}

//Проверяем точки с помощью скалярного произведения
QVector<QPoint> SortPoints(QVector<QPoint> points) {
	for (int i = 0; i < 3; i++) {
		if ((points[(i + 1) % 3].x() - points[i].x()) * (points[(i + 2) % 3].x() - points[i].x()) +
			(points[(i + 1) % 3].y() - points[i].y()) * (points[(i + 2) % 3].y() - points[i].y()) == 0) {
			return { points[i], points[(i + 1) % 3], points[(i + 2) % 3] };
		}
	}
}

bool Polygon::IsLeaf() {
	if (this->children.size() == 0) return true;

	return false;
}

//Тоже спизжено из Интернета, даже не разбирался, как работает, но вроде считает верно. Что-то там связано с площадями трапеций, сложно
float Polygon::Square() {
	float res = 0;
	QVector<QPoint> points;
	if (this->point.size() == 2)
		points = { this->point[0], QPoint(this->point[1].x(), this->point[0].y()), this->point[1], QPoint(this->point[0].x(), this->point[1].y()) };
	else points = this->point;
	for (unsigned i = 0; i < points.size(); i++)
	{
		QPoint
			p1 = i ? points[i - 1] : points.back(),
			p2 = points[i];
		res += (p1.x() - p2.x()) * (p1.y() + p2.y());
	}
	return fabs(res) / 2;
}

int Polygon::GetMinX()
{
	int res = 256;
	for (QPoint p : this->point) {
		if (p.x() < res) res = p.x();
	}

	return res;
}

int Polygon::GetMinY()
{
	int res = 256;
	for (QPoint p : this->point) {
		if (p.y() < res) res = p.y();
	}

	return res;
}

int Polygon::GetMaxX()
{
	int res = 0;
	for (QPoint p : this->point) {
		if (p.x() > res) res = p.x();
	}

	return res;
}

int Polygon::GetMaxY()
{
	int res = 0;
	for (QPoint p : this->point) {
		if (p.y() > res) res = p.y();
	}

	return res;
}

//
//Заливка средним
//
QImage Polygon::FillAverageLightness(QImage image, QImage src) {
	if (this->IsLeaf()) {
		QPixmap px = QPixmap::fromImage(image);
		QPainter p(&px);

		int pixels_in_polygon = 0;
		int lightness = 0;

		//Суммируем яркости всех пикселей полигона
		for (int y = this->GetMinY(); y < this->GetMaxY(); y++) {
			for (int x = this->GetMinX(); x < this->GetMaxX(); x++) {
				if (this->ContainsPoint(QPoint(x, y))) {
					pixels_in_polygon++;
					lightness += src.pixelColor(x, y).lightness();
				}
			}
		}
		//находим среднюю яркость и заливаем полигон
		lightness = lightness / pixels_in_polygon;
		this->SetFillFactors(0, 0, lightness);
		QColor clr(lightness, lightness, lightness);
		QPolygon poly;
		if (this->point.size() == 2)
			poly = QPolygon({ this->point[0], QPoint(this->point[1].x(), this->point[0].y()), this->point[1], QPoint(this->point[0].x(), this->point[1].y()) });
		else
			poly = QPolygon(this->point);
		p.setBrush(clr);
		p.setPen(Qt::PenStyle::NoPen);
		p.drawPolygon(poly, Qt::WindingFill);

		image = px.toImage();
	}
	else {
		for (Polygon& child : this->children)
			image = child.FillAverageLightness(image, src);
	}

	return image;
}

QImage Polygon::FillLinearRegression(QImage image, QImage src)
{
	if (this->IsLeaf()) {
		//Цикл по строкам
		for (int y = this->GetMinY(); y <= this->GetMaxY(); y++) {
			QVector<QVector<long double>> matrix(3, { 0, 0, 0, 0 });
			/*
			//Параболическая заливка
			for (int x = this->GetMinX(); x <= this->GetMaxX(); x++) {
				if (this->ContainsPoint(QPoint(x, y))) {
					x++;
					matrix[0][0] += x*x*x*x; matrix[0][1] += x*x*x; matrix[0][2] += x*x; matrix[0][3] += src.pixelColor(x, y).lightness() * x*x;
					matrix[1][0] += x*x*x; matrix[1][1] += x*x; matrix[1][2] += x; matrix[1][3] += src.pixelColor(x, y).lightness() * x;
					matrix[2][0] += x*x; matrix[2][1] += x; matrix[2][2]++; matrix[2][3] += src.pixelColor(x, y).lightness();
					x--;
				}
			}

			if (matrix[0][0] == 0) {
				int l = 0;
			}
			long double k12 = -matrix[1][0] / matrix[0][0];
			long double k13 = -matrix[2][0] / matrix[0][0];
			for (int i = 0; i <= 3; i++) {
				matrix[1][i] += k12 * matrix[0][i];
			}
			for (int i = 0; i <= 3; i++) {
				matrix[2][i] += k13 * matrix[0][i];
			}
			long double k23 = -matrix[2][1] / matrix[1][1];

			for (int i = 0; i <= 3; i++) {
				matrix[2][i] += k23 * matrix[1][i];
			}

			long double k = matrix[2][2];
			for (int i = 0; i <= 3; i++) {
				matrix[2][i] /= k;
			}
			long double c = matrix[2][3];
			k = matrix[1][2];
			for (int i = 0; i <= 3; i++) {
				matrix[1][i] -= matrix[2][i] * k;
			}
			k = matrix[0][2];
			for (int i = 0; i <= 3; i++) {
				matrix[0][i] -= matrix[2][i] * k;
			}

			k = matrix[1][1];
			for (int i = 0; i <= 3; i++) {
				matrix[1][i] /= k;
			}
			long double b = matrix[1][3];
			k = matrix[0][1];
			for (int i = 0; i <= 3; i++) {
				matrix[0][i] -= matrix[1][i] * k;
			}
			k = matrix[0][0];
			for (int i = 0; i <= 3; i++) {
				matrix[0][i] /= k;
			}
			long double a = matrix[0][3];

			for (int x = this->GetMinX(); x <= this->GetMaxX() + 1; x++) {
				if (this->ContainsPoint(QPoint(x, y))) {
					int lightness = fabs(a*x*x + b*x + c);
					if (lightness < 0 || lightness >= 256) {
						int l = 0;
					}
					QColor clr(lightness, lightness, lightness);
					image.setPixelColor(QPoint(x, y), clr);
				}
			}
			*/
			//Линейное говно
			//Вычисление коэффициентов для апроксиммирующей прямой
			for (int x = this->GetMinX(); x <= this->GetMaxX(); x++) {
				if (this->ContainsPoint(QPoint(x, y))) {
					matrix[0][0] += x * x; matrix[0][1] += x; matrix[0][2] += x * src.pixelColor(x, y).lightness();
					matrix[1][0] += x; matrix[1][1]++; matrix[1][2] += src.pixelColor(x, y).lightness();
				}
			}

			//Вычисление коэффициентов
			float k12 = -(float)matrix[1][0] / matrix[0][0];
			matrix[1][0] += k12 * matrix[0][0];
			matrix[1][1] += k12 * matrix[0][1];
			matrix[1][2] += k12 * matrix[0][2];
			float b = (float)matrix[1][2] / matrix[1][1];
			float a = (matrix[0][2] - b * matrix[0][1]) / matrix[0][0];
			//Заливка полигона
			for (int x = this->GetMinX(); x <= this->GetMaxX() + 1; x++) {
				if (this->ContainsPoint(QPoint(x, y))) {
					int lightness = (a * x + b);
					QColor clr(lightness, lightness, lightness);
					image.setPixelColor(QPoint(x, y), clr);
				}
			}
		}
	}
	else {
		for (Polygon& child : this->children)
			image = child.FillLinearRegression(image, src);
	}

	return image;
}

QImage Polygon::FillSurfaceRegression(QImage image, QImage src)
{
	if (this->IsLeaf()) {
		
	}
	else {
		for (Polygon& child : this->children)
			image = child.FillSurfaceRegression(image, src);
	}

	return image;
}

bool IsOnLine(QPoint P, QPoint A, QPoint B) {
	bool res = false;
	if ((((P.x()) >= A.x() && P.x() <= B.x()) || (P.x() >= B.x() && P.x() <= A.x())) &&
		((P.y() >= A.y() && P.y() <= B.y()) || (P.y() >= B.y() && P.y() <= A.y()))) {
		float k = (float)(A.y() - B.y()) / (A.x() - B.x());
		float b = A.y() - A.x() * k;

		if (P.y() - P.x() * k - b == 0) res = true;
	}

	return res;
}