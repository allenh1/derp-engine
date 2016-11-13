#include <QCoreApplication>
#include "crawler.hpp"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
	QQueue<QString> urls;
	urls.enqueue("http://allen-software.com");

	crawler buddy(urls);
	if (!buddy.init()) {
		std::cerr<<"Error: crawler failed to initialize!"<<std::endl;
		return 1;
	} return app.exec();
}
