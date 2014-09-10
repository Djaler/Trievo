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

	loadImage=new QPushButton(tr("Продолжить"));
	layout->addWidget (loadImage,2,0);
	connect(loadImage,SIGNAL(pressed()),this,SLOT(continueGenerate()));

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
	rezMap=new QPixmap;
	loadMap=new QPixmap;

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

		/* квадраты
		int x=qrand()%width;
		int y=qrand()%height;
		int s=(width-x<height-y)?(qrand()%(width-x)):(qrand()%(height-y));
		painter.drawRect(x,y,s,s);
		int l=x,t=y,r=x+s,b=y+s;
		*/

		/* круги
		int x=qrand()%width;
		int y=qrand()%height;
		int s=(width-x<height-y)?(qrand()%(width-x)):(qrand()%(height-y));
		painter.drawEllipse(x,y,s,s);
		int l=x,t=y,r=x+s,b=y+s;
		*/

		painter.end();

		if(fitness(imageMap->toImage (),l,r,t,b)<fitness(oldMap.toImage (),l,r,t,b))
		{
			qDebug()<<++step<<" "<<startTime.secsTo (QTime::currentTime ())<<endl;

			right->setPixmap (*imageMap);

			painter.begin(rezMap);
			painter.setRenderHint(QPainter::Antialiasing, true);
			painter.setPen(QPen(Qt::transparent));
			painter.setBrush(QBrush(randColor));
			for(int i=0; i<3; i++)
			{
				points[i].rx()*=scaleFactor;
				points[i].ry()*=scaleFactor;
			}
			painter.drawPolygon (points,3);
			painter.end();
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
		int origWidth=loadMap->width();
		int origHeight=loadMap->height();
		QPixmap backup(origWidth*2,origHeight);
		QPainter painter(&backup);
		painter.drawPixmap (0,0,origWidth,origHeight,
							*loadMap,
							0,0,origWidth,origHeight);
		painter.drawPixmap (origWidth,0,origWidth,origHeight,
							*rezMap,
							0,0,origWidth,origHeight);
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
		delete loadMap;
		loadMap = new QPixmap(str);
		QPixmap buf(loadMap->scaledToHeight(300));

		width=buf.width()/2;
		height=buf.height();

		delete left;
		left = new SourceImage;
		left->loadImage (buf.copy (0,0,width,height));
		layout->addWidget (left,0,0);
		source=left->getImage ();

		delete rezMap;
		rezMap=new QPixmap(loadMap->copy(loadMap->width()/2,0,loadMap->width()/2,loadMap->height()));

		delete imageMap;
		imageMap=new QPixmap(rezMap->scaledToHeight(300));

		buf=loadMap->copy(0,0,loadMap->width()/2,loadMap->height());
		delete loadMap;
		loadMap = new QPixmap(buf);

		delete right;
		right = new QLabel;
		right->setPixmap (*imageMap);
		layout->addWidget (right,0,1);

		start->setEnabled (true);
		saveGen->setDisabled(true);
		saveRez->setDisabled(true);
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
		delete loadMap;
		loadMap = new QPixmap(str);

		delete left;
		left = new SourceImage;
		left->loadImage(str,300);
		layout->addWidget (left,0,0);
		source=left->getImage ();

		width=left->width ();
		height=left->height ();

		delete imageMap;
		imageMap=new QPixmap(width,height);
		imageMap->fill (Qt::black);

		delete rezMap;
		rezMap=new QPixmap(loadMap->size());
		rezMap->fill(Qt::black);

		scaleFactor=static_cast<double>(loadMap->height())/static_cast<double>(left->getImage().height());

		delete right;
		right = new QLabel;
		right->setPixmap (*imageMap);
		layout->addWidget (right,0,1);

		start->setEnabled (true);
		saveGen->setDisabled(true);
		saveRez->setDisabled(true);
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
		rezMap->save(&file);
	}
}
void MainWindow::startGenerate()
{
	startTime=QTime::currentTime ();
	timer->start(1);

	start->hide ();
	stop->show ();
	saveGen->setEnabled (true);
	saveRez->setEnabled (true);
}
void MainWindow::stopGenerate()
{
	timer->stop ();

	stop->hide ();
	start->show ();
}
