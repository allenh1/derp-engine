#include "crawler.hpp"

crawler::crawler(const QQueue<QString> & _unexplored,
				 const QSqlDatabase & _db,				 
				 QObject * _p_parent)
	: QObject(_p_parent),	  
	  m_unexplored(_unexplored),
	  m_db(_db)
{ m_p_thread = new QThread(); }

crawler::~crawler()
{
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

bool crawler::send_url_to_db(QString * url)
{
	if (!m_db.open()) {
		std::cerr<<"Error! Failed to open database connection!"<<std::endl;
		delete url; return false;
	} QSqlQuery query(m_db);

	query.prepare("INSERT INTO websites(url) VALUES(?)");
	query.bindValue(0, *url);

	if (!query.exec()) {
		std::cerr<<"Error: Query failed to execute!"<<std::endl;
		std::cerr<<"Query: \""<<query.lastQuery().toStdString()<<"\""<<std::endl;
		delete url;
		return false;
	} delete url; return true;
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
			/* @todo */
		} else if (url.contains("http://")) {
		    QString host = url; host.replace("http://", "");
			QStringList split = host.split('/');
			host = split[0]; /* up to the first '/' */
			QString to_grab("/");
			for (int x = 1; x < split.size(); ++x) {
				to_grab += split[x];
				if (split[x].indexOf('.') == -1) to_grab += "/";
			} if (split.size() > 1 && split[split.size() - 1].indexOf('.') == -1) {
				to_grab += "index.html";
			}
			
			/* find the first '/' */
			std::cout<<"Connecting to host: \""<<host.toStdString()<<"\""<<std::endl;
			p_socket->connectToHost(host, http_port,
									QIODevice::ReadWrite);
			if (!p_socket->isOpen()) {
				std::cerr<<"WARNING: failed to open socket!"<<std::endl;
			}
			QString get = "GET "; get += to_grab + " HTTP/1.0\r\n\r\n";
			get += '\0'; /* just to be extra sure */
			std::cout<<std::endl<<"\t"<<get.toStdString()<<std::endl;
			p_socket->write(get.toStdString().c_str());
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
		QString * p_url = new QString(url);
	    send_url_to_db(p_url);
	}
}
