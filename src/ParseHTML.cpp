#include "ParseHTML.hpp"

ParseHTML::ParseHTML(const QString & _html) {
	m_html=_html;
	m_content = "";
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
				m_urls.push_back(url);
				url.clear();
			} if(m_html[i] == '>') state = CLOSE;
			break;
		case CLOSE:
			if(m_html[i] == '<') state = OPEN;
			else m_content+=m_html[i];
			break;
		} 
	}

	parseContent();
	
	// returns false if no content is found
	if(m_content.size() > 0) return true;
	return false;
}

const QString& ParseHTML::getHtml() {return m_html;}
const QString& ParseHTML::getContent() {return m_content;}
const QQueue<QString>& ParseHTML::getUrls() {return m_urls;}
const QMap<QString, int> & ParseHTML::getKeywords() {return m_keywords;}

void ParseHTML::parseContent() {
	if(m_content.size()==0) return;

    m_content.replace('\r', ' '); m_content.replace('\n', ' ');
	m_content.replace('\t', ' '); m_content.replace('\b', ' ');
	m_content.replace('\f', ' '); m_content.replace('\a', ' ');
	m_content.replace('\v', ' ');

	QString word = "";
	for(int i=0; i<m_content.size();i++) {
		if(m_content[i] == ' '){
			if(!m_keywords.value(word)) m_keywords.insert(word, 1);
			else m_keywords.insert(word, m_keywords.value(word)+1);
			word.clear();
		} else word+=m_content[i];
	} 
}
