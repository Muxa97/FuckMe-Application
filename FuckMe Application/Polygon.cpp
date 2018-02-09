#include "Polygon.h"

Polygon::Polygon()
{
}

Polygon::Polygon(QVector<QPoint> points)
{
	//���� �����������, �� ����� ��������� �����
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
	//���� ������� �������� ������ 4, �� �� ��������� ��� (������� ������ �� ��������)
	if (this->Square() <= 4) return;

	int min = 255, max = 0;

	//�������� �� ��������������, ������������ �� ��������� �������� (� ��������, ��������� � �������������)
	for (int i = this->GetMinY(); i < this->GetMaxY(); i++) {
		for (int j = this->GetMinX(); j < this->GetMaxX(); j++) {
			if (this->ContainsPoint(QPoint(j, i))) {
				QColor clr = src.pixelColor(j, i);
				if (clr.lightness() > max) max = clr.lightness();
				if (clr.lightness() < min) min = clr.lightness();

				if ((max - min) > threshold) {
					//��������� ������� �� �����
					this->AddChildren();
					//������ �� ��� ���������
					for (Polygon& child : this->children) {
						child.CreateGrid(src, threshold);
					}
					//���� ������� ������, ��� �� ����� ������ ������������
					return;
				}
			}
		}
	}
}

//�������� �� �����, �������� �� ����, ������� ����� �� �����. ������ ���������� ����������� �� ��������� �������� -- �� �����������, �������� -- �����������
//����� ����� �����-�� ����������, ����� ���������, ��������������
bool Polygon::ContainsPoint(QPoint point) {
	bool res = false;

	for (int i = 1; i <= this->point.size(); i++) {
		if ((((this->point[i % this->point.size()].y() <= point.y()) && (point.y() < this->point[i - 1].y())) ||
			((this->point[i - 1].y() <= point.y()) && (point.y() < this->point[i % this->point.size()].y()))) &&
			(point.x() > (this->point[i - 1].x() - this->point[i % this->point.size()].x()) * (point.y() - this->point[i % this->point.size()].y()) /
			(this->point[i - 1].y() - this->point[i % this->point.size()].y()) + this->point[i % this->point.size()].x()))
			res = !res;
	}

	if ((((this->point[0].y() <= point.y()) && (point.y() < this->point[1].y())) ||
		((this->point[1].y() <= point.y()) && (point.y() < this->point[0].y()))) &&
		(point.x() > (this->point[1].x() - this->point[0].x()) * 
		(point.y() - this->point[0].y()) / (this->point[1].y() - this->point[0].y()) + this->point[0].x()))
		res = !res;
	
	return res;
}

//��������� �������� ����� � ����������� �� ����
void Polygon::AddChildren() {
	QVector<QPoint> parent_points = this->GetPoints();
	QVector<Polygon> children;

	if (this->type == SQUARE) {
		//����� ������� �������
		Polygon tl_child({ parent_points[0], GetMiddlePoint(parent_points[0], parent_points[1]) });

		//������ �������
		Polygon tr_child({ QPoint(parent_points[0].x() + (parent_points[1].x() - parent_points[0].x()) / 2, parent_points[0].y()),
			QPoint(parent_points[1].x(), parent_points[1].y() + (parent_points[0].y() - parent_points[1].y()) / 2) });

		//����� ������
		Polygon bl_child({ QPoint(parent_points[0].x(), parent_points[1].y() + (parent_points[0].y() - parent_points[1].y()) / 2),
			QPoint(parent_points[0].x() + (parent_points[1].x() - parent_points[0].x()) / 2, parent_points[1].y())});

		//������ ������
		Polygon br_child({ GetMiddlePoint(parent_points[0], parent_points[1]), parent_points[1] });

		children = { tl_child, tr_child, bl_child, br_child };
	}
	else if (this->type == TRIANGLE) {
		Polygon first_child;
		Polygon second_child;
		//���� ��� ������ (������ �������, ����� ��� �����), �� ����� ��� �� ��� ������������
		if (this->point.size() == 2) {
			first_child = Polygon({ parent_points[0], QPoint(parent_points[1].x(), parent_points[0].y()), QPoint(parent_points[0].x(), parent_points[1].y()) });
			second_child = Polygon({ parent_points[1], QPoint(parent_points[1].x(), parent_points[0].y()), QPoint(parent_points[0].x(), parent_points[1].y()) });
		}
		//����� ����� ����������� �� ��� ������ ������������
		else {
			first_child = Polygon({ parent_points[1], parent_points[0], GetMiddlePoint(parent_points[1], parent_points[2]) });
			second_child = Polygon({ parent_points[2], parent_points[0], GetMiddlePoint(parent_points[1], parent_points[2]) });
		}

		children = { first_child, second_child };
	}
	else if (this->type == RECTANGLE) {
		Polygon first_child;
		Polygon second_child;
		//���� ������� ��� dx ������, ��� dy, �� ����� ������������� �� ��� �����������
		if (parent_points[1].x() - parent_points[0].x() >= parent_points[1].y() - parent_points[0].y()) {
			first_child = Polygon({ parent_points[0], QPoint((parent_points[1].x() + parent_points[0].x()) / 2, parent_points[1].y()) });
			second_child = Polygon({ QPoint((parent_points[1].x() + parent_points[0].x()) / 2, parent_points[0].y()), parent_points[1] });
		}
		//����� ����� �������������
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

//������� ����� -- ���
QPoint GetMiddlePoint(QPoint p1, QPoint p2) {
	int x = (p1.x() + p2.x()) / 2;
	int y = (p1.y() + p2.y()) / 2;

	return QPoint(x, y);
}

//��������� ����� � ������� ���������� ������������
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

//���� �������� �� ���������, ���� �� ����������, ��� ��������, �� ����� ������� �����. ���-�� ��� ������� � ��������� ��������, ������
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
