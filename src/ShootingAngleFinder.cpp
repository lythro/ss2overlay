#include "ShootingAngleFinder.h"

#include <QImage>

#include <cmath>

#include <iostream>
using std::cout;
using std::endl;

#define PI 3.141592653589793
#define GRAVITY -9.460022

float ShootingAngleFinder::findAngle(QPixmap* pixmap, QPoint origin, bool &ok)
{
		vector<QPoint> points = findAnglePoints( pixmap, origin );
		if (points.size() < 20)
		{
			// not enough information to get the angle
			ok = false;
			return 0;
		}
		ok = true;

		int dx = 0;
		int dy = 0;
		// build one big vector-sum
		for (int i = 0; i < points.size(); i++)
		{
			dx += points[i].x() - origin.x();
			dy += points[i].y() - origin.y();
		}

		// calc the angle
		float angle = atan2( dy, -dx ) * (180./PI) + 180;

		// TODO mean derivation would be nice

		return angle+1; // dont ask me why ;-)
}

int ShootingAngleFinder::calculatePower(int dx, int dy, float angle, int &miss)
{
	float minDist = 0;
	int minDistVal = 0;
	int vz = 1;

	for (int v = 1; v <= 100; v++)
	{
		float vx = cos( angle * (PI/180.) ) * v;
		float vy = -sin( angle * (PI/180.) ) * v;

		float t = dx/vx;
		float y = vy * t - 0.5 * GRAVITY * t * t;

		// y > dy ? aimed to short! (image coords!)
		vz = y > dy ? -1 : 1;

		float dist = y - dy;
		dist *= dist < 0 ? -1 : 1;

		if (v > 1)
		{
			if (dist < minDist)
			{
				minDist = dist;
				minDistVal = v;
			}
		}
		else
		{
			minDist = dist;
			minDistVal = v;
		}
	}

	miss = vz * minDist;
	return minDistVal;
}

vector<QPoint> ShootingAngleFinder::findAnglePoints(QPixmap* pixmap, QPoint origin)
{
	int range = 200;

	QImage img = pixmap->toImage();

	int width = img.size().width();
	int height = img.size().height();

	vector<QPoint> v;

	for (int dx = -range; dx < range; dx++)
	{
		int x = origin.x() + dx;
		if (x < 20) continue;
		if (x > width-20) break;

		for (int dy = -range; dy < range; dy++)
		{
			// skip my user-name-area! evil white name!
			if ( -20 < dx && dx < 20 )
				if ( -40 < dy && dy < -25 )
					continue;

			//
			int y = origin.y() + dy;
			if (y < 30) continue; // cut the top - titlebar!
			if (y > height-30) break;

			QPoint p( x, y );
			QRgb rgb = img.pixel( p );
			QColor c( rgb );


			int sum = c.red() + c.green() + c.blue();

			int drg = c.red() - c.green();
			drg = drg < 0 ? drg * -1 : drg;

			int dgb = c.green() - c.blue();
			dgb = dgb < 0 ? dgb * -1 : dgb;

			int drb = c.red() - c.blue();
			drb = drb < 0 ? drb * -1 : drb;

			if ((sum > 200 & drg < 30 && dgb < 30 && drb < 30)
			 || (sum > 300 & drg < 50 && dgb < 50 && drb < 70))
					v.push_back( p );
		}
	}

	return v;
}
