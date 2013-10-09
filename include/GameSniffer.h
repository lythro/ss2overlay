#ifndef GAMESNIFFER_H_
#define GAMESNIFFER_H_

#include "qpcap.h"
#include <QObject>

#include <vector>
using std::vector;

class GameSniffer : public QObject
{
	Q_OBJECT

public:
	GameSniffer();
	~GameSniffer();

	// start sniffing
	void start();

public slots:
	// incoming packets from qpcap
	void processPackage(const uchar*);

signals:
	void mapUpdate(vector<int> map);
	void angleUpdate(int angle);

private:
	unsigned int m_net, m_mask;
	bool m_running;
	QPcap m_pcap;
};

#endif
