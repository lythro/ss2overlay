#include "ShootingSimulator.h"

#include <QPointF>

#include <iostream>
using std::cout;
using std::endl;

// everything else is inline
void ShootingSimulator::simulate( int steps, float step_size )
{
	for (int step = 0; step < steps; step++)
	{
		// for every bullet:
		for (int i = 0; i < m_bullets.size(); i++)
		{
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
			}

			// move it
			Bullet* b = m_bullets[i];
			b->step( step_size );
		}
	}
}
