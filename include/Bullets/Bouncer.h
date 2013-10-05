#ifndef BOUNCER_H_
#define BOUNCER_H_

#include "Bullet.h"
#include <vector>
using std::vector;

class Bouncer : public Bullet
{
public:
	// disable invalidation on collision
	Bouncer() : Bullet( false ){}

	virtual void handleCollision( vector<int>& map )
	{
		Bullet::handleCollision( map );
		// TODO ground collision / bounce
	}
};

#endif
