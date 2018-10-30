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
#ifndef CRAWLER_HPP_
#define CRAWLER_HPP_

#include <QtNetwork>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QtCore>

#include <unordered_map>
#include <memory>

#include "file_downloader.hpp"
#include "ParseHTML.hpp"
#include "QString_hash.hpp"

class crawler : public QObject
{
public:
  explicit crawler(
    const QStack<QString> & _unexplored,
    const QSqlDatabase & _db,
    QObject * _p_parent = nullptr);
  virtual ~crawler();

  bool init();
  bool discovered(const QString & url);
  bool check_content(const QString & url, const QString & content);
  const volatile bool & is_running() {return m_continue;}
  const std::unique_ptr<QThread> & get_thread() {return m_p_thread;}
  const QStack<QString> & get_unexplored() {return m_unexplored;}

  Q_SLOT void run();
  Q_SLOT void stop();
  Q_SLOT void parse();
  Q_SLOT bool send_url_to_db(QString, QString, QString);
  Q_SLOT bool add_keyword_to_db(QString, QString, int);

  Q_SIGNAL void got_url(QString *);
  Q_SIGNAL void no_more_urls();

private:
  const quint16 http_port = 80;
  std::unique_ptr<QThread> m_p_thread;
  volatile bool m_continue = true;
  volatile bool m_saving_file = true;
  std::unordered_map<QString, QString> m_local_url;
  QStack<QString> m_unexplored;
  QSqlDatabase m_db;
};
#endif  // CRAWLER_HPP_
