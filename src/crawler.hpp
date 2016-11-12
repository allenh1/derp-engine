#ifndef __CRAWLER_HPP__
#define __CRAWLER_HPP__
#include <QtNetwork>
#include <iostream>
#include <QtCore>
#include "parseHTML.hpp"

class crawler : public QObject
{
public:
	explicit crawler(const QQueue<QString> & _unexplored,
					 QObject * _p_parent = NULL);
	virtual ~crawler();

	bool init();

	const bool & is_running() { return m_continue; }
	const QThread * get_thread() { return m_p_thread; }
	const QQueue & get_unexplored() { return m_unexplored; }

	Q_SLOT void run();
	Q_SLOT void stop();

	Q_SIGNAL void got_url(QString *);
private:
	QThread * m_p_thread;
    volatile bool m_continue = true;

	QQueue<QString> m_unexplored;
};
#endif
