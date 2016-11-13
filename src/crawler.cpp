#include "crawler.hpp"

crawler::crawler(const QQueue<QString> & _unexplored,
				 QObject * _p_parent)
	: QObject(_p_parent),
	  m_unexplored(_unexplored)
{ m_p_thread = new QThread(); }

crawler::~crawler()
{
	m_continue = false;
	if (m_p_thread->isRunning()) m_p_thread->wait();
	delete m_p_thread;
}

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
	return m_p_thread->isRunning();
}

bool crawler::discovered(const QString & url)
{
	/**
	 * @todo search through our list,
	 * then ask the database.
	 *
	 * This is O(log n + n) = O(n)
	 */
	return false;
}

void crawler::run()
{
	std::cout<<"Crawling..."<<std::endl;
	/* run loop for the crawler */
	for (; m_continue && m_unexplored.size(); m_p_thread->msleep(300)) {
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
		    QString host = url.replace("http://", "");
			/* find the first '/' */
			std::cout<<"Connecting to host: \""<<host<<"\""<<std::endl;
			p_socket->connectToHost(host, http_port,
									QIODevice::ReadWrite);
			if (!p_socket->isOpen()) {
				std::cerr<<"WARNING: failed to open socket!"<<std::endl;
			} p_socket->write("GET / HTTP/1.0\r\n\r\n");
			/* wait for our request to finish */
			p_socket->waitForBytesWritten(-1);
			/* wait to be able to read */
			p_socket->waitForReadyRead();
			/* read the text */
			for (; p_socket->canReadLine();) read += p_socket->readLine();
		} else { delete p_socket; continue; }

		/* construct a parser */
		ParseHTML parser(url, read);
		/* call the parser */
		if (!parser()) {
			std::cerr<<"Failed to parse request!"<<std::endl;
			std::cerr<<"Request:"<<std::endl<<std::endl;
			std::cerr<<read.toStdString()<<std::endl;
			delete p_socket; continue;
		}

		foreach (const QString & a, parser.getUrls()) {
			if (!discovered(a)) {
				/* if not seen, enqueue */
				m_unexplored.enqueue(a);
				std::cout<<"Discovered: \""<<a.toStdString()<<"\""<<std::endl;
			}
		}
	} m_p_thread->wait();
}
