#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTextStream>
#include <QMouseEvent>
#include <QEvent>
#include <QObject>
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QFuture>

#include "ui_FuckMeApplication.h"
#include "polygon.h"

#define string QString

class FuckMeApplication : public QMainWindow
{
	Q_OBJECT

public:
	FuckMeApplication(QWidget *parent = Q_NULLPTR);

private:
	Ui::FuckMeApplicationClass ui;
	Polygon root = Polygon({ QPoint(0, 0), QPoint(256, 256) });
	
	/**
	* Загруженное в QLabel изображение заменяет на то же изображение в ч/б
	*/
	void labelToGrayscale(QLabel*);
	string filenameFromImgDialog();
	void loadImageFromFile(string file, QLabel* label);
	QImage getImgGrayscale(QImage);
	void loadImageFromQImage(QImage, QLabel*);

	/**
	* Рекурсивная отрисовка сетки. Полигон отрисовывается полностью, но только тогда, когда он является листом
	*/
	QImage DrawGrid(QImage, Polygon);

private slots:
	void loadBtnHandler();
	void clearBtnHandler();
	void splitBtnHandler();
	/*void sgBtnHandler();
	void lgBtnHandler();
	void restoreBtnHandler();*/
};
