#include "ParseHTML.hpp"

ParseHTML::ParseHTML(const QString & _url, const QString & _html) {
	m_url=_url; m_html=_html; m_content = "";
}

bool ParseHTML::operator() () {
	enum {START, OPEN, ST, STYLE,
		  HREF, URL, QUOTE, CLOSE} state;

	state = START; QString url = ""; QString href = "";
	QString style = "";
	//std::cerr<<std::endl<<"html to parse: "<<std::endl;
	//std::cerr<<m_html.toStdString()<<std::endl;
	
	for(int i=0; i<m_html.size(); i++) {
		switch(state) {
		case START:
			if(m_html[i] == '<') state=OPEN;
			break;
		case OPEN:
			if(m_html[i] == 'h') {
				href+=m_html[i]; state=HREF;
			} else if(m_html[i]=='>') state=CLOSE;
			else if(m_html[i]=='s') {
				style+=m_html[i]; state=ST;
			} break;
		case HREF:
			if(m_html[i] == '"' && href.size() == 5) {
				href.clear(); state=URL;
			} else if(href.size() > 5) {
				href.clear(); state=OPEN;
			} else if(m_html[i]=='>') {
				href.clear(); state=CLOSE;
			} else href+=m_html[i];
			break;
		case ST:
			if(m_html[i]=='>' && style.contains("style")) {
				state=STYLE; style.clear();
			} else if(m_html[i]=='>') {
				state=START; style.clear();
			} else style+=m_html[i];
			break;
		case STYLE:
			if(m_html[i]=='>') state=START;
			break;
		case URL:
			if(m_html[i] == '"') state=QUOTE;
			else url+=m_html[i];
			break;
		case QUOTE:
			if(url.size()>0) {
				if(!url.contains("http://")) {
					if(url[0] == '/') url = m_url+url;
					else url = m_url+'/'+url;
				} if(!url.contains("mailto:")) {
					std::cerr<<"url: "<<url.toStdString()<<std::endl;
					m_urls.enqueue(url);
				} url.clear();
			} if(m_html[i] == '>') state = CLOSE;
			break;
		case CLOSE:
			if(m_html[i] == '<') state = OPEN;
			else m_content+=m_html[i];
			break;
		} 
	}

	parseContent();
	std::cerr<<"content: "<<m_content.toStdString()<<std::endl;
	
	// returns false if no content is found
	if(m_content.size()==0 && m_urls.size()==0) return false;
	return true;
}

const QString& ParseHTML::getHtml() {return m_html;}
const QString& ParseHTML::getContent() {return m_content;}
const QQueue<QString>& ParseHTML::getUrls() {return m_urls;}
const QMap<QString, int> & ParseHTML::getKeywords() {return m_keywords;}

void ParseHTML::parseContent() {
	if(m_content.size()==0) return;

    m_content.replace('\r', ""); m_content.replace('\n', ' ');
	m_content.replace('\t', ""); m_content.replace('\b', "");
	m_content.replace('\f', ""); m_content.replace('\a', "");
	m_content.replace('\v', "");

	QString word = "";
	enum {WHITE, LETTER} state; state=LETTER;
	for(int i=0; i<m_content.size();i++) {
		switch(state) {
		case LETTER:
			if(m_content[i] == ' '){
				state=WHITE;
				if(word.size()==0) continue;
				if(!m_keywords.value(word)) m_keywords.insert(word, 1);
				else m_keywords.insert(word, m_keywords.value(word)+1);
				word.clear();
			} else word+=m_content[i];
			break;
		case WHITE:
			if(m_content[i]!=' ') {
				state=LETTER;
				word+=m_content[i];
			} else {
				m_content.remove(i,1);
				i--;
			} break;
		}
	} 
}
