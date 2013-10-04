#ifndef BOOMERANG_H_
#define BOOMERANG_H_

#include "Bullet.h"

class Boomerang : public Bullet
{
public:
	Boomerang() : Bullet()
	{
		m_ax = -25.;
	}
};

#endif
