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

#ifndef PARSEHTML_HPP_
#define PARSEHTML_HPP_

#include <QString>
#include <QRegExp>
#include <QQueue>
#include <QMap>

#include <memory>
#include <iostream>

class ParseHTML
{
public:
  explicit ParseHTML(const QString & _url, std::unique_ptr<QString> _html);
  virtual ~ParseHTML();
  bool operator()();

  const QMap<QString, int> & getKeywords();
  const QQueue<QString> & getUrls();
  const QString & getContent();
  const QString & getHtml();
  const QString & getTitle();

private:
  QString m_url;
  std::unique_ptr<QString> m_html;
  QString m_title;
  std::unique_ptr<QString> m_content;
  QQueue<QString> m_urls;
  QMap<QString, int> m_keywords;

  bool isUrl(QString _url);
  QString parseTag(QString _tag);
  void parseUrl(QString _url);
  void parseContent();
};
#endif  // PARSEHTML_HPP_
