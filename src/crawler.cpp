#include "crawler.hpp"

crawler::crawler(const QQueue<QString> & _unexplored,
				 QObject * _p_parent)
	: QObject(_p_parent),
	  m_unexplored(_unexplored)
{ m_p_thread = new QThread(); }

bool crawler::init()
{
	/* move onto the thread */
	this->moveToThread(m_p_thread);
	/* connect thread's start to run() */
	connect(m_p_thread, &QThread::started,
			this, &crawler::run);
	/* start the thread */
	m_p_thread->start();
	/* return true if the thread started */
	return m_p_thread.isRunning();
}

void crawler::run()
{
	/* run loop for the crawler */
	for (; m_continue && m_unexplored.size(); m_sleep(300)) {
		/* construct a tcp_socket */
		QTcpSocket * p_socket = new QTcpSocket();
		/* remove the first unexplored url */
		QString url = m_unexplored.dequeue();
		/* this will hold the fetched HTML */
		QString read = "";
		/* connect to the host */
		if (url.contains("https://")) {
			/* open on port 443 */
			/**
			 * @todo figure out how https works...
			 */
		} else if (url.contains("http://")) {
			/* open on port 80 */
			p_socket->connectToHost(url, http_port,
									QIODevice::ReadWrite);
			p_socket->write("GET / HTTP/1.0\r\n\r\n");

			/* wait for our request to finish */
			p_socket->waitForBytesWritten(-1);
			/* wait to be able to read */
			p_socket->waitForReadyRead();
			/* read the text */
			for (; p_socket->canReadLine(); read += p_socket->readLine());
		} else { delete p_socket; continue; }

		/* invoke a parser */
		parseHTML * p_parser = new parseHTML();
	}
}
