#ifndef SOURCEIMAGE_H
#define SOURCEIMAGE_H

#include <QtGui>
#include <QtWidgets>

class SourceImage : public QWidget
{
        Q_OBJECT
    public:
        explicit SourceImage(QWidget *parent = 0);
        QImage getImage();
        void loadImage(QString name, int scale);
        void loadImage(QPixmap image);

    private:
        QPixmap pixmap;
        QLabel *label;
};

#endif // SOURCEIMAGE_H
