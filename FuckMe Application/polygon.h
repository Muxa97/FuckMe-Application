#pragma once

#include <QVector>
#include <QPoint>
#include <QPainter>

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