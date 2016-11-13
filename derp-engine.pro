QT += network sql core
CONFIG += c++14 debug

SOURCES += src/main.cpp \
           src/ParseHTML.cpp \
           src/crawler.cpp \
           src/file_downloader.cpp
           		   
HEADERS += src/ParseHTML.hpp \ 
           src/crawler.hpp \
           src/file_downloader.hpp

TARGET = derp-engine

