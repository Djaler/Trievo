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

		QPoint points[3];
		int l=width;
		int r=0;
		int t=height;
		int b=0;
		for(int i=0; i<3; i++)
		{
			int x=qrand()%width;
			int y=qrand()%height;
			if(x<l)
				l=x;
			if(x>r)
				r=x;
			if(y<t)
				t=y;
			if(y>b)
				b=y;
			points[i]=QPoint(x,y);
		}
		painter.drawPolygon (points,3);
		painter.end ();
		if(fitness(imageMap->toImage (),l,r,t,b)<fitness(oldMap.toImage (),l,r,t,b))
		{
			qDebug()<<++step<<" "<<startTime.secsTo (QTime::currentTime ())<<endl;

			right->setPixmap (*imageMap);
		}
		else
		{
			delete imageMap;
			imageMap=new QPixmap(oldMap);
		}
	}
}

qreal MainWindow::fitness(const QImage image, int left, int right, int top, int bottom)
{
	qreal fitness=0;
	#pragma omp parallel reduction (+: fitness) num_threads(QThread::idealThreadCount ())
	{
		#pragma omp for
		for(int w=left; w<right; w++)
		{
			for(int h=top; h<bottom; h++)
			{
				QColor colorSource=source.pixel (w,h);
				QColor colorImage=image.pixel (w,h);

				qreal red=colorSource.redF ()-colorImage.redF ();
				qreal green=colorSource.greenF ()-colorImage.greenF ();
				qreal blue=colorSource.blueF ()-colorImage.blueF ();
				qreal pixelDistance=	red * red +
										green * green +
										blue * blue;
				fitness+=pixelDistance;
			}
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
	QString str = QFileDialog::getOpenFileName(0, tr("Выберите файл с сохраненным прогрессом"),
											   "", tr("Images (*.png *.jpg)"));
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

		delete right;
		right = new QLabel;
		right->setPixmap (*imageMap);
		layout->addWidget (right,0,1);

		start->setEnabled (true);

		timer->stop ();

		step=0;

		setMaximumSize (QWidget::minimumSizeHint ());
	}
}
void MainWindow::loadFile()
{
	QString str = QFileDialog::getOpenFileName(this, tr("Выберите изображение"),
											   "", tr("Images (*.png *.jpg)"));
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
		imageMap->fill (Qt::black);

		delete right;
		right = new QLabel;
		right->setPixmap (*imageMap);
		layout->addWidget (right,0,1);

		start->setEnabled (true);
		stop->hide ();
		start->show();

		timer->stop ();

		step=0;

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
