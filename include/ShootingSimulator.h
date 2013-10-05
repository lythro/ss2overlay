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
	ShootingSimulator( QPoint fieldsize, vector<int> map )
	{
		m_obstacles = map;
		m_fieldSize = fieldsize;
	}

	~ShootingSimulator()
	{
		reset();
	}
	
	void addBullet( Bullet* b )
	{
		m_bullets.push_back( b );
	}

	void setObstacles( vector<int> o )
	{
		m_obstacles = o;
	}


	// simulation
	void simulate( int steps = 50000, float step_size = 0.01, vector<bool>* ground_status = NULL );

	// visualization
	vector<QPoint> getTracerPoints()
	{
		return m_tracer;
	}

	// clear obstacles, bullets and tracer
	void reset()
	{
		m_obstacles.clear();
		
		for (int i = 0; i < m_bullets.size(); i++) delete m_bullets[i];
		m_bullets.clear();

		m_tracer.clear();
	}

private:
	// outer bounds of the field
	QPoint m_fieldSize;

	// obstacles for collision checking
	// i.e. the ground.
	vector<int> m_obstacles;

	// bullets to simulate
	vector<Bullet*> m_bullets;

	// tracer-points for visualization
	vector<QPoint> m_tracer;
};

#endif
