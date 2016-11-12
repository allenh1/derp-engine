#ifndef __PARSEHTML_HPP__
#define __PARSEHTML_HPP__

#include <iostream>
#include <QString>
#include <QStack>

class ParseHTML {
private:
	QString * html;
	
public:
	QString * content;
	QStack<QString> * urls;
	
	ParseHTML(QString & html);
	~ParseHTML();
	bool operator() ();
};

#endif
