#pragma once

#include <QVector>
#include <QPoint>
#include <QPainter>
#include <QTextStream>

enum PolyType {
	TRIANGLE,
	RECTANGLE,
	SQUARE
};

/**
* ����� ������������� ��������, ��������� ������� �����
*/

class Polygon {
private:
	//������ �����, �������� �������
	QVector<QPoint> point;

	//������ �����
	QVector<Polygon> children;

	//������������ ������� ax + by + c
	//��� �������� ������������ ������ c
	//��� �������� ax + c
	//��� ����������� ax + by + c
	float fill_factors[3];

	//��� �������� (����� �����������?)
	PolyType type;
public:
	Polygon();
	Polygon(QVector<QPoint>);
	~Polygon();

	void SetChildren(QVector<Polygon>);
	void SetFillFactors(float, float, float);
	void SetType(PolyType);

	QVector<QPoint> GetPoints();
	QVector<Polygon> GetChildren();
	QVector<float> GetFillFactors();
	PolyType GetType();
	QPolygon toQPolygon();
	/**
	* �������� ����� � ����������� �� ������
	* @param QImage src - �������� �����������
	* @param int threshold - ����� ���������
	*/
	void CreateGrid(QImage, int);
	/**
	* ���������� �������������� ����� ��������
	* @param QPoint point
	* @return bool result - true, ���� �����������, ����� false
	*/
	bool ContainsPoint(QPoint);
	/**
	* ��������� �������� � ����������� �� ��� ����
	*/
	void AddChildren();

	/**
	* ����������, �������� �� ������� ������
	*/
	bool IsLeaf();

	/**
	* ������� ��������
	*/
	float Square();

	/**
	* ����������� ���������� �� ��� X, ���� ��������� ������� �� ��������������
	*/
	int GetMinX();

	/**
	* ����������� ���������� �� ��� Y, ���� ��������� ������� �� ��������������
	*/
	int GetMinY();

	/**
	* ������������ ���������� �� ��� X, ���� ��������� ������� �� ��������������
	*/
	int GetMaxX();

	/**
	* ������������ ���������� �� ��� Y, ���� ��������� ������� �� ��������������
	*/
	int GetMaxY();

	/**
	* ������� �������
	* @param QImage image - �����������
	* @param QImage src - �������� �����������
	* @return QImage image - ��������������� �����������
	*/
	QImage FillAverageLightness(QImage, QImage);

	/**
	* ������� � ������� �������� ���������
	* @param QImage image - �����������
	* @param QImage src - �������� �����������
	* @return QImage image - ��������������� �����������
	*/
	QImage FillLinearRegression(QImage, QImage, int);

	/**
	* ������� � ������� ����������� ���������
	* @param QImage image - �����������
	* @param QImage src - �������� �����������
	* @return QImage image - ��������������� �����������
	*/
	QImage FillSurfaceRegression(QImage, QImage);
	
	/**
	* ���������� ������ ������ ���������
	*/
	float getTreeSize();

	/**
	* ������ ������ � ����
	*/
	void WriteTreeToFile(QTextStream&, int);

	/**
	* ���������� ������ �� �����
	*/
	void ReadTreeFromFile(QTextStream&, PolyType);

	/**
	* ����� ��������-�����, ����������� ������ �����
	*/
	Polygon* GetPolygonByPoint(QPoint);

	/**
	* ����� ������� ��������
	*/
	QVector<Polygon> GetNeighbours(Polygon, QVector<Polygon>);

	/**
	* ����������, ������������ �� ��������
	*/
	bool Intersect(Polygon);
};

/**
* ����� � �������� ������� p1,p2
* @param QPoint p1 - ������ �������
* @param QPoint p2 - ����� �������
* @return QPoint point - ����� � ������������ � �������� �������
*/
QPoint GetMiddlePoint(QPoint, QPoint);
/**
* ���������� ����� ��� ������������ ���, ����� ������ ������ ���� ��, ������� ��������� � ������� ����
* @param QVector<QPoint> points - ������ �����
* @return QVector<QPoint> points - ��������������� ������ �����
*/
QVector<QPoint> SortPoints(QVector<QPoint>);
/**
* ����������, ����� �� ����� P �� ������� AB
* @param QPoint p
* @param QPoint A
* @param QPoint B
* @return bool res - true, ���� �����, ����� false
*/
bool IsOnLine(QPoint, QPoint, QPoint);