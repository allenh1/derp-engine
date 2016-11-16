#include "ParseHTML.hpp"

ParseHTML::ParseHTML(const QString & _url, QString * _html) {
	m_url=_url;
	m_html=new QString(*_html);
	m_content=new QString("");
	m_title="";
}

bool ParseHTML::operator() () {
	enum {START, OPEN, CLOSE, TITLE, CONTENT} state;

	state = START; QString tag="";
	
	for(int i=0; i<m_html->size(); i++) {
		switch(state) {
		case START:
			if((*m_html)[i]=='<') state=OPEN;
			break;
		case OPEN:
			if((*m_html)[i]=='>') {
				tag=parseTag(tag);
				state=CLOSE;
			} else tag+=(*m_html)[i];
			break;
		case CLOSE:
			if((*m_html)[i]=='<') {
				state=OPEN;
				tag.clear();
			} else if(tag.contains("content")) {
				state=CONTENT;
				(*m_content)+=(*m_html)[i];
				tag.clear();
			} else if(tag.contains("title")) {
				state=TITLE;
				m_title+=(*m_html)[i];
				tag.clear();
			} break;
		case TITLE:
			if((*m_html)[i]=='<') state=OPEN;
			else m_title+=(*m_html)[i];
			break;
		case CONTENT:
			if((*m_html)[i]=='<') state=OPEN;
			else (*m_content)+=(*m_html)[i];
			break;
		} 
	}

	parseContent();
	std::cerr<<"content: "<<m_content->toStdString()<<std::endl;
	
	// returns false if no content is found
	if(m_content->size()==0 && m_urls.size()==0) return false;
	return true;
}

const QString& ParseHTML::getTitle() {return m_title;}
const QString& ParseHTML::getHtml() {return *m_html;}
const QString& ParseHTML::getContent() {return *m_content;}
const QQueue<QString>& ParseHTML::getUrls() {return m_urls;}
const QMap<QString, int> & ParseHTML::getKeywords() {return m_keywords;}

bool ParseHTML::isUrl(QString _url) {
	if (_url.contains("mailto:") || _url.contains(".a") || _url.contains(".cpio")
		|| _url.contains(".shar") || _url.contains(".LBR")
		|| _url.contains(".iso") || _url.contains(".lbr") || _url.contains(".mar")
		|| _url.contains(".bz2") || _url.contains(".F") || _url.contains(".gz")
		|| _url.contains(".lz") || _url.contains(".rz") || _url.contains(".sfark")
		|| _url.contains(".sz") || _url.contains(".?") || _url.contains(".x")
		|| _url.contains(".z") || _url.contains(".Z") || _url.contains(".t")
		|| _url.contains(".u") || _url.contains(".w")) {
		return false;
	} return true;
}

void ParseHTML::parseUrl(QString _url) {
	// call isUrl then parse if it is a url
	if(!isUrl(_url)) return; 
	if(!_url.contains("http://")&&!_url.contains("https://")) {
		if(_url[0]=='/' && m_url[m_url.size()-1]=='/') {
			_url.remove(0,1);
		}
		if((_url[0]=='/' && m_url[m_url.size()-1]!='/')
		   ||(_url[0]!='/' && m_url[m_url.size()-1]=='/')) _url=m_url+_url;
		else _url = m_url+'/'+_url;
	} m_urls.enqueue(_url);
}

QString ParseHTML::parseTag(QString _tag) {
	// call parseUrl() if href found
	int index = _tag.indexOf("href=");
	if(index != -1) {
		QString temp = "";
		for(int i=index+6; _tag[i]!='"';i++) temp+=_tag[i];
		parseUrl(temp);
	}
	
	// return "title" if title follows tag
	// if not title tag found return "title"
	// on first header found
	QString res;
	if(_tag.contains("title")) {
		res="title";
		return res;
	}
	if(!m_title.size()) {
		if(_tag.contains("h1") || _tag.contains("h2") || _tag.contains("h3")
		   || _tag.contains("h4") || _tag.contains("h5") || _tag.contains("h6")) {
			res = "title";
			return res;
		}
	}
	// return "content" if content follows tag
	if(_tag.indexOf("link")!=0 && _tag.indexOf("style")!=0
	   && _tag.indexOf("script")!=0) {
		res="content";
		return res;
	} res="skip";
	return res;
}

void ParseHTML::parseContent() {
	if(m_content->size()==0) return;

    m_content->replace(QRegExp("[\r\t\n\v\f\b\a]"), " "); 

	QString word = "";
	enum {WHITE, LETTER} state; state=LETTER;
	for(int i=0; i<m_content->size();i++) {
		switch(state) {
		case LETTER:
			if((*m_content)[i] == ' '){
				state=WHITE;
				if(word.size()==0) continue;
				if(!m_keywords.value(word)) m_keywords.insert(word, 1);
				else m_keywords.insert(word, m_keywords.value(word)+1);
				word.clear();
			} else word+=(*m_content)[i];
			break;
		case WHITE:
			if((*m_content)[i]!=' ') {
				state=LETTER;
				word+=(*m_content)[i];
			} else {
				m_content->remove(i,1);
				i--;
			} break;
		}
	} m_content->resize(m_content->size());
}
