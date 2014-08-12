#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent)
{
	qsrand (QTime(0,0,0).msecsTo (QTime::currentTime ()));

	left = new SourceImage;
	right=new QLabel;

	layout =new QGridLayout(this);

	load=new QPushButton(tr("Открыть"));
	layout->addWidget (load,1,0);
	connect(load,SIGNAL(pressed()),this,SLOT(loadFile()));

	loadMap=new QPushButton(tr("Продолжить"));
	layout->addWidget (loadMap,2,0);
	connect(loadMap,SIGNAL(pressed()),this,SLOT(continueGenerate()));

	start=new QPushButton(tr("Генерировать"));
	layout->addWidget (start,1,1);
	connect(start,SIGNAL(pressed()),this,SLOT(startGenerate()));
	start->setDisabled (true);

	saveGen=new QPushButton(tr("Сохранить прогресс"));
	layout->addWidget (saveGen,2,1);
	connect(saveGen,SIGNAL(pressed()),this,SLOT(saveGenerate()));
	saveGen->setDisabled (true);

	stop=new QPushButton(tr("Остановить"));
	layout->addWidget (stop,1,1);
	connect(stop,SIGNAL(pressed()),this,SLOT(stopGenerate()));
	stop->hide ();

	saveRez=new QPushButton(tr("Сохранить результат"));
	layout->addWidget (saveRez,3,0,1,2);
	connect(saveRez,SIGNAL(pressed()),this,SLOT(saveRezult()));
	saveRez->setDisabled (true);

	imageMap=new QPixmap;

	timer = new QTimer;
	connect(timer, SIGNAL(timeout()), this, SLOT(paintStep()));

	setLayout (layout);

	setMaximumSize (QWidget::minimumSizeHint ());

	step=0;
}
void MainWindow::paintEvent(QPaintEvent *e)
{
	if(e==0)
	{
		QPixmap oldMap(*imageMap);

		QPainter painter (imageMap);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setPen(QPen(Qt::transparent));
		QColor randColor(qrand()%255+0,qrand()%255+0,qrand()%255+0,qrand()%254+1);
		painter.setBrush(QBrush(randColor));

		int num=3;
		QPoint points[num];
		for(int i=0; i<num; i++)
		{
			points[i]=QPoint(qrand()%width,qrand()%height);
		}

		painter.drawPolygon (points,num);
		painter.end ();

		fitnessAfter=fitness(imageMap->toImage ());

		if(fitnessAfter<fitnessBefore)
		{
			right->setPixmap (*imageMap);
			fitnessBefore=fitnessAfter;

			qDebug()<<++step<<" "<<startTime.secsTo (QTime::currentTime ())<<endl;
			//startTime=QTime::currentTime ();
		}
		else
		{
			delete imageMap;
			imageMap=new QPixmap(oldMap);
		}
	}
}


double MainWindow::fitness(const QImage image)
{
	double fitness=0;
	for(int w=0; w<width; w++)
	{
		for(int h=0; h<height;h++)
		{
			QColor colorSource=source.pixel (w,h);
			QColor colorImage=image.pixel (w,h);

			double red=colorSource.redF ()-colorImage.redF ();
			double green=colorSource.greenF ()-colorImage.greenF ();
			double blue=colorSource.blueF ()-colorImage.blueF ();
			double pixelDistance=red * red +
									green * green +
									blue * blue;
			fitness+=pixelDistance;
		}
	}
	return fitness;
}
void MainWindow::saveGenerate()
{
	QString str = QFileDialog::getSaveFileName (this,tr("Выберите место для сохранения прогресса"),
												"",tr("Images (*.png *.jpg)"));
	if(str!="")
	{
		QPixmap backup(width*2,height);
		QPainter painter(&backup);
		painter.drawPixmap (0,0,width,height,
							QPixmap::fromImage (source),
							0,0,width,height);
		painter.drawPixmap (width,0,width,height,
							*imageMap,
							0,0,width,height);
		painter.end ();
		QFile file(str);
		file.open(QIODevice::WriteOnly);
		backup.save(&file);
	}
}
void MainWindow::continueGenerate()
{
	QString str = QFileDialog::getOpenFileName(0, tr("Выберите изображение"), "", "");
	if (str!="")
	{
		QPixmap loadMap(str);

		delete left;
		left = new SourceImage;
		left->loadImage (loadMap.copy (0,0,loadMap.width ()/2,loadMap.height ()));
		layout->addWidget (left,0,0);
		source=left->getImage ();

		width=left->width ();
		height=left->height ();

		delete imageMap;
		imageMap=new QPixmap(loadMap.copy (width,0,width,height));
		fitnessBefore=fitness(imageMap->toImage ());

		delete right;
		right = new QLabel;
		right->setPixmap (*imageMap);
		layout->addWidget (right,0,1);

		start->setEnabled (true);

		timer->stop ();

		setMaximumSize (QWidget::minimumSizeHint ());
	}
}
void MainWindow::loadFile()
{
	QString str = QFileDialog::getOpenFileName(this, tr("Выберите изображение"), "", "*.png *.jpg");
	if(str!="")
	{
		delete left;
		left = new SourceImage;
		left->loadImage (str,250);
		layout->addWidget (left,0,0);
		source=left->getImage ();

		width=left->width ();
		height=left->height ();

		delete imageMap;
		imageMap=new QPixmap(width,height);
		fitnessBefore=fitness(imageMap->toImage ());

		delete right;
		right = new QLabel;
		right->setPixmap (QPixmap(width,height));
		layout->addWidget (right,0,1);

		start->setEnabled (true);

		timer->stop ();

		setMaximumSize (QWidget::minimumSizeHint ());
	}
}
void MainWindow::saveRezult()
{
	QString str = QFileDialog::getSaveFileName (this,tr("Выберите место для сохранения результата"),
												"",tr("Images (*.png *.jpg)"));
	if(str!="")
	{
		QFile file(str);
		file.open(QIODevice::WriteOnly);
		imageMap->save(&file);
	}
}
