// Copyright 2018 Hunter L. Allen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ParseHTML.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>

#include "scope_exit.hpp"

ParseHTML::ParseHTML(const QString & _url, std::unique_ptr<QString> _html)
{
  m_html = std::move(_html);
  m_content = std::make_unique<QString>();
  m_keywords = std::make_unique<std::unordered_map<QString, int>>();
  m_url = _url;
}

ParseHTML::~ParseHTML()
{
  // NOP
}

bool ParseHTML::operator()()
{
  enum {START, OPEN, CLOSE, TITLE, CONTENT} state;

  state = START; QString tag = ""; QString type = "";

  int max;
  if (m_html->size() > 2 << 21) {
    max = 2 << 21;
  } else {
    max = m_html->size();
  }
  std::cout << "parse: " << m_url.toStdString() << std::endl;
  for (int i = 0; i < max; i++) {
    switch (state) {
      case START:
        if ((*m_html)[i] == '<') {state = OPEN;}
        break;
      case OPEN:
        if ((*m_html)[i] == '>') {
          type = parseTag(tag);
          tag.clear();
          state = CLOSE;
        } else {tag += (*m_html)[i];}
        break;
      case CLOSE:
        if ((*m_html)[i] == '<' && (*m_html)[i - 1] != '\'' &&
          (*m_html)[i - 1] != '"')
        {
          state = OPEN;
          type.clear();
        } else if (type.contains("content")) {
          state = CONTENT;
          (*m_content) += ' ';
          (*m_content) += (*m_html)[i];
          type.clear();
        } else if (type.contains("title")) {
          state = TITLE;
          m_title += (*m_html)[i];
          type.clear();
        }
        break;
      case TITLE:
        if ((*m_html)[i] == '<') {
          state = OPEN;
        } else if ((*m_html)[i] == '>') {
          state = START;
          m_title.clear();
        } else {
          m_title += (*m_html)[i];
        }
        break;
      case CONTENT:
        if ((*m_html)[i] == '<') {
          state = OPEN;
        } else {
          (*m_content) += (*m_html)[i];
        }
        break;
    }
  }
  std::cerr << "end parsing" << std::endl;

  parseContent();
  // returns false if no content is found
  return !(m_content->size() == 0 && m_urls.size() == 0);
}

const QString & ParseHTML::getTitle() {return m_title;}
const std::unique_ptr<QString> & ParseHTML::getHtml() {return m_html;}
std::unique_ptr<QString> ParseHTML::getContent() {return std::move(m_content);}
const QQueue<QString> & ParseHTML::getUrls() {return m_urls;}
std::unique_ptr<std::unordered_map<QString, int>> ParseHTML::getKeywords()
{
  return std::move(m_keywords);
}

bool ParseHTML::isUrl(const QString & _url) const
{
  return !(_url.contains("mailto:") || _url.contains(".a") || _url.contains(".cpio") ||
         _url.contains(".shar") || _url.contains(".LBR") ||
         _url.contains(".iso") || _url.contains(".lbr") || _url.contains(".mar") ||
         _url.contains(".bz2") || _url.contains(".F") || _url.contains(".gz") ||
         _url.contains(".lz") || _url.contains(".rz") || _url.contains(".sfark") ||
         _url.contains(".sz") || _url.contains(".?") || _url.contains(".x") ||
         _url.contains(".z") || _url.contains(".Z") || _url.contains(".t") ||
         _url.contains(".u") || _url.contains(".w"));
}

void ParseHTML::parseUrl(QString _url)
{
  // call isUrl then parse if it is a url
  if (!isUrl(_url)) {
    return;
  } else if (m_url.contains(_url)) {
    return;
  }
  auto enqueue_at_exit = make_scope_exit([this, &_url]() {
        m_urls.enqueue(std::move(_url));
      });
  if (_url.contains("http://") || _url.contains("https://")) {
    return;  // _url is still enqueued
  }
  if (_url[0] == '/' && m_url[m_url.size() - 1] == '/') {
    _url.remove(0, 1);
  }
  if ((_url[0] == '/' && m_url[m_url.size() - 1] != '/') ||
    (_url[0] != '/' && m_url[m_url.size() - 1] == '/'))
  {
    _url = m_url + _url;
  } else {
    _url = m_url + '/' + _url;
  }
}

QString ParseHTML::parseTag(QString _tag)
{
  // call parseUrl() if href found
  const int index = _tag.indexOf("href=");
  if (index != -1) {
    QString temp;
    for (int i = index + 6; _tag[i] != '"' &&
      _tag[i] != '\'' &&
      (i - (index + 6) < 500); i++)
    {
      temp += _tag[i];
    }
    parseUrl(std::move(temp));
  }

  // return "title" if title follows tag
  // if not title tag found return "title"
  // on first header found
  QString res;
  if (_tag.indexOf("title") == 0 && m_title.size() == 0) {
    std::cerr << "found title: " << _tag.toStdString() << std::endl;
    res = "title";
    return res;
  }
  if (m_title.size() == 0) {
    if (_tag.contains("h1") || _tag.contains("h2") || _tag.contains("h3") ||
      _tag.contains("h4") || _tag.contains("h5") || _tag.contains("h6"))
    {
      std::cerr << "found title: " << _tag.toStdString() << std::endl;
      res = "title";
      return res;
    }
  }
  // return "content" if content follows tag
  res = "skip";
  const bool tag_check =
    _tag.indexOf("body") == 0 ||
    _tag.indexOf("div") == 0 ||
    _tag.indexOf("p") == 0 ||
    _tag.indexOf("a") == 0 ||
    _tag.indexOf("th") == 0 ||
    _tag.indexOf("tr") == 0 ||
    _tag.indexOf("h") == 0 ||
    _tag.indexOf("ul") == 0 ||
    _tag.indexOf("li") == 0 ||
    _tag.indexOf("i") == 0 ||
    _tag.indexOf("option") == 0 ||
    _tag.indexOf("span") == 0 ||
    _tag.indexOf("string") == 0 ||
    _tag.indexOf("img") == 0;
  if (tag_check) {
    res = "content";
  }
  return res;
}

void ParseHTML::parseContent()
{
  std::cout << "parse content" << std::endl;
  if (m_content->size() == 0) {
    return;
  }
  m_content = std::make_unique<QString>(m_content->toLatin1());
  m_content->replace(QRegExp("[\r\t\n\v\f\b\a]"), " ");
  m_content = std::make_unique<QString>(m_content->toLatin1());
  m_content->replace('?', ' ');
  m_content = std::make_unique<QString>(m_content->toLatin1());
  m_content->replace(",", ""); m_content->replace(".", "");
  m_content->replace("(", ""); m_content->replace(")", "");
  m_content->replace("{", ""); m_content->replace("}", "");
  m_content->replace("!", ""); m_content->replace("+", "");
  m_content->replace("=", ""); m_content->replace("#", "");
  m_content->replace("$", ""); m_content->replace("!", "");
  m_content->replace("/", ""); m_content->replace('"', "");
  m_content->replace(":", ""); m_content->replace(";", "");
  m_content->replace(">", ""); m_content->replace("<", "");
  m_content->replace("?", ""); m_content->replace("*", "");
  m_content->replace("&", ""); m_content->replace("^", "");
  m_content->replace("`", ""); m_content->replace("~", "");
  m_content->replace("]", ""); m_content->replace("[", "");
  m_content->replace("_", ""); m_content->replace("|", "");

  auto word{std::make_unique<QString>()};
  enum {WHITE, LETTER} state; state = LETTER;
  for (int i = 0; i < m_content->size(); i++) {
    switch (state) {
      case LETTER:
        if ((*m_content)[i] == ' ') {
          state = WHITE;
          *word = word->trimmed();
          if (word->size() == 0) {
            continue;
          }
          const auto & it = m_keywords->find(*word);
          if (it == m_keywords->end()) {
            m_keywords->insert({*word, 1});
          } else {
            m_keywords->insert({*word, it->second + 1});
          }
          word = std::make_unique<QString>();
        } else {
          *word += (*m_content)[i];
        }
        break;
      case WHITE:
        if ((*m_content)[i] != ' ') {
          state = LETTER;
          *word += (*m_content)[i];
        } else {
          m_content->remove(i, 1);
          i--;
        }
        break;
    }
  }
  m_content = std::make_unique<QString>(m_content->toUtf8());
  m_content->replace("?", "");
  (*m_content) = m_content->trimmed();
  m_content->resize(m_content->size());

  m_title = m_title.trimmed();
  size_t size = m_title.size();
  if (size > 100) {
    size = 100;
  }
  m_title.resize(size);
  std::cerr << "end parse content" << std::endl;
}
