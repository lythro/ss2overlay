#include "ShootingAngleFinder.h"

#include <QImage>

#include <cmath>

#include <iostream>
using std::cout;
using std::endl;

#define PI 3.141592653589793
//#define GRAVITY -9.23 /* TODO calc a better value... */
#define GRAVITY -9.5

float ShootingAngleFinder::findAngle(QPixmap* pixmap, QPoint origin, bool &ok)
{
		vector<QPoint> points = findAnglePoints( pixmap, origin );
		if (points.size() < 100)
		{
			// not enough information to get the angle
			ok = false;
			return 0;
		}
		ok = true;

		// approach: every found point votes for an angle!
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

		//cout << "maxVotings: " << maxCount << endl;

		// accept all angles with at least 60% of maxCount votings
		int minCount = (int) (0.7 * maxCount);

		//cout << "minNeeded: " << minCount << endl;

		// mean of all acceptable angles, weighted
		float mean = 0;
		float used = 0;
		for (int i = 0; i < 360 * granularity; i++)
		{
			if (angle[i] > minCount)
			{
				used += angle[i];
				mean += angle[i] * (i - (180 * granularity)); // - 180*granularity to make the scale symmetric
												// (1 + 359)/2 = 180  | WRONG
												// (-179 + 179)/2 = 0 | RIGHT
			}
		}

		//cout << "used: " << used << endl;

		mean = mean / (granularity * used);

		mean += 180; // undo symmetry

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

QPoint ShootingAngleFinder::findGunEndpoint(QPixmap* pixmap, QPoint origin, QColor cc)
{
	QImage img = pixmap->toImage();

	// pre-work: find all the needed pixel
	vector<QPoint> possible;
	for (int dx = -20; dx <= 20; dx++)
	{
		for (int dy = -20; dy <= 20; dy++)
		{
			QPoint p = origin + QPoint( dx, dy );
			QRgb rgb = img.pixel( p );

			QColor rc( rgb );

			// compare with given color
			int dr = rc.red() - cc.red();
			int dg = rc.green() - cc.green();
			int db = rc.blue() - cc.blue();

			dr *= dr < 0 ? -1 : 1;
			dg *= dg < 0 ? -1 : 1;
			db *= db < 0 ? -1 : 1;

			if (dr + dg + db < 50) // tolerance 50
			{
				possible.push_back( p );
			}
		}
	}


	// first: find the maximum distance - in a 40x40 pixel area around
	// the players origin
	int maxDist = 0;

	for (int i = 0; i < possible.size(); i++)
	{
		QPoint p = possible[i];
		int dx = p.x() - origin.x();
		int dy = p.y() - origin.y();

		int dist = dx*dx + dy*dy;
		if (dist > maxDist)
		{
			maxDist = dist;
		}
	}

	// second: sum up all the points with maxDist distance
	// to the players origin and take the weighted sum as a result
	// this will hopefully find the middle of the guns end, not an edge
	
	float compare = sqrt( maxDist );

	vector<QPoint> endpoints;

	for (int i = 0; i < possible.size(); i++)
	{
		QPoint p = possible[i];
		int dx = p.x() - origin.x();
		int dy = p.y() - origin.y();
		
		//if (sqrt( dx*dx + dy*dy ) + 1. < compare)
		if ( dx*dx + dy*dy < maxDist - 2. )
		{
			continue;
		}
		else
		{
			// found a possible gun-endpoint.
			endpoints.push_back( p );
		}
	}

	// third: calculate the mean.
	QPoint mean( 0, 0 );
	for (int i = 0; i < endpoints.size(); i++) mean += endpoints[i];
	if (endpoints.size())
	{
		mean.rx() /= endpoints.size();
		mean.ry() /= endpoints.size();
	}

	return mean;
}

vector<QPoint> ShootingAngleFinder::findAnglePoints(QPixmap* pixmap, QPoint origin)
{
	QColor ignore( 255, 255, 0 ); // this color: text drawn by this program!

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
			if ( -15 < dx && dx < 15 )
				if ( -33 < dy && dy < -22 )
					continue;
			

			//
			int y = origin.y() + dy;
			if (y < 30) continue; // cut the top - titlebar!
			if (y > height-30) break;

			QPoint p( x, y );
			QRgb rgb = img.pixel( p );
			QColor c( rgb );

			if (c == ignore) continue;


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
