######################################################################
# Automatically generated by qmake (2.01a) Sat Mar 12 14:26:18 2011
######################################################################

TEMPLATE = lib
TARGET = qpcap

DEPENDPATH += .
INCLUDEPATH += .

LIBS += -lpcap
QT += network

# Input
HEADERS += qpcap.h qpcapethernetpacket.h qpcapippacket.h qpcaptcppacket.h qpcapudppacket.h
SOURCES += qpcap.cpp qpcappacket.cpp