#ifndef SHOOTINGSIMULATOR_H_
#define SHOOTINGSIMULATOR_H_

#include <QPoint>
#include "Bullet.h"

#include <vector>
using std::vector;


class ShootingSimulator
{
public:
	// setup
	ShootingSimulator( QPoint fieldsize )
	{
		m_fieldSize = fieldsize;
	}
	
	void addBullet( Bullet* b )
	{
		m_bullets.push_back( b );
	}

	void setObstacles( vector<QPoint> o )
	{
		m_obstacles = o;
	}


	// simulation
	void simulate( int steps = 50000, float step_size = 0.01 );

	// visualization
	vector<QPoint> getTracerPoints()
	{
		return m_tracer;
	}

	// clear obstacles, bullets and tracer
	void reset()
	{
		m_obstacles.clear();
		m_bullets.clear();
		m_tracer.clear();
	}

private:
	// outer bounds of the field
	QPoint m_fieldSize;

	// obstacles for collision checking
	// i.e. the ground.
	vector<QPoint> m_obstacles;

	// bullets to simulate
	vector<Bullet*> m_bullets;

	// tracer-points for visualization
	vector<QPoint> m_tracer;
};

#endif
