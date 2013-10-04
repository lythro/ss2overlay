#ifndef HOVERBALL_H_
#define HOVERBALL_H_

#include "Bullet.h"

#include <iostream>
using std::cout;
using std::endl;

class Hoverball : public Bullet
{
public:
	Hoverball()
	{
		m_hovertime = 0.;
	}

	virtual void step( float stepsize )
	{
		// remember y motion
		float old_vy = m_vy;
		float old_y = m_position.y();

		Bullet::step( stepsize );

		// if ball "dropped": hovertime!
		if (old_vy < 0. && m_vy > 0. && m_hovertime == 0.)
		{
			m_hovertime = 3.6; // TODO: adjust!
		}

		// if hovertime: hover!
		if (m_hovertime > 0)
		{
			m_position.ry() = old_y;
			m_vy = old_vy;

			m_hovertime -= stepsize;
		}
	}

private:
	float m_hovertime;
};

#endif
