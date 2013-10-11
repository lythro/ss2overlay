#ifndef BULLET_H_
#define BULLET_H_

#include <QPoint>
#include <QPointF>
#include <cmath>

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

class Bullet
{
public:
	Bullet(bool checkValid = true)
	{
		m_checkValid = checkValid;
		m_valid = true;
		m_aboveGround = true;

		m_vx = 0.;
		m_vy = 0.;
		m_angle = 0.;
		m_lastStepsize = 1.;

		m_ax = 0.;
		m_ay = 9.5;
	}

	void setPosition( QPointF p )
	{
		m_position = p;
	}
	void setPosition( QPoint p )
	{
		setPosition( QPointF( p ) );
	}

	void setAngle( int angle )
	{
		m_angle = angle % 360;
		float power = m_vx * m_vx + m_vy * m_vy;
		power = sqrt( power );

		setVelocity( (int)power );
	}

	void setVelocity( int power )
	{
		float PI = 3.14159265;
		m_vx = power * cos( m_angle * PI/180. );
		m_vy = -power * sin( m_angle * PI/180. );
	}

	void setVelocity( float vx, float vy )
	{
		m_vx = vx;
		m_vy = vy;

		float PI = 3.14159265;
		m_angle = (int) (atan2( -m_vy, m_vx ) * 180./PI);
	}

	// the most important of them all:
	// progression through time
	virtual void step( float stepsize )
	{
		// backup of the old position and the stepsize
		m_positionBefore = m_position;
		m_lastStepsize = stepsize;

		m_position.rx() += m_vx * stepsize;
		m_position.ry() += m_vy * stepsize;

		setVelocity( m_vx + m_ax * stepsize,
					m_vy + m_ay * stepsize );
	}

	// allow the simulator to hand collision handling to the bullet
	virtual void handleCollision( vector<int>& map, float scale  )
	{
		bool oldAbove = m_aboveGround;
		checkAboveGround( map, scale );

		if (oldAbove != m_aboveGround && m_checkValid) m_valid = false;
		
		// check for collision with the borders of the map
		if (map.size() < 2) return;
		if (	( x() <= 0)
			||	( x() >= ((map.size()-1) * scale)) )
		{
			int dy = (x() <= 0 ? y() - map[0] : y() - map[map.size()-1]);
			dy = dy > 0 ? dy : -dy;

			if (dy < 100)
			{
				
				int vz   = (x() < 1 ? +1 : -1);
				int corr = (vz * m_vx > 0 ? +1 : -1);

				// to prevent oscillation, only do this once.
				if (corr == -1)
				{
					// set the x position to the exact border
					if (x() <= 0) m_position.rx() = 0.;
					if (x() > 0)  m_position.rx() = (map.size()-1) * scale;
				}

				// just invert the x-direction
				setVelocity( corr * m_vx, m_vy );
			}
			else
			{
				// at the border and not bouncing?
				// --> invalid, no matter what.
				m_valid = false;
			}
		}
	}

	// these functions should not be needed
	// bullets must calculate their steps themselves,
	// else no bouncing etc is possible.
	// the simulator does not know the behaviour,
	// it just tells the bullet "you hit something"
	// or "step forward (through time)
	int x(){ return (int) m_position.x(); }
	int y(){ return (int) m_position.y(); }
	QPoint position(){ return m_position.toPoint(); }
	QPointF positionF(){ return m_position; }

	bool aboveGround(){ return m_aboveGround; }
	bool valid(){ return m_valid; }

	float vx(){ return m_vx; }
	float vy(){ return m_vy; }


	// In shellshock, gravity and air resistance are not equal for every bullet type.
	// Hence the bullet-classes need to know their own forces:
	//
	// Update:
	// This was intended to be re-implemented by the bullets in
	// a first version of this tool. But for the sake of bouncers
	// etc.. every bullet implements its own "step"-method (or "handleCollision")
	
	// accelleration
	virtual float ax(){ return m_ax; }
	virtual float ay(){ return m_ay; }

protected:
	virtual void checkAboveGround( vector<int>& map, float scale )
	{
		if (map.size() < 2) return;

		int left_index = (int)(m_position.x()/scale); // do NOT round up!
		if (left_index < 0) left_index = 0;
		if (left_index > map.size()-2) left_index = map.size()-2;

		int right_index = left_index + 1;

		// difference between point left of me and my x position
		float dx = m_position.x() - (left_index * scale);
		// differnce of height between points right and left of me
		float dy = map[right_index] - map[left_index];
		// incline of the line between those points
		float m = dy/(scale * 1.);
		// line height at my position
		float lh = map[left_index] + m * dx;

		// above the ground if my y is less than lh
		m_aboveGround = m_position.y() < lh;
	}


protected:
	// last used step-size
	float m_lastStepsize;

	// position
	QPointF m_position;
	QPointF m_positionBefore;

	// velocity
	float m_vx;
	float m_vy;

	// angle
	int m_angle;

	float m_ax;
	float m_ay;

	// bullet still valid?
	bool m_valid;
	bool m_checkValid;

	// submerged or not?
	bool m_aboveGround;
};

#endif
