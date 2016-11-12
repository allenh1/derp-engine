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
	ParseHTML(const QString & html);
	bool operator() ();

	const QQueue<QString>& getUrls();
	const QString& getContent();
	const QString& getHtml();
};

#endif
