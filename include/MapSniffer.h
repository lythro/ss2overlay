#ifndef MAPSNIFFER_H_
#define MAPSNIFFER_H_

#include "qpcap.h"
#include <QObject>

#include <vector>
using std::vector;

class MapSniffer : public QObject
{
	Q_OBJECT

public:
	MapSniffer();
	~MapSniffer();

	// start sniffing
	void start();

public slots:
	// incoming packets from qpcap
	void processPackage(const uchar*);

signals:
	void mapUpdate(vector<int> map);

private:
	bool m_running;
	QPcap m_pcap;
};

#endif
