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
				// now, how to process it?
				
				// for a first attempt I will just "undo" the encoding
				// and print everything I got.
				vector<int> map;
				int mapcount = -19; // 19 entries before map data, which are 160 in count

				bool verbose = true;

				int index = 0;
				while (index < data.size())
				{
					if (mapcount <= 0 || mapcount > 160)
					{
						cout << mapcount << "| " << flush;
						verbose = true;
					}
					else verbose = false;

					// differ between datatypes
					int val;
					char buf[4];

					switch (data[index])
					{
					case '\x04':
						val = (unsigned char) data[++index];
						if (verbose) cout << "Value 1: " << val << endl;
						break;

					case '\x05':
						buf[1] = data[++index];
						buf[0] = data[++index];

						val = *((unsigned short*) buf);
						if (verbose) cout << "Value 2: " << val << endl;
						break;

					case '\x07':
						for (int i = 3; i >= 0; i--)
						{
							buf[i] = data[++index];
						}
						val = *((int*) buf);
						if (verbose) cout << "Value 4: " << val << endl;
						break;

					default:
						if ( (data[index] & '\xC0') == '\xC0' )
						{
							// C0 == 1100 0000
							// this means: a character-sequence is coming!
							// the number of chars is encoded in the 6 remaining bit
							int count = data[index] & '\x3F';

							if (verbose) cout << "Char[" << count << "]: " << flush;
							for (int i = 0; i < count; i++)
							{
								if (verbose) cout << data[++index];
							}
							if (verbose) cout << endl;
						}
						else if ( (data[index] & '\x80') == '\x80' )
						{
							// 80 == 1000 0000
							// this means: an int value encoded in my last bits
							val = data[index] & '\x3F';

							if (verbose) cout << "Value 0: " << val << endl;;
						}
						else if ( data[index] == '\x00' )
						{
							if (verbose) cout << "Bool   : false" << endl;
						}
						else if ( data[index] == '\x01' )
						{
							if (verbose) cout << "Bool   : true" << endl;
						}
						else
						{
							cout << "Unknown! " << (data[index] & '\x80') << endl;
							index = data.size(); // stop the loop!
						}
						break;
					} // switch

					index++;

					mapcount++;

					if (0 < mapcount && mapcount <= 161)
					{
						map.push_back( val );
					}
				} // while

				emit mapUpdate(map);
			}
		}
	}
}


MapSniffer::~MapSniffer()
{
	if (m_running) m_pcap.close();
}
