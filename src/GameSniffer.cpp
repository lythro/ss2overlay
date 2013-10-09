#include "GameSniffer.h"

#include "qpcapethernetpacket.h"
#include "qpcapippacket.h"
#include "qpcaptcppacket.h"

#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QHostAddress>

#include <cstdio>
#include <iostream>
using std::cout;
using std::endl;
using std::flush;

GameSniffer::GameSniffer()
{
	m_running = false;
}

void GameSniffer::start()
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
		//cout << " found." << endl;
		cout << " " << device.toStdString() << endl;
	}

	cout << "PCAP: getting device network... " << flush;
	m_pcap.deviceNetMask( device, m_net, m_mask );
	if (m_net == -1)
	{
		cout << "failed." << endl;
		return;
	}
	
	for (int i = 0; i < 4; i++)
	{
		unsigned char c;
		c = ( m_net >> (i*8) ) & 0xFF;

		cout << ((int) c);
		if (i < 3) cout << ".";
	}
	cout << "  /  ";
	for (int i = 0; i < 4; i++)
	{
		unsigned char c;
		c = ( m_mask >> (i*8) ) & 0xFF;

		cout << ((int) c ) << ".";
	}

	cout << endl;


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

void GameSniffer::processPackage(const uchar* raw)
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
					/*
					if (mapcount <= 0 || mapcount > 160)
					{
						cout << mapcount << "| " << flush;
						verbose = true;
					}
					else */
						verbose = false;


					// differ between datatypes
					int val;
					char buf[4];

					switch (data[index])
					{
					case '\x04': // confirmed.
						val = (unsigned char) data[++index];
						if (verbose) cout << "Value 1: " << val << endl;
						break;

					case '\x05': // confirmed.
						buf[1] = data[++index];
						buf[0] = data[++index];

						val = *((unsigned short*) buf);
						if (verbose) cout << "Value 2: " << val << endl;
						break;

					case '\x07': // quite sure.
						for (int i = 3; i >= 0; i--)
						{
							buf[i] = data[++index];
						}
						val = *((int*) buf);
						if (verbose) cout << "Value 4: " << val << endl;
						break;

					default:
						if ( (data[index] & '\xC0') == '\xC0' ) // confirmed.
						{
							// C0 == 1100 0000
							// this means: a character-sequence is coming!
							// the number of chars is encoded in the 6 remaining bit
							int count = data[index] & '\x3F';

							cout << "Char[" << count << "]: " << flush;
							for (int i = 0; i < count; i++)
							{
								char c =  data[++index];
								if (verbose) cout << c;
							}
							if (verbose) cout << endl;
						}
						else if ( (data[index] & '\x80') == '\x80' ) // confirmed
						{
							// 80 == 1000 0000
							// this means: an int value encoded in my last bits
							val = data[index] & '\x3F';

							if (verbose) cout << "Value 0: " << val << endl;;
						}
						else if ( data[index] == '\x00' ) // just a guess
						{
							if (verbose) cout << "Bool   : false" << endl;
						}
						else if ( data[index] == '\x01' ) // just a guess
						{
							if (verbose) cout << "Bool   : true" << endl;
						}
						else
						{
							QByteArray hex( data );
							hex = hex.toHex();

							cout << "Unknown! 0x" << hex[index*2] << hex[index*2+1] << endl;
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

				cout << "mapsize: " << map.size() << endl;
				emit mapUpdate(map);
			}
			// but we are not only interested in "StartTurn"
			// "TurretUpdate" gives us information about our precise shooting angle - as a double!
			else if ( data.contains( "TurretUpdate" ) )
			{
				int pos = data.indexOf( "TurretUpdate" ) + 12;

				// we also get turret-updates of our enemies!
				// so we need to check if we are the source of the packet.
				bool match = true;
				for (int i = 0; i < 4; i++)
				{
					unsigned char a = (m_mask >> (i*8)) & (m_net >> (i*8));
					unsigned char b = (m_mask >> (i*8)) & (ip.source().toIPv4Address() >> ((3-i)*8));

					//cout << ((int) a) << " -- " << ((int) b) << " | ";

					if ( a != b )
					{
						match = false;
						break;
					}
				}
				cout << endl;
				if (!match)
				{
					return;
				}


				QString type( "nope." );
				int angle = -1;
				// after the position of "TurretUpdate" there comes a 0x03 byte,
				// indicating a double value.
				if (data[pos] == '\x03') // confirmed.
				{
					type = "double";

					char buf[8];
					for (int i = 7; i >= 0; i--) buf[i] = data[++pos];

					double tmp = (270. - (*((double*) buf) + 180.)) + 0.5;
					if (tmp < 0.) tmp += 360.;
					angle = (int) tmp;
				}
				// or a 0x04 byte...  :-(
				else if (data[pos] == '\x04') // confirmed.
				{
					type = "1 byte";
					angle = (unsigned char) data[++pos];
				}
				// and possibly even a 0x05 byte...
				else if (data[pos] == '\x05') // confirmed.
				{
					type = "2 byte";

					char buf[2];
					buf[1] = data[++pos];
					buf[0] = data[++pos];

					angle = *((unsigned short*) buf);
				}
				else if ( (data[pos] & '\x80') == '\x80' )
				{
					type = "in-byte";
					angle = data[pos] & '\x3F';
				}

				emit angleUpdate( angle );
			}
		}
	}
}


GameSniffer::~GameSniffer()
{
	if (m_running) m_pcap.close();
}
