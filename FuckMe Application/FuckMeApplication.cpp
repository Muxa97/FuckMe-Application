#include "FuckMeApplication.h"

FuckMeApplication::FuckMeApplication(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//Обработчики кнопок
	connect(ui.loadBtn, SIGNAL(released()), this, SLOT(loadBtnHandler()));
	connect(ui.clearBtn, SIGNAL(released()), this, SLOT(clearBtnHandler()));
	connect(ui.splitBtn, SIGNAL(released()), this, SLOT(splitBtnHandler()));
	connect(ui.SaveGridBtn, SIGNAL(released()), this, SLOT(sgBtnHandler()));
	connect(ui.LoadGridBtn, SIGNAL(released()), this, SLOT(lgBtnHandler()));
	connect(ui.restoreBtn, SIGNAL(released()), this, SLOT(restoreBtnHandler()));

	connect(ui.SrcImage, SIGNAL(clicked(QMouseEvent*)), this, SLOT(ImageClickHandler(QMouseEvent*)));
	connect(ui.GridImage, SIGNAL(clicked(QMouseEvent*)), this, SLOT(ImageClickHandler(QMouseEvent*)));
	connect(ui.ResImage, SIGNAL(clicked(QMouseEvent*)), this, SLOT(ImageClickHandler(QMouseEvent*)));
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
		ui.restoreBtn->setEnabled(true);

		this->loadImageFromQImage(grid, ui.GridImage);

		float k = 0, r = 0;
		float size = this->root.getTreeSize();
		k = 3 * 256 * 256 / size;
		r = size / 256 / 256;
		ui.label_4->setText(QString::number(k));
		ui.label_7->setText(QString::number(r));
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
		else if (this->ui.LinearRegression->isChecked() || this->ui.LinearRegression_2->isChecked()) {
			image = this->root.FillLinearRegression(image, src, this->ui.LinearRegression->isChecked() ? 0 : 1);
		}
		//Плоскостная регрессия
		else {
			image = this->root.FillSurfaceRegression(image, src);
		}

		this->loadImageFromQImage(image, ui.ResImage);

		ui.label_5->setText(this->getSD(image, ui.SrcImage->pixmap()->toImage()));
		ui.SaveGridBtn->setEnabled(true);
	}

	QString FuckMeApplication::getSD(QImage resImage, QImage src) {
		float SKO = 0;

		int sqSum = 0, n = 0;

		for (int i = 0; i < 256; i++)
		{
			for (int j = 0; j < 256; j++)
			{
				int sub = src.pixelColor(j, i).lightness() - resImage.pixelColor(j, i).lightness();
				sqSum += sub * sub;
				n++;
			}
		}

		SKO = sqrt(sqSum / n);

		return QString::number(SKO);
	}
/*----------------------------------------------------------ВОССТАНОВЛЕНИЕ ИЗОБРАЖЕНИЯ--------------------------------------------------*/
/*----------------------------------------------------------СОХРАНЕНИЕ И ЗАГРУЗКА СЕТКИ-------------------------------------------------*/

	void FuckMeApplication::sgBtnHandler() {
		QString filename = QFileDialog::getSaveFileName(this, "Сохранение сетки", "c:\\test\\picture", "Grid (*.grid)", &tr("*.grid"));
		QFile file(filename);

		if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
		{
			QTextStream stream(&file);

			stream << this->root.GetType() << endl;

			this->root.WriteTreeToFile(stream, 0);
		}
		file.close();
	}

	void FuckMeApplication::lgBtnHandler() {
		QImage image(256, 256, QImage::Format_Grayscale8);
		image.fill(Qt::white);

		this->loadImageFromQImage(image, ui.GridImage);

		QString filename = QFileDialog::getOpenFileName(this, tr("Load Grid"), "c:\\test\\", tr("Grid (*.grid) ;; All files (*.*)"), &tr("Grid (*.grid)"));
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream stream(&file);
			
			int t = 0;
			stream >> t;
			PolyType type = (PolyType)t;

			this->root.ReadTreeFromFile(stream, type);
			image = this->DrawGrid(image, this->root);

			file.close();

			this->loadImageFromQImage(image, ui.GridImage);
			ui.clearBtn->setEnabled(true);
		}
	}

/*----------------------------------------------------------СОХРАНЕНИЕ И ЗАГРУЗКА СЕТКИ-------------------------------------------------*/
/*-----------------------------------------------------------ВЫДЕЛЕНИЕ ОБЪЕКТА ПО КЛИКУ-------------------------------------------------*/

	void FuckMeApplication::ImageClickHandler(QMouseEvent* event) {
		Polygon poly = *(this->root.GetPolygonByPoint(QPoint(event->pos().x(), event->pos().y())));

		QVector<Polygon> neighbours;
		neighbours = poly.GetNeighbours(this->root, neighbours);

		QImage image(256, 256, QImage::Format_Grayscale8);
		image.fill(Qt::white);
		QPixmap px = QPixmap::fromImage(image);
		QPainter p(&px);
		QColor clr(0, 0, 0);

		for (Polygon poly : neighbours) {
			p.drawPolygon(poly.toQPolygon(), Qt::OddEvenFill);
		}
		image = px.toImage();

		this->loadImageFromQImage(image, ui.ObjImage);
	}

/*-----------------------------------------------------------ВЫДЕЛЕНИЕ ОБЪЕКТА ПО КЛИКУ-------------------------------------------------*/