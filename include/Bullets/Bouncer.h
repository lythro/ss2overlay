#ifndef BOUNCER_H_
#define BOUNCER_H_

#include "Bullet.h"
#include <cmath>
#include <vector>
using std::vector;

class Bouncer : public Bullet
{
public:
	// disable invalidation on collision
	Bouncer() : Bullet( false )
	{
		m_bouncing = 1;
	}

	virtual void handleCollision( vector<int>& map )
	{
		bool before = m_aboveGround;

		Bullet::handleCollision( map ); // this also updates the above-ground-status

		bool after = m_aboveGround;

		if (x() < 4 || x() > map.size() -5) return;

		// TODO ground collision / bounce
		if (before && !after)
		{
			if (!m_bouncing)
			{
				m_valid = false;
				return;
			}
			m_bouncing--;

			// collision with the ground, so what to do?
			//
			// 1. normal estimation
			// 2. "bouncing"
			
			// 1. normal estimation
			// "plain-vector"
			int px = 6;
			int py = map[x() + 3] - map[x() - 3];

			// "normal-vector"
			float nx = -py;
			float ny =  px;
			// normalize
			float length = sqrt( ny*ny + nx*nx );
			nx = nx/length;
			ny = ny/length;

			cout << "-------- Normal ---------" << endl;
			cout << "x: " << nx << endl;
			cout << "y: " << ny << endl << endl;

			// "bouncing"
			// http://www.blitzbasic.com/Community/posts.php?topic=56814
			// http://www.blitzbasic.com/codearcs/codearcs.php?code=670
			
			float vdot = m_vx * nx + m_vy * ny;
			float nfx = -2. * nx * vdot;
			float nfy = -2. * ny * vdot;

			setVelocity( 0.6 * (m_vx + nfx), 0.6 * (m_vy + nfy) );
		}
	}

private:
	int m_bouncing;
};

#endif
