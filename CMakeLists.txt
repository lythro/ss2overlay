cmake_minimum_required( VERSION 2.6 )
project( ss2overlay )

if ( UNIX )
	add_subdirectory( ./qpcap/qpcap )
else()
	message( "-- The PCAP-Features were not ported to Windows yet." )
	message( "   ss2overlay will use an image-based map-estimation instead of" )
	message( "   extracting the exact information from the games network traffic." )
endif()

add_subdirectory( src )
