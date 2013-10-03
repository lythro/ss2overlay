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
	}


	int x(){ return (int) m_position.x(); }
	int y(){ return (int) m_position.y(); }
	QPoint position(){ return m_position.toPoint(); }
	QPointF positionF(){ return m_position; }

	float vx(){ return m_vx; }
	float vy(){ return m_vy; }

	// In shellshock, gravity and air resistance are not equal for every bullet type.
	// Hence the bullet-classes need to know their own forces
	
	// in terms of accelleration
	virtual float ax(){ return 0.; }
	virtual float ay(){ return 9.5; } // TODO: adjust this!

private:
	// position
	QPointF m_position;

	// velocity
	float m_vx;
	float m_vy;

	// angle
	int m_angle;

};

#endif
