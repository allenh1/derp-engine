#include "crawler.hpp"

crawler::crawler(const QStack<QString> & _unexplored,
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
	/* check for the url locally, first. */
	auto && x = m_local_url.find(url);
	if (x != m_local_url.end()) return true;
	
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
		m_local_url[url] = url;
		return true;
	} return query.size();
}

bool crawler::send_url_to_db(QString url, QString title, QString text)
{
	if (!m_db.open()) {
		std::cerr<<"Error! Failed to open database connection!"<<std::endl;
	    return false;
	} QSqlQuery query(m_db);

	query.prepare("INSERT INTO websites(url, title, content) VALUES(?, ?, ?)");
	query.bindValue(0, url); query.bindValue(1, title);
	query.bindValue(2, text);

	if (!query.exec()) {
		std::cerr<<"Error: Query failed to execute!"<<std::endl;
		std::cerr<<"Query: \""<<query.lastQuery().toStdString()<<"\""<<std::endl;
		return false;
	} return true;
}

bool crawler::add_keyword_to_db(QString url, QString word, int count)
{
	if (!m_db.open()) {
		std::cerr<<"Error! Failed to open database connection!"<<std::endl;
	    return false;
	} QSqlQuery query(m_db);

	query.prepare("CALL InsertKeyword(?, ?, ?, @success)");
	query.bindValue(0, url); query.bindValue(1, word);
	query.bindValue(2, count);

	if (!query.exec()) {
		std::cerr<<"Error: Query failed to execute!"<<std::endl;
		std::cerr<<"Query: \""<<query.lastQuery().toStdString()<<"\""<<std::endl;
		return false;
	} return true;
}

void crawler::run()
{
	std::cout<<"Crawling..."<<std::endl;
	/* run loop for the crawler */
	for (; m_continue && m_unexplored.size(); m_p_thread->msleep(50)) {
		std::cout<<std::endl<<"\t*** Looped around! ***"<<std::endl;
		/* remove the first unexplored url */		
		QString url = m_unexplored.pop();
		if (discovered(url)) continue;
		m_saving_file = true; m_local_url[url] = url;
		/* connect to the host */
		QNetworkAccessManager manager;
		QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
		QEventLoop event;
		connect(response,SIGNAL(finished()),&event,SLOT(quit()));
		event.exec();
		// Source should be stored here
		QString * html = new QString(response->readAll());
		QString read; std::cerr<<std::endl<<"\t******** In parse! ********"
							   <<std::endl<<std::endl;
		/* construct a parser */
		ParseHTML parser(url, html);
		delete html;
		/* call the parser */
		try {
			if (!parser()) {
				std::cerr<<"Failed to parse request!"<<std::endl;
				std::cerr<<"Request:"<<std::endl<<std::endl;
				std::cerr<<read.toStdString()<<std::endl;
				continue;
			}
		} catch ( ... ) {
			std::cerr<<"WARNING: exception was caught during parse!"<<std::endl;
		} if (parser.getContent().contains("404") ||
			parser.getContent().contains("301")) {
			std::cerr<<"Got 404. site: \""
					 <<url.toStdString()<<"\""<<std::endl;
			continue;
		}

		if (!send_url_to_db(url, parser.getTitle(),
							parser.getContent().left(500))) {
			std::cerr<<"DB communication failed!"<<std::endl;
		}
		
	    for (int x = 0; x < parser.getUrls().size(); ++x) {
			QString a = parser.getUrls()[x];
			/* if not seen, enqueue */			
			if (m_local_url.find(a) == m_local_url.end()) {
				if (a.contains("facebook.com")) {
					std::cout<<"URL was too bland (ty, Facebook)"<<std::endl;
					continue;
				} else if (a.contains("google.com")) {
					std::cout<<"URL was too lame (ty, Google)"<<std::endl;
					continue;
				}
				/* temporarily remove the http:// tag */
				QString prefix;
				if (a.contains("https://")) prefix = "https://", a.replace("https://", "");
				else if (a.contains("http://")) prefix = "http://", a.replace("http://", "");
				/* check for a self-link */
			    QStringList temp = a.split('/'); temp.removeDuplicates();
				QString toPush = prefix + temp[0];
				/* depth limit of 30 */
				if (temp.size() > 30) continue;
				for (int x = 1; x < temp.size(); toPush += "/" + temp[x++]);
				
				m_unexplored.push(toPush);
				std::cout<<"Discovered["<<m_unexplored.size() - 1
						 <<"]: \""<<toPush.toStdString()<<"\""<<std::endl;
			}
		}
		QMap<QString, int>::const_iterator x;
		for (x = parser.getKeywords().begin();
			 x != parser.getKeywords().end(); ++x) {
			add_keyword_to_db(url, x.key(), x.value());
		}

		std::cerr<<std::endl<<"\t******** Leaving Parse! ********"
				 <<std::endl<<std::endl;
	} std::cerr<<"Somehow we hit a wall? What?"<<std::endl;
}
