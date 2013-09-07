FIND_PACKAGE (Qt4 REQUIRED)

ADD_DEFINITIONS (${QT_DEFINITIONS})

#SET( QT_USE_QTXML TRUE )
#SET( QT_USE_QTOPENGL TRUE )
SET( QT_WRAP_CPP TRUE )

INCLUDE (${QT_USE_FILE})

SET ( UI_sources
	# all your .ui files
	../ui/mainwindow.ui
)
  	
QT4_WRAP_UI ( UI_headers ${UI_sources})


SET ( App_sources
	# all your .cpp files
	main.cpp

	MainWindow.cpp

	ColorClusterFinder.cpp
	ShootingAngleFinder.cpp
)

QT4_WRAP_CPP ( MOC_headers
	# all header files that need to be moc'ed
	# (the ones using Q_OBJECT macro)

	../include/MainWindow.h
)


INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/include ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} )

ADD_EXECUTABLE ( ss2overlay
	${App_sources}
	${MOC_headers}
	${UI_headers}
)

TARGET_LINK_LIBRARIES( ss2overlay ${QT_LIBRARIES} )

if ( UNIX )
		TARGET_LINK_LIBRARIES( ss2overlay Xext ) # for X11 access (click-through-window)
endif ( UNIX )
