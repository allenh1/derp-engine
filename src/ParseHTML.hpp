#ifndef __PARSEHTML_HPP__
#define __PARSEHTML_HPP__

#include <iostream>
#include <QString>
#include <QQueue>

class ParseHTML {
private:
	QString m_html;
	QString m_content;
	QQueue<QString> m_urls;
	
public:
	ParseHTML(QString & html);
	~ParseHTML();
	bool operator() ();

	QQueue<QString>& getUrls();
	QString& getContent();
	QString& getHtml();
};

#endif
