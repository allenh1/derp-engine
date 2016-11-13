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
	if (!m_db.open()) {
		std::cerr<<"Error! Failed to open database connection!"<<std::endl;
		return true;
	} QSqlQuery query(m_db);
	QString query_string = "SELECT * FROM websites WHERE url = \"";
	query_string += url + "\"";
	query.prepare(query_string);

	if (!query.exec()) {
		std::cerr<<"Error: Query failed to execute!"<<std::endl;
		std::cerr<<"Query: \""<<query.lastQuery().toStdString()<<"\""<<std::endl;
		return true;
	} return query.size();
}

bool crawler::send_url_to_db(QString url)
{
	if (!m_db.open()) {
		std::cerr<<"Error! Failed to open database connection!"<<std::endl;
	    return false;
	} QSqlQuery query(m_db);

	query.prepare("INSERT INTO websites(url) VALUES(?)");
	query.bindValue(0, url);

	if (!query.exec()) {
		std::cerr<<"Error: Query failed to execute!"<<std::endl;
		std::cerr<<"Query: \""<<query.lastQuery().toStdString()<<"\""<<std::endl;
		return false;
	} return true;
} 

void crawler::parse()
{
	/* get the sender */
	FileDownloader * p_downloader = qobject_cast<FileDownloader*>(sender());
	QString read; std::cerr<<std::endl<<"\t******** In parse! ********"
						   <<std::endl<<std::endl;
	/* construct a parser */
	ParseHTML parser(p_downloader->get_url(),
					 read = QString(p_downloader->downloadedData()));
	/* call the parser */
	if (!parser()) {
		std::cerr<<"Failed to parse request!"<<std::endl;
		std::cerr<<"Request:"<<std::endl<<std::endl;
		std::cerr<<read.toStdString()<<std::endl;
	    return;
	}

	if (parser.getContent().contains("404: Page")) {
		std::cerr<<"Got 404. site: \""
				 <<p_downloader->get_url().toStdString()<<"\""<<std::endl;
	    return;
	}
	
    foreach (const QString & a, parser.getUrls()) {
		if (!discovered(a)) {
			/* if not seen, enqueue */
			m_unexplored.enqueue(a);
			if (!send_url_to_db(a)) {
				std::cerr<<"DB communication failed!"<<std::endl;
			}
			std::cout<<"Discovered["<<m_unexplored.size() - 1
					 <<"]: \""<<a.toStdString()<<"\""<<std::endl;
		}
	}

	std::cerr<<std::endl<<"\t******** Leaving Parse! ********"
			 <<std::endl<<std::endl;
	m_saving_file = false;
}

void crawler::run()
{
	std::cout<<"Crawling..."<<std::endl;
	/* run loop for the crawler */
	for (; m_continue && m_unexplored.size();) {
		std::cout<<std::endl<<"\t*** Looped around! ***"<<std::endl;
		/* remove the first unexplored url */
		QString url = m_unexplored.dequeue();
		m_saving_file = true;
		/* connect to the host */
		FileDownloader * p_fd = new FileDownloader(QUrl(url));

		connect(p_fd, &FileDownloader::downloaded, this, &crawler::parse,
				Qt::DirectConnection);

		//for (; m_saving_file; m_p_thread->msleep(100));
	} std::cerr<<"Somehow we hit a wall? What?"<<std::endl;
}
