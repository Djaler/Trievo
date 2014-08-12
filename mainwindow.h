#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QTime>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QPainter>
#include <QFile>
#include "sourceimage.h"

#include <QDebug>
class MainWindow : public QWidget
{
		Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		void paintEvent (QPaintEvent *e);
		double fitness(const QImage image);
	private:
		QGridLayout *layout;
		QPushButton *load;
		QPushButton *start;
		QPushButton *stop;
		QPushButton *loadMap;
		QPushButton *saveGen;
		QPushButton *saveRez;

		SourceImage *left;
		QLabel *right;

		int width;
		int height;
		QPixmap *imageMap;
		QImage source;
		QTime startTime;
		QTimer *timer;

		int step;
		double fitnessBefore;
		double fitnessAfter;
	public slots:
		void loadFile();
		void continueGenerate();
		void saveGenerate();
		void saveRezult();
		void startGenerate()
		{
			startTime=QTime::currentTime ();
			timer->start(1);

			start->hide ();
			stop->show ();
			saveGen->setDisabled (true);
			saveRez->setEnabled (true);
		}
		void stopGenerate()
		{
			timer->stop ();

			stop->hide ();
			start->show ();
			saveGen->setEnabled (true);
		}
		void paintStep()
		{
			paintEvent (0);
		}
};

#endif // MAINWINDOW_H
