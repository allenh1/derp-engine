#include <QCoreApplication>
#include "crawler.hpp"
#include "master_node.hpp"

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
	if (argc == 3 && !strcmp(argv[1], "--crawl")) {
		
		QQueue<QString> urls; QSqlDatabase db = setup_db();
		urls.enqueue(QString(argv[2]));
		crawler buddy(urls, db); buddy.init();
		return app.exec();
	} else {
		master_node node("localhost", 2016);
		node.init();
		return app.exec();
	}
}
