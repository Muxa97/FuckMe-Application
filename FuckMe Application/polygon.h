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
* Класс произвольного полигона, заданного набором точек
*/

class Polygon {
private:
	//массив точек, задающих полигон
	QVector<QPoint> point;

	//массив детей
	QVector<Polygon> children;

	//коэффициенты заливки ax + by + c
	//Для среднего используется только c
	//Для линейной ax + c
	//Для плоскостной ax + by + c
	float fill_factors[3];

	//Тип полигона (вдруг понадобится?)
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
	* Создание сетки в зависимости от порога
	* @param QImage src - исходное изображение
	* @param int threshold - порог разбиения
	*/
	void CreateGrid(QImage, int);
	/**
	* Определяет принадлежность точки полигону
	* @param QPoint point
	* @return bool result - true, если принадлежит, иначе false
	*/
	bool ContainsPoint(QPoint);
	/**
	* Разбиение полигона в зависимости от его типа
	*/
	void AddChildren();

	/**
	* Определяет, является ли полигон листом
	*/
	bool IsLeaf();

	/**
	* Площадь полигона
	*/
	float Square();

	/**
	* Минимальная координата по оси X, если достроить полигон до прямоугольника
	*/
	int GetMinX();

	/**
	* Минимальная координата по оси Y, если достроить полигон до прямоугольника
	*/
	int GetMinY();

	/**
	* Максимальная координата по оси X, если достроить полигон до прямоугольника
	*/
	int GetMaxX();

	/**
	* Максимальная координата по оси Y, если достроить полигон до прямоугольника
	*/
	int GetMaxY();

	/**
	* Заливка средним
	* @param QImage image - изображение
	* @param QImage src - исходное изображение
	* @return QImage image - восстановленное изображение
	*/
	QImage FillAverageLightness(QImage, QImage);

	/**
	* Заливка с помощью линейной регрессии
	* @param QImage image - изображение
	* @param QImage src - исходное изображение
	* @return QImage image - восстановленное изображение
	*/
	QImage FillLinearRegression(QImage, QImage, int);

	/**
	* Заливка с помощью плоскостной регрессии
	* @param QImage image - изображение
	* @param QImage src - исходное изображение
	* @return QImage image - восстановленное изображение
	*/
	QImage FillSurfaceRegression(QImage, QImage);
	
	/**
	* Возвращает размер дерева полигонов
	*/
	float getTreeSize();

	/**
	* Запись дерева в файл
	*/
	void WriteTreeToFile(QTextStream&, int);

	/**
	* Считывание дерева из файла
	*/
	void ReadTreeFromFile(QTextStream&, PolyType);

	/**
	* Поиск полигона-листа, содержащего данную точку
	*/
	Polygon* GetPolygonByPoint(QPoint);

	/**
	* Поиск соседей полигона
	*/
	QVector<Polygon> GetNeighbours(Polygon, QVector<Polygon>);

	/**
	* Определяет, пересекаются ли полигоны
	*/
	bool Intersect(Polygon);
};

/**
* Точки в середине отрезка p1,p2
* @param QPoint p1 - начало отрезка
* @param QPoint p2 - конец отрезка
* @return QPoint point - точка с координатами в середине отрезка
*/
QPoint GetMiddlePoint(QPoint, QPoint);
/**
* Сортировка точек для треугольника так, чтобы первой точкой была та, которая находится у прямого угла
* @param QVector<QPoint> points - вектор точек
* @return QVector<QPoint> points - отсортированный вектор точек
*/
QVector<QPoint> SortPoints(QVector<QPoint>);
/**
* Определяет, лежит ли точка P на отрезке AB
* @param QPoint p
* @param QPoint A
* @param QPoint B
* @return bool res - true, если лежит, иначе false
*/
bool IsOnLine(QPoint, QPoint, QPoint);