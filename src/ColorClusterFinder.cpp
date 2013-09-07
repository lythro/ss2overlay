#include "ColorClusterFinder.h"
#include <QImage>
#include <QSize>
#include <QRgb>
#include <QColor>

#include <iostream>

vector<QPoint> ColorClusterFinder::findCluster( QPixmap* pixmap, QRgb color )
{
	QImage img = pixmap->toImage();

	QSize size = img.size();
	
	vector<struct ClusterPoint> points;

	QColor cc( color ); // to compare with

	for (int x = 0; x < size.width(); x++)
	{
		for (int y = 0; y < size.height(); y++)
		{
			QRgb rgb = img.pixel( x, y );

			QColor rc( rgb );

			if (rc == cc)
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
	

	vector<QPoint> v;
	return v;
}
