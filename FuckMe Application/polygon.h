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
*  ласс произвольного полигона, заданного набором точек
*/

class Polygon {
private:
	//массив точек, задающих полигон
	QVector<QPoint> point;

	//массив детей
	QVector<Polygon> children;

	//коэффициенты заливки ax + by + c
	//ƒл€ среднего используетс€ только c
	//ƒл€ линейной ax + c
	//ƒл€ плоскостной ax + by + c
	float fill_factors[3];

	//“ип полигона (вдруг понадобитс€?)
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
	* —оздание сетки в зависимости от порога
	* @param QImage src - исходное изображение
	* @param int threshold - порог разбиени€
	*/
	void CreateGrid(QImage, int);
	/**
	* ќпредел€ет принадлежность точки полигону
	* @param QPoint point
	* @return bool result - true, если принадлежит, иначе false
	*/
	bool ContainsPoint(QPoint);
	/**
	* –азбиение полигона в зависимости от его типа
	*/
	void AddChildren();

	/**
	* ќпредел€ет, €вл€етс€ ли полигон листом
	*/
	bool IsLeaf();

	/**
	* ѕлощадь полигона
	*/
	float Square();

	/**
	* ћинимальна€ координата по оси X, если достроить полигон до пр€моугольника
	*/
	int GetMinX();

	/**
	* ћинимальна€ координата по оси Y, если достроить полигон до пр€моугольника
	*/
	int GetMinY();

	/**
	* ћаксимальна€ координата по оси X, если достроить полигон до пр€моугольника
	*/
	int GetMaxX();

	/**
	* ћаксимальна€ координата по оси Y, если достроить полигон до пр€моугольника
	*/
	int GetMaxY();

	/**
	* «аливка средним
	* @param QImage image - изображение
	* @param QImage src - исходное изображение
	* @return QImage image - восстановленное изображение
	*/
	QImage FillAverageLightness(QImage, QImage);

	/**
	* «аливка с помощью линейной регрессии
	* @param QImage image - изображение
	* @param QImage src - исходное изображение
	* @return QImage image - восстановленное изображение
	*/
	QImage FillLinearRegression(QImage, QImage);

	/**
	* «аливка с помощью плоскостной регрессии
	* @param QImage image - изображение
	* @param QImage src - исходное изображение
	* @return QImage image - восстановленное изображение
	*/
	QImage FillSurfaceRegression(QImage, QImage);
	
};

/**
* “очки в середине отрезка p1,p2
* @param QPoint p1 - начало отрезка
* @param QPoint p2 - конец отрезка
* @return QPoint point - точка с координатами в середине отрезка
*/
QPoint GetMiddlePoint(QPoint, QPoint);
/**
* —ортировка точек дл€ треугольника так, чтобы первой точкой была та, котора€ находитс€ у пр€мого угла
* @param QVector<QPoint> points - вектор точек
* @return QVector<QPoint> points - отсортированный вектор точек
*/
QVector<QPoint> SortPoints(QVector<QPoint>);
/**
* ќпредел€ет, лежит ли точка P на отрезке AB
* @param QPoint p
* @param QPoint A
* @param QPoint B
* @return bool res - true, если лежит, иначе false
*/
bool IsOnLine(QPoint, QPoint, QPoint);