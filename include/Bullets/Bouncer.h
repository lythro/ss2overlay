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

	virtual void handleCollision( vector<int>& map, float scale )
	{
		bool before = m_aboveGround;
		Bullet::handleCollision( map, scale ); // this also updates the above-ground-status
		bool after = m_aboveGround;


		// above --> !above: collision with ground.
		if (before && !after)
		{
			if (!m_bouncing)
			{
				m_valid = false;
				return;
			}
			m_bouncing--;

			// collision with the ground, so what to do?
			// 0. 	calculate the exact point of collision
			// 		this is important to get the correct map-indices
			//
			// 1. 	normal estimation
			// 2. 	"bouncing"
			
			// 0. 	collision-point-calculation
			// it occured somewhere on the path from the last known position to this.
			float movedX = m_position.x() - m_positionBefore.x();
			float movedY = m_position.y() - m_positionBefore.y();

			cout << "movedX " << movedX << endl;
			cout << "movedY " << movedY << endl;

			// scale with stepsize
			movedX /= m_lastStepsize;
			movedY /= m_lastStepsize;

			// I will try a binary search in the elapsed time to approximate the position.
			// Just a few iterations will get us close enough.

			float lower = 0.;
			float upper = m_lastStepsize;
			float mid;
			for (int i = 0; i < 20; i++)
			{
				mid = (lower + upper)/2.;
				cout << "mid: " << mid;

				// check for collision.
				// how? just set the position and checkAboveGround.
				m_position.rx() = m_positionBefore.x() + mid * movedX;
				m_position.ry() = m_positionBefore.y() + mid * movedY;

				cout << "   " << m_position.x() << ", " << m_position.y() << " | ";
				Bullet::checkAboveGround( map, scale );

				// set the new lower and upper bounds.
				if (m_aboveGround)
				{
					cout << " above" << endl;
					lower = mid;
				}
				else
				{
					cout << " below" << endl;
					upper = mid;
				}
			}
			cout << 0. << "   -   " << mid << "   -   " << m_lastStepsize << endl;
			// lower and upper are more up-to-date now, but who cares?
			// m_position is set properly, we could also just do another iteration.
			// So let's just move on with the bounce.


			// 1. normal estimation
			// vector created by the points left and right of me
			// indices
			int left_index = (int)(m_position.x()/scale); // do NOT round up!
			if (left_index < 0) left_index = 0;
			if (left_index > map.size()-2) left_index = map.size()-2;
			int right_index = left_index + 1;

			// difference in x between both points
			float dx = scale * 1.;
			// difference in y between both points
			float dy = map[right_index] - map[left_index];
			// incline
			float m = dy/dx;

			float px = 1.;
			float py = m * 1.;


			// "normal-vector" by rotation the line-vector
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
