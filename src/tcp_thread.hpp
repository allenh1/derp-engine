#ifndef __TCP_THREAD_HPP__
#define __TCP_THREAD_HPP__
#include <iostream>

#include <QDataStream>
#include <QtNetwork>
#include <QtCore>
#include <QDebug>

class tcp_thread : public QObject
{
	Q_OBJECT
public:
	explicit tcp_thread(const QString & _hostname, const quint16 & _port, const bool & _master_mode = true, QObject * parent = NULL);
	~tcp_thread() { /** @todo This function is important, I suppose... **/ }

	bool init();
	bool writeData(QByteArray data, tcp_connection * receiver);
   
	Q_SLOT void disconnected();
	Q_SLOT void readFromClient();
	Q_SLOT void sendMessage(QString, tcp_connection * request);
	Q_SLOT void acceptConnection();
	Q_SLOT void stop(){ m_continue = false; }
	Q_SLOT void disconnect_client(tcp_connection *, QString *);

	Q_SIGNAL void readIt(QTcpSocket*);
	Q_SIGNAL void receivedMessage();
	Q_SIGNAL void dropped_connection(tcp_connection *);
	
	Q_SLOT void echoReceived(QString);

	const QTcpServer * getServer() { return m_pServer; }
private:
	QTcpServer * m_pServer;

	volatile bool m_continue = true;
   
	QString m_hostname;
	quint16 m_port;
	quint16 m_blockSize;

	QQueue<tcp_connection> * m_pTcpMessages;
	QList<tcp_connection*> m_tcp_connections;
};
#endif
