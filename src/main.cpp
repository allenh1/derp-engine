#include <QCoreApplication>
#include "crawler.hpp"

QSqlDatabase setup_db()
{
	std::cout<<"Setting up database..."<<std::endl;
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
	db.setPort(port); std::cout<<"database connection established"<<std::endl;
	return db;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
	QQueue<QString> url1; QSqlDatabase db = setup_db();
	QQueue<QString> url2;
    //	url1.enqueue("http://www.purdue.edu");
	// url1.enqueue("http://allen-software.com");
	// url1.enqueue("http://www.mysql.com");
	// url1.enqueue("http://netflix.com");
	// url1.enqueue("http://wiki.ros.org");
	// url1.enqueue("http://vanderbilt.edu");
	// url1.enqueue("http://www.52tapes.com");
	// url1.enqueue("http://www.junk-culture.com");
	// url1.enqueue("http://minutelabs.io");
	// url1.enqueue("http://browsehappy.com");
	url1.enqueue("http://www.purdue.edu");
	url1.enqueue("https://www.elegantthemes.com");
	url1.enqueue("https://www.reddit.com");
	
	crawler buddy(url1, db);
	
	if (!buddy.init()) {
		std::cerr<<"Error: crawler failed to initialize!"<<std::endl;
		return 1;
	} return app.exec();
}
