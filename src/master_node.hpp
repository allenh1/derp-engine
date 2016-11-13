#ifndef __MASTER_NODE_HPP__
#define __MASTER_NODE_HPP__
#pragma once

/* Qt Includes */
#include <QThread>
#include <QtCore>
#include <QStringList>
#include <QString>
#include <string>
#include <iostream>
#include <string.h>
#include <QSqlDatabase>
#include <QSqlQuery>

/* File Includes */
#include "tcp_thread.hpp"
/* #include "slave.hpp" */

class tcp_thread;

/**
 * TimeFuse Master class.
 *
 * This is the thread on which the
 * master node decides how to/if it
 * can conect a user to a worker node.
 *
 * @author Hunter Allen <allen286@purdue.edu>
 */
class master_node : public QObject
{
	Q_OBJECT
public:
	explicit master_node(const QString & _hostname,       /* master node's hostname */
						 const quint16 & _port,           /* master node's port num */
						 QObject * _p_parent = NULL       /* master node's parent   */
		);
   
	virtual ~master_node();

	bool init();

	Q_SIGNAL void send_info(tcp_connection * receiver);

	/* Callback functions for tcp connections */
	Q_SLOT void handle_search(QTcpSocket * p_socket, QString * text);
	Q_SLOT void handle_home_page(QTcpSocket * p_socket);
	Q_SLOT bool search(QString text);

	Q_SLOT QSqlDatabase setup_db();
	
	Q_SLOT void stop() { m_continue = false; }

	Q_SIGNAL void send_html(tcp_connection *, QString *);
private:
	volatile bool m_continue = true;

	QString * _msg;
	QByteArray * _to_browser;
	
	QString m_hostname;
	quint16 m_port;
	QSqlDatabase m_db;

	tcp_thread * m_p_tcp_thread;

	/* code that writes an html page to browser a directory  */
	const char * htmlBegin = "<!DOCTYPE html><head><title>";
	const char * htmlEndTitle = "</title></head><body><h1>";
	const char * htmlEndHead = "</h1><table><tbody>";
	const char * htmlLine = "<tr><th colspan=\"5\"><hr></th></tr>";
	const char * tableEntryHyperLink = "<tr><td><details><summary><a  href=\"";
	const char * tableEntryEndLink = "\">";
	const char * tableEntryEndSummary = "</a></summary>";
	const char * tableEntryEndText = "</td></tr>";
	const char * htmlEnd = "</tbody></table></body></html>";

// basic header strings
	const char * sp = " ";
	const char * crlf = "\r\n";
	const char * crlf2 = "\n\n";
	const char * http = "HTTP/1.0";
	const char * server = "Server:";
	const char * s_name = "myhttp";
	const char * ctype = "Content-type:";

	void build_message(tcp_connection *);
};
#endif

