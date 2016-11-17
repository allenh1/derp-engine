#include "master_node.hpp"

/** 
 * Construct the master node for the timefuse-server.
 * 
 * @brief constructor
 *
 * @param _hostname Hostname for the master.
 * @param _port Port on which to construct the master.
 * @param _p_parent Parent QObject
 */
master_node::master_node(const QString & _hostname,
						 const quint16 & _port,
						 QObject * _p_parent)
   : QObject(_p_parent),
	 m_hostname(_hostname),
	 m_port(_port)
{
	_to_browser = new QByteArray("");
	_msg = new QString("");
}

master_node::~master_node()
{
	
/**
	* @todo free resources and halt threads
	*/
}

/** 
 * @brief Initialize the master node.
 *
 * This function does a number of things
 * that need to be done before using this
 * node. Namely, this function constructs
 * this thread, constructs the tcp_thread,
 * connects the handlers to tcp_thread's
 * signals, and, lastly, moves us onto
 * the thread we constructed.
 *
 * @return True upon successful initialization.
 */
bool master_node::init()
{
   std::cout<<"Initializing master thread..."<<std::endl;
   /* construct the tcp_thread */
   m_p_tcp_thread = new tcp_thread(m_hostname, m_port);
   m_p_tcp_thread->init();

   m_db = setup_db();

   connect(m_p_tcp_thread, &tcp_thread::got_search,
		   this, &master_node::handle_search,
		   Qt::DirectConnection);
   connect(m_p_tcp_thread, &tcp_thread::got_home_page,
		   this, &master_node::handle_home_page,
		   Qt::DirectConnection);
   connect(this, &master_node::send_html,
		   m_p_tcp_thread, &tcp_thread::disconnect_client,
		   Qt::DirectConnection);
   return true;
}


void master_node::handle_search(QTcpSocket * p_socket, QString * text) {
	QString host = p_socket->peerName();
	tcp_connection * client = new tcp_connection(host, p_socket);
	QStringList words = text->split(' ');

	for(int i=0; i<words.size(); i++) {
		std::cerr<<"word: \""<<words[i].toStdString()<<"\""<<std::endl;
		words[i]=words[i].trimmed();
		if(words[i].size()==0) continue;
		try {
			if (!search(words[i])) {
				std::cerr<<"No results found!"<<std::endl;
				_msg = new QString("ERROR: NO RESULTS FOUND\r\n");
			}// else _msg = new QString("OK\r\n");
		} catch ( ... ) {
			_msg = new QString("ERROR: DB COMMUNICATION FAILED\r\n");		
		}
	} build_message(client);
}

void master_node::handle_home_page(QTcpSocket * p_socket) {
	QString host = p_socket->peerName();
	tcp_connection * client = new tcp_connection(host, p_socket);
	QString home = "src/derp-engine.html"; QFile page(home);
	if (!page.open(QIODevice::ReadOnly)) std::cerr<<"*shrugs*"<<std::endl;
	QByteArray all = page.readAll(); (*_to_browser)+=all;
	build_message(client);
}

bool master_node::search(QString text) {
	std::cerr<<"search function called on \""
			 <<text.toStdString()<<"\""<<std::endl;
	if(!m_db.open()) {
		std::cerr<<"Error! Failed to open database connection!"<<std::endl;
		return false;
	} 

	QSqlQuery query(m_db);
	QString txt = "SELECT DISTINCT websites.url, websites.title, websites.content, website_keyword_relation.times_used FROM websites, keywords, website_keyword_relation WHERE websites.website_id=website_keyword_relation.website_id AND website_keyword_relation.keyword_id=keywords.keyword_id AND keywords.keyword='";	
	txt += text + "' ORDER BY website_keyword_relation.times_used DESC";
	query.prepare(txt);
	
	if(!query.exec()) {
		std::cerr<<"Query Failed to execute!"<<std::endl;
		std::cerr<<"query: "<<query.lastQuery().toStdString()<<std::endl;	
		throw std::invalid_argument("failed to query the user's groups");
		return false;
	} else if (!query.size()) {
		*_msg += "\n";
		return false;
	}

	for (; query.next();) {
		*_msg += query.value(0).toString() + ":::"
			+ query.value(1).toString() + ":::" +
			query.value(2).toString() + ":::"+ query.value(3).toString() +";;;";
	}
	return true;
}

/**
 * Unter doesn't know how to spell worker. (Unter=Hunter)
 */
QSqlDatabase master_node::setup_db() {
	const char *user, *pwd, *dbb, *host, *port_string;
	if ((user = getenv("DBUSR")) == NULL) {
		perror("getenv");
		throw std::invalid_argument( "getenv on user failed" );
	} else if ((pwd = getenv("DBPASS")) == NULL) {
		perror("getenv");
		throw std::invalid_argument( "getenv on pwd failed" );
	} else if ((dbb = getenv("DBNAME")) == NULL) {
		perror("getenv");
		throw std::invalid_argument( "getenv on db name failed" );
	} else if ((host = getenv("DBHOST")) == NULL) {
		perror("getenv");
		throw std::invalid_argument( "getenv on db host failed" );
	} else if ((port_string = getenv("DBPORT")) == NULL) {
		perror("getenv");
		throw std::invalid_argument("getenv on db host failed");
	}

	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	quint64 port = std::stoi(std::string(port_string));
							
	db.setHostName(host); db.setDatabaseName(dbb);
	db.setUserName(user); db.setPassword(pwd);
	db.setPort(port);
	return db;
}

void master_node::build_message(tcp_connection * p) {
	std::cout<<"building message"<<std::endl;
	QString collect =  "";
	collect+=http; collect+=sp; collect+="200 OK"; collect+=sp;
	collect+="Document"; collect+=sp; collect+="follows"; collect+=crlf;
	collect+=server; collect+=sp; collect+=s_name; collect+=crlf;
	collect+=ctype; collect+=sp; collect+="text/html"; collect+=crlf;
	collect+=crlf;

	QString * htmlDoc = new QString();
	*htmlDoc+= QString(htmlBegin) + "Derp-Engine Results" + htmlEndTitle
		+ "Derp-Engine Results:" + htmlEndHead + htmlLine;

	//std::cout<<"msg: "<<_msg->toStdString()<<std::endl;

	if(_msg->contains("ERROR")) {
		*htmlDoc+=htmlEnd; collect+=*htmlDoc;
	} else if(_msg->size() > 1) {
		QStringList lines = _msg->split(";;;");
		for(int i=0; i<lines.size();i++) {
			//std::cout<<"line "<<i<<": "<<lines[i].toStdString()<<std::endl;
			QStringList things = lines[i].split(":::");
			if (things.size() < 3) continue;
			*htmlDoc+=tableEntryHyperLink; *htmlDoc+=things.at(0);
			*htmlDoc+=tableEntryEndLink;
			things.replace(1, things.at(1).trimmed());
			if(things.at(1).size()>0) *htmlDoc+=things.at(1);
			else *htmlDoc+="Search Result";
			*htmlDoc+=tableEntryEndSummary; *htmlDoc+=things.at(2);
			*htmlDoc+=tableEntryEndText;
		} *htmlDoc+=htmlEnd; collect+=*htmlDoc;
	} else collect+=_to_browser->toStdString().c_str();
	_to_browser->clear(); _msg->clear();
	QString * p_msg = new QString(collect);
	Q_EMIT(send_html(p, p_msg));
}
