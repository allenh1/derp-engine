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

#include "master_node.hpp"

#include <string>
#include <memory>
#include <utility>

#include "scope_exit.hpp"

/**
 * Construct the master node for the timefuse-server.
 *
 * @brief constructor
 *
 * @param _hostname Hostname for the master.
 * @param _port Port on which to construct the master.
 * @param _p_parent Parent QObject
 */
master_node::master_node(
  const QString & _hostname,
  const quint16 & _port,
  QObject * _p_parent)
: QObject(_p_parent),
  m_hostname(_hostname),
  m_port(_port)
{
  _to_browser = std::make_unique<QByteArray>();
  results = std::make_unique<BinarySearchDictionary>();
  _msg = std::make_unique<QString>();
}

master_node::~master_node()
{
}

/**
 * @brief Initialize the master node.
 *
 * This function does a number of things
 * that need to be done before using this
 * node. Namely, this function constructs
 * this thread, constructs the tcp_thread,
 * connects the handlers to tcp_thread's
 * signals, and, lastly, moves us onto
 * the thread we constructed.
 *
 * @return True upon successful initialization.
 */
bool master_node::init()
{
  std::cout << "Initializing master thread..." << std::endl;
  /* construct the tcp_thread */
  m_p_tcp_thread = std::make_unique<tcp_thread>(m_hostname, m_port);
  m_p_tcp_thread->init();

  m_db = setup_db();

  connect(
    m_p_tcp_thread.get(), &tcp_thread::got_search,
    this, &master_node::handle_search,
    Qt::DirectConnection);
  connect(
    m_p_tcp_thread.get(), &tcp_thread::got_home_page,
    this, &master_node::handle_home_page,
    Qt::DirectConnection);
  connect(
    this, &master_node::send_html,
    m_p_tcp_thread.get(), &tcp_thread::disconnect_client,
    Qt::DirectConnection);
  return true;
}


void master_node::handle_search(QTcpSocket * p_socket, QString * text)
{
  QString host{p_socket->peerName()};
  auto client{new tcp_connection(host, p_socket)};
  try {
    if (!search(*text)) {
      std::cerr << "No results found!" << std::endl;
      _msg = std::make_unique<QString>("ERROR: NO RESULTS FOUND\r\n");
    }
  } catch (...) {
    _msg = std::make_unique<QString>("ERROR: DB COMMUNICATION FAILED\r\n");
  }
  build_message(client);
}

void master_node::handle_home_page(QTcpSocket * p_socket)
{
  QString host = p_socket->peerName();
  auto client = new tcp_connection(host, p_socket);
  QString home = "src/derp-engine.html";
  QFile page(home);
  if (!page.open(QIODevice::ReadOnly)) {
    std::cerr << "*shrugs*" << std::endl;
  }
  QByteArray all = page.readAll();
  (*_to_browser) += all;
  build_message(client);
}

bool master_node::search(const QString & text)
{
  m_search = text;
  if (!m_db.open()) {
    std::cerr << "Error! Failed to open database connection!" << std::endl;
    return false;
  }
  QSqlQuery query{m_db};
  QString txt{
    "SELECT DISTINCT websites.url, websites.title, websites.content, "
    "website_keyword_relation.times_used FROM websites, keywords, "
    "website_keyword_relation WHERE websites.website_id="
    "website_keyword_relation.website_id AND "
    "website_keyword_relation.keyword_id=keywords.keyword_id AND ( "
  };
  QStringList words = text.split(' ');
  for (int i{0}; i < words.size(); ++i) {
    /* TODO(allenh1): what is going on in here? */
    words[i] = words[i].trimmed();
    if (words[i].size() == 0) {
      continue;
    }
    if (i != 0) {
      txt += " OR ";
    }
    txt += "keywords.keyword='" + words[i] + "' ";
  }
  txt += ") ORDER BY website_keyword_relation.times_used DESC";
  std::cerr << txt.toStdString() << std::endl;
  query.prepare(txt);

  if (!query.exec()) {
    std::cerr << "Query Failed to execute!" << std::endl;
    std::cerr << "query: " << query.lastQuery().toStdString() << std::endl;
    throw std::invalid_argument("failed to query the user's groups");
    return false;
  } else if (!query.size()) {
    *_msg += "\n";
    return false;
  }
  std::cerr << "end query" << std::endl;
  results = std::make_unique<BinarySearchDictionary>();
  for (; query.next(); ) {
    QString tmp = query.value(0).toString() + ":::" +
      query.value(1).toString() + ":::" +
      query.value(2).toString();
    int count = query.value(3).toInt();
    if (query.value(1).toString().contains(m_search,
      Qt::CaseInsensitive))
    {
      count += 100;
    }
    if (results->find(tmp) < 0) {
      results->add_record(tmp, count);
    } else {
      results->add_record(tmp, results->find(tmp) + count);
    }
  }
  results->sort();
  return true;
}

/**
 * Unter doesn't know how to spell worker. (Unter=Hunter)
 */
QSqlDatabase master_node::setup_db()
{
  const char * user, * pwd, * dbb, * host, * port_string;
  if ((user = getenv("DBUSR")) == nullptr) {
    perror("getenv");
    throw std::invalid_argument("getenv on user failed");
  } else if ((pwd = getenv("DBPASS")) == nullptr) {
    perror("getenv");
    throw std::invalid_argument("getenv on pwd failed");
  } else if ((dbb = getenv("DBNAME")) == nullptr) {
    perror("getenv");
    throw std::invalid_argument("getenv on db name failed");
  } else if ((host = getenv("DBHOST")) == nullptr) {
    perror("getenv");
    throw std::invalid_argument("getenv on db host failed");
  } else if ((port_string = getenv("DBPORT")) == nullptr) {
    perror("getenv");
    throw std::invalid_argument("getenv on db host failed");
  }

  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
  quint64 port = std::stoi(std::string(port_string));

  db.setHostName(host); db.setDatabaseName(dbb);
  db.setUserName(user); db.setPassword(pwd);
  db.setPort(port);
  return db;
}

void master_node::build_message(tcp_connection * p)
{
  std::cout << "building message" << std::endl;
  QString collect{QString(http) + sp + "200 OK" + sp + "Document" + sp + "follows" + crlf +
    server + sp + s_name + crlf + ctype + sp + "text/html" + crlf + crlf};
  QString c; c.setNum(results->size());
  QString htmlDoc{QString(htmlBegin) + "Derp-Engine Results: " + c + htmlEndHead + htmlLine};
  /* this returns the message, and will run whenever this scope is exited */
  auto at_exit{make_scope_exit([this, &collect, &p]() {
        _to_browser->clear();
        _msg->clear();
        QString * p_msg = new QString(collect);
        Q_EMIT (send_html(p, p_msg));
      })};

  if (_msg->contains("ERROR")) {
    htmlDoc += htmlEnd;
    collect += htmlDoc;
    return;
  } else if (results->size() > 0) {
    size_t n;
    std::cout << "results: " << results->size() << std::endl;
    auto res = std::move(results->keys(&n));
    for (size_t x{0}; x < n; ++x) {
      QStringList things = res[x].split(":::");
      if (things.size() < 3) {
        std::cerr << "ERROR: things.size() < 3" << std::endl;
        std::cerr << res[x].toStdString() << std::endl;
        continue;
      }
      htmlDoc += tableEntryHyperLink;
      htmlDoc += things.at(0);
      htmlDoc += tableEntryEndLink;
      things.replace(1, things.at(1).trimmed());
      if (things.at(1).size() > 0) {
        htmlDoc += things.at(1);
      } else {
        htmlDoc += m_search;
      }
      htmlDoc += tableEntryEndSummary;
      htmlDoc += things.at(2);
      htmlDoc += tableEntryEndText;
    }
    htmlDoc += htmlEnd;
    collect += htmlDoc;
  }
  collect += _to_browser->toStdString().c_str();
}
