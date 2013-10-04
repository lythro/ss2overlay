#ifndef SPLITTER_H_
#define SPLITTER_H_

#include "Bullet.h"
#include "ShootingSimulator.h"

#include <cmath>

class Splitter : public Bullet
{
public:
	Splitter(ShootingSimulator* sim)
	{
		m_simulator = sim;
		m_splitted = false;
	}


	virtual void step( float stepsize )
	{
		if (m_splitted) return;
		Bullet::step( stepsize );

		if (m_vy > 50.)
		{
			// split!
			m_splitted = true;

			Bullet* b = new Bullet();
			b->setPosition( m_position );
			b->setVelocity( m_vx, m_vy );
			b->setAngle( m_angle - 10 );

			//m_simulator->addBullet( b );

			b = new Bullet();
			b->setPosition( m_position );
			b->setVelocity( m_vx, m_vy );
			b->setAngle( m_angle + 10 );

			//m_simulator->addBullet( b );
		}
	}


private:
	float power(){ return sqrt(m_vy * m_vy + m_vx * m_vx); }
	bool m_splitted; // already splitted

	ShootingSimulator* m_simulator;
};

#endif
