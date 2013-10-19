FIND_PACKAGE (Qt4 REQUIRED)


ADD_DEFINITIONS (${QT_DEFINITIONS})

#SET( QT_USE_QTXML TRUE )
#SET( QT_USE_QTOPENGL TRUE )
SET( QT_WRAP_CPP TRUE )


INCLUDE (${QT_USE_FILE})

SET ( UI_sources
	# all your .ui files
	../ui/mainwindow.ui
	../ui/childwindow.ui
	../ui/settings.ui
)
  	
QT4_WRAP_UI ( UI_headers ${UI_sources})


SET ( App_sources
	# all your .cpp files
	main.cpp

	MainWindow.cpp
	ChildWindow.cpp

	ColorClusterFinder.cpp
	ShootingAngleFinder.cpp
	ShootingSimulator.cpp
	SurfaceDetector.cpp
)

if ( UNIX )
	SET( App_sources ${App_sources} GameSniffer.cpp )
else()
	message( "-- building without GameSniffer" )
endif()

SET ( m_headers
	../include/MainWindow.h
	../include/ChildWindow.h
)

if ( UNIX )
	SET( m_headers ${m_headers} ../include/GameSniffer.h )
endif()


QT4_WRAP_CPP( MOC_headers ${m_headers} )



INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/include/Bullets
	${QT_INCLUDES}
	${CMAKE_CURRENT_BINARY_DIR}
)

if ( UNIX )
	INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/qpcap/qpcap )
endif()

ADD_EXECUTABLE ( ss2overlay
	${App_sources}
	${MOC_headers}
	${UI_headers}
)

if ( UNIX )
	LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/../qpcap/qpcap )
endif()


TARGET_LINK_LIBRARIES( ss2overlay ${QT_LIBRARIES} ) 

if ( UNIX )
	TARGET_LINK_LIBRARIES( ss2overlay qpcap )
	TARGET_LINK_LIBRARIES( ss2overlay Xext ) # for X11 access (click-through-window)
endif ( UNIX )
