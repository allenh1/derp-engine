#ifndef __CRAWLER_HPP__
#define __CRAWLER_HPP__
#include <QtNetwork>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QtCore>
#include <map>
#include "file_downloader.hpp"
#include "ParseHTML.hpp"

class crawler : public QObject
{
public:
	explicit crawler(const QStack<QString> & _unexplored,
					 const QSqlDatabase & _db,
					 QObject * _p_parent = NULL);
	virtual ~crawler();

	bool init();
	bool discovered(const QString & url);
	bool check_content(const QString & url, const QString & content);
	const volatile bool & is_running() { return m_continue; }
	const QThread * get_thread() { return m_p_thread; }
	const QStack<QString> & get_unexplored() { return m_unexplored; }

	Q_SLOT void run();
	Q_SLOT void stop();
	Q_SLOT void parse();
	Q_SLOT bool send_url_to_db(QString, QString, QString);
	Q_SLOT bool add_keyword_to_db(QString, QString, int);

	Q_SIGNAL void got_url(QString *);
	Q_SIGNAL void no_more_urls();
private:
	const quint16 http_port = 80;
	QThread * m_p_thread;
    volatile bool m_continue = true;
	volatile bool m_saving_file = true;
	std::map<QString, QString> m_local_url;
	QStack<QString> m_unexplored;
	QSqlDatabase m_db;
};
#endif
