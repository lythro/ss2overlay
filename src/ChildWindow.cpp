#include "ChildWindow.h"
#include "ui_childwindow.h"

#include "ui_settings.h"

#include "sleep.h"

#include "ColorClusterFinder.h"
#include "ShootingAngleFinder.h"

#include "ShootingSimulator.h"
#include "Bullet.h"

#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QDesktopWidget>
#include <QRgb>
#include <QPoint>
#include <QRect>
#include <QRegion>
#include <QPalette>

#include <QShortcut>
#include <QKeySequence>


#include <cmath>
#include <iostream>
#include <vector>
using namespace std;

// used for "click-through"
#if _WIN32
    #include <windows.h>
#else
	#include <QX11Info>
	#include <X11/Xlib.h>
	#include <X11/extensions/shape.h>
#endif

ChildWindow::ChildWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::ChildWindow)
{

	setAttribute(Qt::WA_TranslucentBackground, true ); // important: before! clickthrough!

	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	// set window-click-through
#if _WIN32
	HWND hwnd = (HWND) winId();
	LONG styles = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, styles | WS_EX_TRANSPARENT);
#else
	QRegion region;
	XShapeCombineRegion( QX11Info::display(), winId(), 
							ShapeInput, 0, 0, region.handle(), ShapeSet );
#endif


	ui->setupUi(this);

	m_settingsUi = new Ui::SettingsWidget();
	m_settingsUi->setupUi( &m_settingsWidget );
	
	m_settingsUi->spinBoxPower->setValue( 74 );
	m_settingsUi->spinBoxAngle->setValue( 55 );

	m_settingsWidget.setWindowFlags( Qt::WindowStaysOnTopHint );

	QObject::connect( m_settingsUi->pushButton, SIGNAL(clicked()),
					this, SLOT(estimateCurrentState()) );

	m_settingsWidget.show();


	// debug
	//m_debugLabel = new QLabel();
	//m_debugLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	//m_debugLabel->show();

	m_angle = 0;

	// setup timer
	QObject::connect( &m_timer, SIGNAL(timeout()), this, SLOT(updateOverlay()) );
	m_timer.start( 1000 );
}

void ChildWindow::estimateCurrentState()
{
	QRect g = geometry(); // client area geometry
	QPixmap pic = QPixmap::grabWindow( QApplication::desktop()->winId(),
					g.x(), g.y(), g.width(), g.height() );
	
	QColor colorPlayer( 4, 153, 4 ); // player-tank-green
	QColor colorEnemy( 203, 0, 0 ); // enemy-tank-red

	vector<QPoint> my_position = ColorClusterFinder::findCluster( &pic, colorPlayer.rgb() );

	if (my_position.size() == 1)
	{
		bool ok;
		float angle = ShootingAngleFinder::findAngle( &pic, my_position[0], ok );
		int power = ShootingAngleFinder::lastPower();

		m_settingsUi->spinBoxPower->setValue( power );
		m_settingsUi->spinBoxAngle->setValue( (int)( angle + 0.5 ) );
	}
}


void ChildWindow::updateOverlay()
{
	QRect g = geometry(); // client area geometry
	QPixmap pic = QPixmap::grabWindow( QApplication::desktop()->winId(),
					g.x(), g.y(), g.width(), g.height() );
	
	QColor colorPlayer( 4, 153, 4 ); // player-tank-green
	QColor colorEnemy( 203, 0, 0 ); // enemy-tank-red


	m_debugPoints.clear();
	m_enemyTargetInfos.clear();
	m_angle = -1;

	// step 1: find the players position
	vector<QPoint> my_position = ColorClusterFinder::findCluster( &pic, colorPlayer.rgb() );

	if (my_position.size() > 1 || my_position.empty())
	{
		cout << "player position not distinct" << endl;
	}
	else
	{
		// DEBUG - points used for estimating the shooting angle
		//vector<QPoint> anglepoints = ShootingAngleFinder::findAnglePoints( &pic, my_position[0] );
		//m_debugPoints.insert( m_debugPoints.end(), anglepoints.begin(), anglepoints.end() );

		m_playerPosition = my_position[0];
		m_debugPoints.push_back( my_position[0] );

		// locate the enemies
		vector <QPoint> enemies = ColorClusterFinder::findCluster( &pic, colorEnemy.rgb(), m_playerPosition );
			
		m_debugPoints.insert( m_debugPoints.end(), enemies.begin(), enemies.end() );

		// step 5: sort out the red markers (over enemies right now - need to locate allies to delete their markers)
		for (int i = 0; i < enemies.size(); i++)
		{
			QPoint* a = &(enemies[i]);
			if (a->x() == -1) continue;

			// check all the other locations - mark found "red markers" with -1
			for (int j = i+1; j < enemies.size(); j++)
			{
				QPoint* b = &(enemies[j]);
				if (b->x() == -1) continue;

				// in a 4 dot width area
				if (a->x() - 2 < b->x() && b->x() < a->x() + 2)		// TODO  adjust this
				{
					// one is less than 50 dots below the other
					if (a->y() > b->y() && a->y()-50 < b->y())
					{
						// a higher b, but a-50 lower b --> a is bs marker!
						a->rx() = -1;
						a->ry() = -1;
					}
					else if (b->y() > a->y() && b->y()-50 < a->y())
					{
						// b higher a, but b-50 lower a --> b is as marker!
						b->rx() = -1;
						b->ry() = -1;
					}
				}
			}

			// step 6: TESTING SIMULATION
			

			ShootingSimulator sim( QPoint( g.width(), g.height() ) );

			Bullet b;
			b.setPosition( m_playerPosition );
			
			b.setAngle( m_settingsUi->spinBoxAngle->value() );
			b.setVelocity( m_settingsUi->spinBoxPower->value() );

			sim.addBullet( b );

			// three-ball!
			if (m_settingsUi->checkBox->isChecked())
			{
				for (int i = -1; i <= 1; i++)
				{
					if (i == 0) continue;
	
					Bullet t;

					t.setPosition( m_playerPosition );
					t.setAngle( m_settingsUi->spinBoxAngle->value() + i*6 );
					t.setVelocity( m_settingsUi->spinBoxPower->value() );

					sim.addBullet( t );
				}
			}

			sim.simulate( 10000, 0.01);

			QPoint gunPoint = ShootingAngleFinder::findGunEndpoint( &pic, m_playerPosition, colorPlayer.rgb() );
			int dx = gunPoint.x() - m_playerPosition.x();
			int dy = gunPoint.y() - m_playerPosition.y();

			// viz
			vector<QPoint> tracer = sim.getTracerPoints();
			
			cout << "Tracer: " << tracer.size() << endl;
			// adjust tracer by moving it to the guns endpoint
			for (int i = 0; i < tracer.size(); i++)
			{
				tracer[i].rx() += dx;
				tracer[i].ry() += dy;
			}

			m_debugPoints.insert( m_debugPoints.end(), tracer.begin(), tracer.end() );


			/*
			for (int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i].x() == -1) continue;

			}
			*/
		}
	}
	repaint(); // force repaint
}

void ChildWindow::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.setPen( QColor( 255, 255, 0 ) );

	//cout << "to draw: " << m_debugPoints.size() << endl;

	for (int i = 0; i < m_debugPoints.size(); i++)
	{
		if (0 < i && i  < 10 ) continue; // TODO skip the first n points to unblock gun endpoint

		//cout << "Cluster at: " << m_debugPoints[i].x() << ","
		//		<< m_debugPoints[i].y() << endl;
		p.drawPoint( m_debugPoints[i] );
	}


	/*
	// draw found angle at players position
	QPoint pPos = m_playerPosition - QPoint( 20, -20 );
	p.drawText( pPos, QString( "Angle: " ) + QString::number( m_angle ) );


	// draw target infos!
	for (int i = 0; i < m_enemyTargetInfos.size(); i++)
	{
		TargetInfo t = m_enemyTargetInfos[i];

		QPoint drawPos = t.pos - QPoint( 20, -30 );

		if (drawPos.x() < 10) drawPos.rx() = 10;
		if (drawPos.x() > width() - 60) drawPos.rx() = width() - 60;

		if (drawPos.y() > height() - 30) drawPos.ry() = t.pos.y() - 30;


		p.drawText( drawPos, QString( "Vel.: " ) + QString::number( t.velocity ) );
		drawPos.ry() += 10;

		p.drawText( drawPos, QString( "Miss: " ) + QString::number( t.miss ) );

	}
	*/

	p.end();
}

ChildWindow::~ChildWindow()
{
    delete ui;
}
