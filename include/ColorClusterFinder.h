#ifndef COLORCLUSTERFINDER_H_
#define COLORCLUSTERFINDER_H_

#include <QPoint>
#include <QPixmap>

#include <vector>
using std::vector;

class ColorClusterFinder
{
public:
	// returns the mass center of any cluster of the given color
	// value in the pixmap
	static vector<QPoint> findCluster(QPixmap* pixmap, QRgb color, QPoint = QPoint(0, 0));

private:
	struct ClusterPoint{
		QPoint p;
		int cluster;
	};
};

#endif
