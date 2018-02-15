#ifndef _LABEL_H_
#define _LABEL_H_

#ifdef _WIN64

#include <QMouseEvent>
#include <QLabel>

#else

#include <QtWidgets/QLabel>
#include <QtGui/QMouseEvent>

#endif // _WIN64

class QCLabel :
	public QLabel
{
	Q_OBJECT
public:
	explicit QCLabel(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	//explicit QCLabel(const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~QCLabel();
signals:
	void clicked(QMouseEvent* event);
protected:
	void mousePressEvent(QMouseEvent* event);
};

#endif