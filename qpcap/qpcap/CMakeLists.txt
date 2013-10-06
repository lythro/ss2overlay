cmake_minimum_required( VERSION 2.6 )

FIND_PACKAGE (Qt4 REQUIRED)


ADD_DEFINITIONS (${QT_DEFINITIONS})

#SET( QT_USE_QTXML TRUE )
#SET( QT_USE_QTOPENGL TRUE )
SET( QT_WRAP_CPP TRUE )


INCLUDE (${QT_USE_FILE})


SET( sources
	./qpcappacket.cpp
	./qpcap.cpp
)

SET( header
	./qpcap.h
	./qpcapethernetpacket.h
	./qpcaptcppacket.h
	./qpcapudppacket.h
	./qpcapippacket.h
)

QT4_WRAP_CPP( MOC_headers ${header} )

INCLUDE_DIRECTORIES( ${QT_INCLUDES} )

ADD_LIBRARY( qpcap ${sources} ${MOC_headers} )
