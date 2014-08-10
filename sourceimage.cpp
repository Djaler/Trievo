#include "sourceimage.h"
SourceImage::SourceImage(QWidget *parent)
	: QWidget(parent)
{
	label = new QLabel(this);
	label->setScaledContents (true);
}

void SourceImage::loadImage(QString name,int scale)
{
	pixmap.load (name);
	if(pixmap.width ()>pixmap.height ())
		pixmap=pixmap.scaledToWidth (scale);
	else
		pixmap=pixmap.scaledToHeight (scale);
	label->setPixmap (pixmap);
	setFixedSize (pixmap.width (),pixmap.height ());
}

QImage SourceImage::getImage()
{
	return pixmap.toImage ();
}


void SourceImage::loadImage(QPixmap image)
{
	pixmap=image;
	label->setPixmap (pixmap);
	setFixedSize (pixmap.width (),pixmap.height ());
}
