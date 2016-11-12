#include "ParseHTML.hpp"

ParseHTML::ParseHTML(QString & html) {
	html = new QString(html);
	content = new QString();
	urls = new QStack<QString>();
}

ParseHTML::~ParseHTML() {
	delete content; 
}

bool ParseHTML::operator() () {
	return false;
}
