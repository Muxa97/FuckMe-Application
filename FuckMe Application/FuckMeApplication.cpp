#include "FuckMeApplication.h"

FuckMeApplication::FuckMeApplication(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//Обработчики кнопок
	connect(ui.loadBtn, SIGNAL(released()), this, SLOT(loadBtnHandler()));
	connect(ui.clearBtn, SIGNAL(released()), this, SLOT(clearBtnHandler()));
	connect(ui.splitBtn, SIGNAL(released()), this, SLOT(splitBtnHandler()));
	connect(ui.SaveGridBtn, SIGNAL(released()), this, SLOT(sgBtnButtonHandler()));
	connect(ui.LoadGridBtn, SIGNAL(released()), this, SLOT(lgButtonHandler()));
	connect(ui.restoreBtn, SIGNAL(released()), this, SLOT(restoreBtnHandler()));

}

/*------------------------------------------------------------ЗАГРУЗКА ИЗОБРАЖЕНИЯ-----------------------------------------------------*/
	//
	//Обработчик кнопки загрузки изображения
	//
	void FuckMeApplication::loadBtnHandler() {
		string fileName = this->filenameFromImgDialog();

		this->loadImageFromFile(fileName, ui.SrcImage);

		this->labelToGrayscale(ui.SrcImage);

		if (ui.SrcImage->pixmap())
		{
			ui.splitBtn->setEnabled(true);
		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText("Error reading image!");
			msgBox.exec();
		}
	}

	string FuckMeApplication::filenameFromImgDialog()
	{
		string filename = QFileDialog::getOpenFileName(this,
			tr("Open Image"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), tr("Изображения (*.png *.jpg *.bmp)"));

		return filename;
	}

	void FuckMeApplication::loadImageFromFile(string file, QLabel* label)
	{
		QFile f(file);

		label->setPixmap(QPixmap(file));
	}

	//
	// Перевод изображения в оттенки серого
	//
	void FuckMeApplication::labelToGrayscale(QLabel* labelImg)
	{
		QImage image;

		const QPixmap* pixmap = labelImg->pixmap();
		if (pixmap)
		{
			image = (pixmap->toImage());
		}

		labelImg->setPixmap(QPixmap::fromImage(this->getImgGrayscale(image)));
	}

	//
	//Получение ч/б изображения из цветного
	//
	QImage FuckMeApplication::getImgGrayscale(QImage image)
	{
		for (int ii = 0; ii < image.height(); ii++) {
			uchar* scan = image.scanLine(ii);
			int depth = 4;
			for (int jj = 0; jj < image.width(); jj++) {
				QRgb* rgbpixel = reinterpret_cast<QRgb*>(scan + jj*depth);
				int gray = qGray(*rgbpixel);
				*rgbpixel = QColor(gray, gray, gray).rgba();
			}
		}

		return image;
	}

/*------------------------------------------------------------ЗАГРУЗКА ИЗОБРАЖЕНИЯ-----------------------------------------------------*/

	//
	//Очистка всех окон
	//
	void FuckMeApplication::clearBtnHandler() {
		ui.SrcImage->clear();
		ui.GridImage->clear();
		ui.ResImage->clear();
		ui.ObjImage->clear();
	}

/*----------------------------------------------------------СОЗДАНИЕ И ОТРИСОВКА СЕТКИ--------------------------------------------------*/
	//
	//Обработчик кнопки
	//
	void FuckMeApplication::splitBtnHandler() {
		//Очищаем изображение
		ui.GridImage->clear();
		this->root.SetChildren({});
		QImage grid(256, 256, QImage::Format_Grayscale8);
		grid.fill(Qt::white);

		//Определяем тип полигонов
		if (ui.polyType->currentIndex() == 0) this->root.SetType(TRIANGLE);
		else if (ui.polyType->currentIndex() == 1) this->root.SetType(RECTANGLE);
		else this->root.SetType(SQUARE);

		//Создаем дерево полигонов и отрисовываем на его основе сетку
		this->root.CreateGrid(ui.SrcImage->pixmap()->toImage(), ui.threshold->value());
		grid = this->DrawGrid(grid, this->root);

		//Подрубаем кнопки
		ui.clearBtn->setEnabled(true);
		ui.SaveGridBtn->setEnabled(true);
		ui.restoreBtn->setEnabled(true);

		this->loadImageFromQImage(grid, ui.GridImage);
	}

	//
	//Загрузка изображения в QLabel из QImage
	//
	void FuckMeApplication::loadImageFromQImage(QImage image, QLabel* labelImg)
	{
		labelImg->setPixmap(QPixmap::fromImage(image));
	}

	//
	//Отрисовка сетки
	//
	QImage FuckMeApplication::DrawGrid(QImage grid, Polygon poly) {
		//Если полигон -- лист, то мы его рисуем
		if (poly.IsLeaf()) {
			QPixmap px = QPixmap::fromImage(grid);
			QPainter p(&px);

			//Отрисовка треугольника
			if (poly.GetPoints().size() == 3) {
				for (int i = 0; i < 3; i++) {
					p.drawLine(poly.GetPoints()[i], poly.GetPoints()[(i + 1) % 3]);
				}
			}
			//Отрисовка прямоугольников и квадратов. Разделено, т.к. в треугольниках не нужно ничего просчитывать лишний раз
			else {
				p.drawLine(QPoint(poly.GetPoints()[0].x(), poly.GetPoints()[0].y()), QPoint(poly.GetPoints()[1].x(), poly.GetPoints()[0].y()));
				p.drawLine(QPoint(poly.GetPoints()[1].x(), poly.GetPoints()[0].y()), QPoint(poly.GetPoints()[1].x(), poly.GetPoints()[1].y()));
				p.drawLine(QPoint(poly.GetPoints()[0].x(), poly.GetPoints()[1].y()), QPoint(poly.GetPoints()[1].x(), poly.GetPoints()[1].y()));
				p.drawLine(QPoint(poly.GetPoints()[0].x(), poly.GetPoints()[0].y()), QPoint(poly.GetPoints()[0].x(), poly.GetPoints()[1].y()));
			}

			grid = px.toImage();
		}
		//Иначе рекурсивно запускаем для каждого ребенка
		else {
			for (Polygon child : poly.GetChildren()) {
				grid = this->DrawGrid(grid, child);
			}
		}
		return grid;
	}
/*----------------------------------------------------------СОЗДАНИЕ И ОТРИСОВКА СЕТКИ--------------------------------------------------*/

/*----------------------------------------------------------ВОССТАНОВЛЕНИЕ ИЗОБРАЖЕНИЯ--------------------------------------------------*/
	void FuckMeApplication::restoreBtnHandler() {
		QImage image(256, 256, QImage::Format_Grayscale8);
		image.fill(Qt::white);
		QImage src = this->ui.SrcImage->pixmap()->toImage();
		//Заливка средним
		if (this->ui.AverageLightness->isChecked()) {
			image = this->root.FillAverageLightness(image, src);
		}
		//Линейная регрессия
		else if (this->ui.LinearRegression->isChecked()) {
			image = this->root.FillLinearRegression(image, src);
		}
		//Плоскостная регрессия
		else {
			image = this->root.FillSurfaceRegression(image, src);
		}

		this->loadImageFromQImage(image, ui.ResImage);
	}
/*----------------------------------------------------------ВОССТАНОВЛЕНИЕ ИЗОБРАЖЕНИЯ--------------------------------------------------*/