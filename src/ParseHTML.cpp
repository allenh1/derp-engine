#include "ParseHTML.hpp"

ParseHTML::ParseHTML(QString & html) {
	m_html+= html;
	m_content = "";
}

ParseHTML::~ParseHTML() {
	m_content.clear();
	m_html.clear();
	m_urls.clear();
}

bool ParseHTML::operator() () {
	enum {START, OPEN, HREF, URL, QUOTE, CLOSE} state;

	state = START; QString url = ""; QString href = "";

	for(int i=0; i<m_html.size(); i++) {
		switch(state) {
		case START:
			if(m_html[i] == '<') state = OPEN;
			break;
		case OPEN:
			if(m_html[i] == 'h') {
				href+=m_html[i];
				state = HREF;
			} else if(m_html[i]=='>') state = CLOSE;
			break;
		case HREF:
			if(m_html[i] == '"' && href.size() == 5) {
				href.clear();
				state = URL;
			} else if(href.size() > 5) {
				href.clear();
				state = OPEN;
			} else href+=m_html[i];
			break;
		case URL:
			if(m_html[i] == '"') state = QUOTE;
			else url+=m_html[i];
			break;
		case QUOTE:
			if(url.size()>0) {
				m_urls.push(url);
				url.clear();
			} if(m_html[i] == '>') state = CLOSE;
			break;
		case CLOSE:
			if(m_html[i] == '<') state = OPEN;
			else m_content+=m_html[i];
			break;
		} 
	}

	// returns false if no content is found
	if(content.size() > 0) return true;
	return false;
}


const QString& ParseHTML::getHtml() { return m_html; }
const QString& ParseHTML::getContent() {return m_content;}
const QQueue<QString>& ParserHTML::getUrls() {return m_urls;}
