#ifndef TUNNELER_H_
#define TUNNELER_H_

#include "Bullet.h"

class Tunneler : public Bullet
{
public:
	// turn off invalidation through collision
	Tunneler() : Bullet( false ){}

	virtual void step( float stepsize )
	{
		m_position.rx() += m_vx * stepsize;
		m_position.ry() += m_vy * stepsize;

		// inverse gravity if underground
		float vz = m_aboveGround ? +1 : -0.97;

		setVelocity( m_vx + m_ax * stepsize,
					m_vy + vz * m_ay * stepsize );
	}

	virtual void handleCollision( vector<int>& map )
	{
		bool below = !m_aboveGround;

		Bullet::handleCollision( map );

		if (below && m_aboveGround)
		{
			// was below, now above
			// --> explosion!
			m_valid = false;
		}
	}
};

#endif
