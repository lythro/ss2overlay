#include "ShootingSimulator.h"

#include <QPointF>
#include <iostream>
using std::cout;
using std::endl;

// everything else is inline
void ShootingSimulator::simulate( int steps, float step_size, vector<bool>* ground_status )
{
	for (int step = 0; step < steps; step++)
	{
		// for every bullet:
		for (int i = 0; i < m_bullets.size(); i++)
		{
			if (!m_bullets[i]->valid()) continue;

			// save its position for the tracer
			// TODO boundary / obstacle-check
			QPointF pos = m_bullets[i]->positionF();
			if (pos.x() < 0 || pos.y() < 0
				|| pos.x() > m_fieldSize.x()
				|| pos.y() > m_fieldSize.y())
			{	
			}
			else
			{
				m_tracer.push_back( pos.toPoint() );
				ground_status->push_back( m_bullets[i]->aboveGround() );

			}

			// move it
			Bullet* b = m_bullets[i];
			b->step( step_size );

			// check collisions
			float scale = 1.;
			if (m_obstacles.size() > 1) scale = (float)m_fieldSize.x()/(float)m_obstacles.size();
			b->handleCollision( m_obstacles, scale );
		}
	}
}
