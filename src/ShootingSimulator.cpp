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
				ground_status->push_back( m_bullets[i]->m_aboveGround );

			}

			// move it
			Bullet* b = m_bullets[i];
			b->step( step_size );

			// check collisions
			b->handleCollision( m_obstacles );
		}
	}
}
