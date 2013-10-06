#include "MapSniffer.h"

#include "qpcapethernetpacket.h"
#include "qpcapippacket.h"
#include "qpcaptcppacket.h"

#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QHostAddress>

#include <iostream>
using std::cout;
using std::endl;
using std::flush;

MapSniffer::MapSniffer()
{
	m_running = false;
}

void MapSniffer::start()
{
	if (m_running) return;

	cout << "PCAP: looking for device..." << flush;
	QString device = m_pcap.lookupDevice();
	if (device.isEmpty())
	{
		cout << " not found." << endl;
		return;
	}
	else
	{
		cout << " found." << endl;
	}


	cout << "PCAP: open device..." << flush;
	bool ok = m_pcap.open( device, 65535, true );
	if (!ok)
	{
		cout << " failed. " << m_pcap.errorString().toStdString() << endl;
		return;
	}
	cout << " ok." << endl;

	cout << "PCAP: setting filter..." << flush;
	ok = m_pcap.setFilter( "ip" );
	if (!ok)
	{
		cout << " failed." << endl;
		return;
	}
	cout << " ok." << endl;

	m_pcap.start();

	QObject::connect( &m_pcap, SIGNAL(packetReady(const uchar*)), this, SLOT(processPackage(const uchar*)) );

	cout << "PCAP running." << endl;
	m_running = true;
}

void MapSniffer::processPackage(const uchar* raw)
{
	QPcapEthernetPacket ether( raw );
	if (ether.isIpPacket())
	{
		QPcapIpPacket ip = ether.toIpPacket();

		if (ip.isTcpPacket())
		{
			QPcapTcpPacket tcp = ip.toTcpPacket();

			// only TCP-packets are interesting			
			QByteArray data = tcp.data();

			// okay, how to process the data now?
			// first, we need to find the "StartTurn"-message
			if (data.contains( "StartTurn" ))
			{
				cout << "PCAP: StartTurn!" << endl;

				// so, we found the correct packet.
				// now we need to split it at '05',
				// and those again at '04'
				// and take only one-byte and two-byte segments as values
				vector<int> map;

				QByteArray tmp;
				for (int i = 0; i < data.size(); i++)
				{
					// splitter?
					if (data[i] == '\05' || data[i] == '\04')
					{
						if (0 < tmp.size() && tmp.size() < 3)
						{
							// convert the value from binary to int
							char* buf = new char[tmp.size()];
							for (int j = 0; j < tmp.size(); j++) buf[tmp.size() - 1 - j] = tmp[j];

							int val = *((unsigned short*) buf);

							delete[] buf;

							map.push_back( val );

						}
						tmp.clear();
					}
					else
					{
						// data.
						tmp.append( data[i] );
					}
				}

				emit mapUpdate(map);
			}
		}
	}
}


MapSniffer::~MapSniffer()
{
	if (m_running) m_pcap.close();
}
