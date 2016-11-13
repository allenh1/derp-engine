#ifndef __TCP_CONNECITON_HPP__
#define __TCP_CONNECTION_HPP__
#pragma once
#include <QtNetwork>
#include <QObject>
#include <QtCore>

#include <iostream>

class tcp_connection : public QObject
{
	Q_OBJECT
public:
	explicit tcp_connection(QString & _hostname,
							QTcpSocket * _p_socket,
							QObject * _p_parent = NULL
		);
	virtual ~tcp_connection();

	bool operator == (const tcp_connection & tcp1) {
		/* compare the socket */
		return (tcp1.m_p_socket->peerName() == m_p_socket->peerName()) &&
			(tcp1.m_p_socket->peerPort() == m_p_socket->peerPort());
	}
   
	Q_SLOT void disconnect(); /* @todo replace disconnect with a lambda */
   
	const QString & get_hostname() { return m_hostname; }
	const QTcpSocket * get_socket() { return m_p_socket; }  
private:
	QString m_hostname;
	QTcpSocket * m_p_socket;
};
#endif
