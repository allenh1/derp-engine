#include "ParseHTML.hpp"

ParseHTML::ParseHTML(QString & html) {
	html = new QString(html);
	content = new QString();
	urls = new QStack<QString>();
}

ParseHTML::~ParseHTML() {
	delete content;
	delete html;
	delete urls;
}

bool ParseHTML::operator() () {
	enum {START, OPEN, HREF, URL, QUOTE, CLOSE} state;

	state = START; QString url = ""; QString href = "";

	for(int i=0; i<html->size(); i++) {
		switch(state) {
		case START:
			if((*html)[i] == '<') state = OPEN;
			break;
		case OPEN:
			if((*html)[i] == 'h') {
				href+=(*html)[i];
				state = HREF;
			} else if((*html)[i]=='>') state = CLOSE;
			break;
		case HREF:
			if((*html)[i] == '"' && href.size() == 5) {
				href.clear();
				state = URL;
			} else if(href.size() > 5) {
				href.clear();
				state = OPEN;
			} else href+=(*html)[i];
			break;
		case URL:
			if((*html)[i] == '"') state = QUOTE;
			else url+=(*html)[i]
			break;
		case QUOTE:
			if(url.size()>0) {
				urls->push(url);
				url.clear();
			} if((*html)[i] == '>') state = CLOSE;
			break;
		case CLOSE:
			if((*html)[i] == '<') state = OPEN;
			else (*content)+=(*html)[i];
			break;
		} 
	}
	
	// returns false if no content found
	return false;
}
