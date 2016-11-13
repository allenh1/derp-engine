QT += network sql core
CONFIG += c++14 debug

SOURCES += src/main.cpp \
           src/ParseHTML.cpp \
           src/crawler.cpp
           		   
HEADERS += src/ParseHTML.hpp \ 
           src/crawler.hpp

TARGET = derp-engine

