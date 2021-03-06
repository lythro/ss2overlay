#include "ColorClusterFinder.h"
#include <QImage>
#include <QSize>
#include <QRgb>
#include <QColor>

#include <iostream>

vector<QPoint> ColorClusterFinder::findCluster( QPixmap* pixmap, QRgb color, QPoint avoid )
{
	QImage img = pixmap->toImage();

	QSize size = img.size();
	
	vector<struct ClusterPoint> points;

	QColor cc( color ); // to compare with

	for (int x = 0; x < size.width(); x++)
	{
		for (int y = 0; y < size.height(); y++)
		{
			// spare the area exactly over the players position (avoid red-marker as enemy)
			int dx = x - avoid.x();
			if ( -10 < dx && dx < 10 ) continue;

			QRgb rgb = img.pixel( x, y );

			QColor rc( rgb );

			int dr = rc.red() - cc.red();
			int dg = rc.green() - cc.green();
			int db = rc.blue() - cc.blue();

			dr *= dr < 0 ? -1 : 1;
			dg *= dg < 0 ? -1 : 1;
			db *= db < 0 ? -1 : 1;

			if (dr + dg + db < 30)
			//if (rc == cc)
			{
				// find all the correct points
				//points.push_back( QPoint( x, y ) );

				struct ClusterPoint p;
				p.p = QPoint( x, y );
				p.cluster = -1;

				points.push_back( p );
			}
		}
	}

	// todo: cluster!
	
	int clusterNumber = 0;
	// for every point
	for (int i = 0; i < points.size(); i++)
	{
		// first: if its not in a cluster:
		if (points[i].cluster == -1)
		{
			points[i].cluster = clusterNumber++;
		}

		// search for nearest points, add them to the cluster
		for (int j = i; j < points.size(); j++)
		{
			// only if not already clustered!
			if (points[j].cluster == -1)
			{
				// and distance < 30 ?
				int dx = points[j].p.x() - points[i].p.x();
				int dy = points[j].p.y() - points[i].p.y();

				if ((dx*dx + dy*dy) < 30*30)
				{
					points[j].cluster = points[i].cluster;
				}
			}
		}

	}

	// now: every point should be in a cluster
	// there should be clusterNumber clusters
	//
	// count their number of points, if > ... 5 (?) add their center of mass to the list
	vector<QPoint> v;

	for (int i = 0; i < clusterNumber; i++)
	{
		int count = 0;
		int sumX = 0;
		int sumY = 0;

		for (int j = 0; j < points.size(); j++)
		{
			if (points[j].cluster == i)
			{
				count++;
				sumX += points[j].p.x();
				sumY += points[j].p.y();
			}
		}

		if (count > 5)
		{
			QPoint p( sumX/count, sumY/count );
			v.push_back( p );
		}
	}

	return v;
}
