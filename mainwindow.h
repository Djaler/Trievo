#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include "sourceimage.h"
class MainWindow : public QWidget
{
		Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		void paintEvent (QPaintEvent *e);
		qreal fitness(const QImage image, int left, int right, int top, int bottom);
	private:
		QGridLayout *layout;
		QPushButton *load;
		QPushButton *start;
		QPushButton *stop;
		QPushButton *loadImage;
		QPushButton *saveGen;
		QPushButton *saveRez;
		QSlider *sizeSlider;

		SourceImage *left;
		QLabel *right;

		int width;
		int height;
		QPixmap *loadMap;
		QImage source;
		QPixmap *imageMap;
		QPixmap *rezMap;
		double scaleFactor;
		QTime startTime;
		QTimer *timer;

		int step;
	public slots:
		void loadFile();
		void continueGenerate();
		void saveGenerate();
		void saveRezult();
		void startGenerate();
		void stopGenerate();
		void changeSize(int Size);
		void paintStep()
		{
			paintEvent (0);
		}
};

#endif // MAINWINDOW_H
