#include "ShootingAngleFinder.h"

#include <QImage>

#include <cmath>

#include <iostream>
using std::cout;
using std::endl;

#define PI 3.141592653589793
#define GRAVITY -9.23 /* TODO calc a better value... */

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

		/*
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
		*/

		// different approach: weight the possible angles!
		int granularity = 1;
		vector<int> angle;
		for (int i = 0; i < 360 * granularity; i++) angle.push_back( 0 );

		for (int i = 0; i < points.size(); i++)
		{
			int dx = points[i].x() - origin.x();
			int dy = points[i].y() - origin.y();
			
			float fangle = atan2( dy, -dx ) * (180./PI) + 180;

			int rounded = (int) ((fangle*granularity) + 0.5);
			if (rounded == 360 * granularity) rounded = 360*granularity - 1;

			// vote!
			angle[rounded] += 1;
		}

		// find the highest rating
		int maxCount = 0;
		for (int i = 0; i < 360 * granularity; i++)
		{
			if (angle[i] > maxCount) maxCount = angle[i];
		}

		cout << "maxVotings: " << maxCount << endl;

		// accept all angles with at least 60% of maxCount votings
		int minCount = (int) (0.6 * maxCount);

		cout << "minNeeded: " << minCount << endl;

		// mean of all acceptable angles
		float mean = 0;
		float used = 0;
		for (int i = 0; i < 360 * granularity; i++)
		{
			if (angle[i] > minCount)
			{
				used++;
				mean += i;
			}
		}

		cout << "used: " << used << endl;

		mean = mean / (granularity * used);

		return mean;
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
