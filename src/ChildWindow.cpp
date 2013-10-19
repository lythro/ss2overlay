#include "ChildWindow.h"
#include "ui_childwindow.h"

#include "ui_settings.h"

#include "sleep.h"

#include "ColorClusterFinder.h"
#include "ShootingAngleFinder.h"

#include "ShootingSimulator.h"
#include "SurfaceDetector.h"

// gamesniffer is not available under windows
#ifdef linux
	#include "GameSniffer.h"
#endif

#include "Bullet.h"
#include "Hoverball.h"
#include "Boomerang.h"
#include "Splitter.h"
#include "Tunneler.h"
#include "Bouncer.h"

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
	QObject::connect( m_settingsUi->pushButtonAutoAngle, SIGNAL(clicked()),
					this, SLOT(toggleAutoAngle()) );

	m_autoAngle = true;

#ifdef _WIN32
	m_settingsUi->pushButtonAutoAngle->setEnabled( false );
#endif

	m_settingsWidget.show();


	// debug
	m_debugLabel = new QLabel();
	m_debugLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_debugLabel->show();

	m_angle = 0;

	// setup timer
	QObject::connect( &m_timer, SIGNAL(timeout()), this, SLOT(updateOverlay()) );
	m_timer.start( 1000 );

#ifdef linux
	cout << "linux" << endl;
	// setup map-sniffer
	QObject::connect( &m_sniffer, SIGNAL(mapUpdate(vector<int>)), this, SLOT(recieveMap(vector<int>)) );
	QObject::connect( &m_sniffer, SIGNAL(angleUpdate(int)), this, SLOT(recieveAngle(int)) );
	m_sniffer.start();
#endif
}


void ChildWindow::toggleAutoAngle()
{
	m_autoAngle = m_settingsUi->pushButtonAutoAngle->isChecked();
}

void ChildWindow::recieveAngle(int angle)
{
	if (m_autoAngle)
	{
		m_settingsUi->spinBoxAngle->setValue( angle );
	}
}

void ChildWindow::recieveMap(vector<int> map)
{
	m_map = map;
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
	m_tracerAboveGround.clear();
	m_tracerPoints.clear();

	m_angle = -1;

	// step 1: find the players position
	vector<QPoint> my_position = ColorClusterFinder::findCluster( &pic, colorPlayer.rgb() );

	if (my_position.size() > 1 || my_position.empty())
	{
		//cout << "player position not distinct" << endl;
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



#if WIN32
			// extracting the surface - if the GameSniffer is not available
			m_map = SurfaceDetector::extractSurface( pic, m_playerPosition );

			// debug: show the surface in the debug-label!
			QPixmap debugPixmap( g.width(), g.height() );

			QPainter paint( &debugPixmap );
			paint.setPen( QColor( 255, 255, 0 ) );
			for (int i = 0; i < surface.size(); i++)
			{
				paint.drawPoint( QPoint( i, surface[i] ) );
			}
			paint.end();

			m_debugLabel->setPixmap( debugPixmap );
			m_debugLabel->resize( g.width(), g.height() );
#endif

			// and the position of the gun
			QPoint gunPoint = ShootingAngleFinder::findGunEndpoint( &pic, m_playerPosition, colorPlayer.rgb() );

			// simulation
			ShootingSimulator sim( QPoint( g.width(), g.height() ), m_map );


			/* shot simulation */
			int relativeCount = -1;
			if (m_settingsUi->radioButtonShot->isChecked())
			{
				relativeCount = 0;
			}
		

			/* three-/five-shot simulation */
			if (m_settingsUi->radioButtonThreeshot->isChecked())
			{
				relativeCount = 1;
			}

			if (m_settingsUi->radioButtonFiveshot->isChecked())
			{
				relativeCount = 2;
			}
			for (int i = -relativeCount; i <= relativeCount; i++)
			{
				if (relativeCount == -1) break;
										/* invalid on collision only if one-shot! */
				Bullet* t = new Bullet( relativeCount == 0 );
				t->setPosition( gunPoint );
				t->setAngle( m_settingsUi->spinBoxAngle->value() + i*6 );
				t->setVelocity( m_settingsUi->spinBoxPower->value() );
				sim.addBullet( t );
			}

			/* hoverball simulation */
			if (m_settingsUi->radioButtonHoverball->isChecked())
			{
				Hoverball* b = new Hoverball();
				b->setPosition( gunPoint );
				b->setAngle( m_settingsUi->spinBoxAngle->value() );
				b->setVelocity( m_settingsUi->spinBoxPower->value() );
				sim.addBullet( b );
			}

			/* Boomerang simulation */
			if (m_settingsUi->radioButtonBoomerang->isChecked())
			{
				Boomerang* b = new Boomerang();
				b->setPosition( gunPoint );
				b->setAngle( m_settingsUi->spinBoxAngle->value() );
				b->setVelocity( m_settingsUi->spinBoxPower->value() );
				sim.addBullet( b );
			}

			/* Tunneler simulation */
			if (m_settingsUi->radioButtonTunneler->isChecked())
			{
				Tunneler* t = new Tunneler();
				t->setPosition( gunPoint );
				t->setAngle( m_settingsUi->spinBoxAngle->value() );
				t->setVelocity( m_settingsUi->spinBoxPower->value() );
				sim.addBullet( t );
			}

			/* Bouncer simulation. Warning: Bouncer != Three-Bouncer! */
			if (m_settingsUi->radioButtonBouncer->isChecked())
			{
				Bouncer* b = new Bouncer();
				b->setPosition( gunPoint );
				b->setVelocity( m_settingsUi->spinBoxPower->value() );
				b->setAngle( m_settingsUi->spinBoxAngle->value() );

				sim.addBullet( b );
			}


			sim.simulate( 20000, 0.005, &m_tracerAboveGround );

			int dx = gunPoint.x() - m_playerPosition.x();
			int dy = gunPoint.y() - m_playerPosition.y();

			// viz
			vector<QPoint> tracer = sim.getTracerPoints();
			
			//cout << "Tracer: " << tracer.size() << endl;
			
			m_tracerPoints.insert( m_tracerPoints.end(), tracer.begin(), tracer.end() );
		}
	}
	repaint(); // force repaint
}

void ChildWindow::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.setPen( QColor( 255, 255, 0 ) );

	// draw debug-points
	for (int i = 0; i < m_debugPoints.size(); i++)
	{
		//cout << "Cluster at: " << m_debugPoints[i].x() << ","
		//		<< m_debugPoints[i].y() << endl;
		
		// only draw if not too close to the player
		int dx = m_debugPoints[i].x() - m_playerPosition.x();
		int dy = m_debugPoints[i].y() - m_playerPosition.y();
		dx = (dx < 0 ? -dx : dx);
		dy = (dy < 0 ? -dy : dy);

		if (dx + dy > 15)
			p.drawPoint( m_debugPoints[i] );
	}


	// draw map-points/ lines
	//cout << "map: " << m_map.size() << endl;

	if (m_map.size() > 1)
	{
		float scale = ((float)(this->width())) / (m_map.size()-1);
		QPoint last( 0, m_map[0] );

		for (int i = 1; i < m_map.size(); i++)
		{
			QPoint tmp( i*scale, m_map[i] );

			p.setPen( QColor( 255, 0, 255 ) );
			p.drawLine( last, tmp );
			
			p.setPen( QColor( 255, 255, 0 ) );
			p.drawPoint( last );

			last = tmp;
		}
	}
	// draw tracer-points
	for (int i = 0; i < m_tracerPoints.size(); i++)
	{
		p.setPen( m_tracerAboveGround[i] ? QColor( 255, 255, 0 ) : QColor( 255, 0, 255 ) );
		// only draw if not too close to the player
		int dx = m_tracerPoints[i].x() - m_playerPosition.x();
		int dy = m_tracerPoints[i].y() - m_playerPosition.y();
		dx = (dx < 0 ? -dx : dx);
		dy = (dy < 0 ? -dy : dy);

		if (dx + dy > 20)
			p.drawPoint( m_tracerPoints[i] );
	}

	p.end();
}

ChildWindow::~ChildWindow()
{
    delete ui;
}
