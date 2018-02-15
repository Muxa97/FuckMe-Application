#include "QCLabel.h"

QCLabel::QCLabel(QWidget *parent, Qt::WindowFlags f)
	: QLabel(parent)
{
	//setText(text);
}

QCLabel::~QCLabel()
{
}

void QCLabel::mousePressEvent(QMouseEvent* event)
{
	emit clicked(event);
}