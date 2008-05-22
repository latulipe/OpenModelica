######################################################################
# Automatically generated by qmake (2.01a) to 26. apr 10:49:42 2007
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . \
               ../Pltpkg2

QT += network xml
CONFIG += warn_off

# Input
HEADERS += ../Pltpkg2/legendLabel.h \
	   ../Pltpkg2/compoundWidget.h \
	   ../Pltpkg2/dataSelect.h \
           ../Pltpkg2/graphWindow.h \
           ../Pltpkg2/curve.h \
	   ../Pltpkg2/point.h \ 
	   ../Pltpkg2/label.h \
	   ../Pltpkg2/focusRect.h \
	   ../Pltpkg2/graphScene.h \
	   ../Pltpkg2/graphWidget.h \
	   ../Pltpkg2/line2D.h \
	   ../Pltpkg2/lineGroup.h \
	   ../Pltpkg2/preferenceWindow.h \
	   ../Pltpkg2/variableData.h \
	   ../Pltpkg2/verticalLabel.h \
           ../Pltpkg2/variablewindow.h

SOURCES += e.cpp \
	   ../Pltpkg2/compoundWidget.cpp \
	   ../Pltpkg2/dataSelect.cpp \
           ../Pltpkg2/graphWindow.cpp \
           ../Pltpkg2/curve.cpp \
	   ../Pltpkg2/point.cpp \
	   ../Pltpkg2/legendLabel.cpp \
	   ../Pltpkg2/graphWidget.cpp \
	   ../Pltpkg2/line2D.cpp \
	   ../Pltpkg2/lineGroup.cpp \
	   ../Pltpkg2/preferenceWindow.cpp \
	   ../Pltpkg2/variableData.cpp \
           ../Pltpkg2/variablewindow.cpp

FORMS += ../Pltpkg2/compoundWidget.ui\
         ../Pltpkg2/dataSelect.ui\
         ../Pltpkg2/graphWindow.ui\
         ../Pltpkg2/preferences.ui \
         ../Pltpkg2/newgraph.ui
