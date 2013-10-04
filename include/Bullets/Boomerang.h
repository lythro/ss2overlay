#ifndef BOOMERANG_H_
#define BOOMERANG_H_

#include "Bullet.h"
#

class Boomerang : public Bullet
{
public:
	Boomerang()
	{
		m_ax = 0.;
	}
	
	virtual void step( float stepsize )
	{
		// quick & dirty: its the first step, so calc the repell
		if (m_ax == 0.)
		{
			m_ax = -0.077 * m_vx;
		}

		Bullet::step( stepsize );
	}

};

#endif
