#ifndef BULLET_H_
#define BULLET_H_

#include <QPoint>
#include <QPointF>
#include <cmath>

class Bullet
{
public:
	Bullet()
	{
		m_vx = 0.;
		m_vy = 0.;
		m_angle = 0.;

		m_ax = 0.;
		m_ay = 9.5;
	}

	void setPosition( QPointF p )
	{
		m_position = p;
	}
	void setPosition( QPoint p )
	{
		m_position = QPointF( p );
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
		m_position.rx() += m_vx * stepsize;
		m_position.ry() += m_vy * stepsize;

		/*
		m_vx += m_ax * stepsize;
		m_vy += m_ay * stepsize;
		*/

		setVelocity( m_vx + m_ax * stepsize,
					m_vy + m_ay * stepsize );
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

	float vx(){ return m_vx; }
	float vy(){ return m_vy; }

	// In shellshock, gravity and air resistance are not equal for every bullet type.
	// Hence the bullet-classes need to know their own forces
	
	// in terms of accelleration
	virtual float ax(){ return m_ax; }
	virtual float ay(){ return m_ay; } // TODO: adjust this!

protected:
	// position
	QPointF m_position;

	// velocity
	float m_vx;
	float m_vy;

	// angle
	int m_angle;

	float m_ax;
	float m_ay;

};

#endif
