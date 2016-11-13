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
	QQueue<QString> urls; QSqlDatabase db = setup_db();
	urls.enqueue("http://allen-software.com");
	urls.enqueue("http://allen-software.com/bcc-doc");
	crawler buddy(urls, db);

	if (!buddy.init()) {
		std::cerr<<"Error: crawler failed to initialize!"<<std::endl;
		return 1;
	} return app.exec();
}
