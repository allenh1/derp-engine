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

#ifndef TCP_THREAD_HPP_
#define TCP_THREAD_HPP_
#include "tcp_connection.hpp"

#include <QDataStream>
#include <QtNetwork>
#include <QtCore>
#include <QDebug>

#include <iostream>

class tcp_thread : public QObject
{
  Q_OBJECT

public:
  explicit tcp_thread(
    const QString & _hostname, const quint16 & _port,
    const bool & _master_mode = true, QObject * parent = nullptr);
  ~tcp_thread() { /** @todo This function is important, I suppose... **/}

  bool init();
  bool writeData(QByteArray data, tcp_connection * receiver);

  Q_SLOT void disconnected();
  Q_SLOT void readFromClient();
  Q_SLOT void sendMessage(QString, tcp_connection * request);
  Q_SLOT void acceptConnection();
  Q_SLOT void stop() {m_continue = false;}
  Q_SLOT void disconnect_client(tcp_connection *, QString *);

  Q_SIGNAL void readIt(QTcpSocket *);
  Q_SIGNAL void receivedMessage();
  Q_SIGNAL void dropped_connection(tcp_connection *);
  Q_SIGNAL void got_home_page(QTcpSocket *);
  Q_SIGNAL void got_search(QTcpSocket *, QString *);

  Q_SLOT void echoReceived(QString);

  const QTcpServer * getServer() {return m_pServer;}

private:
  QTcpServer * m_pServer;

  volatile bool m_continue = true;

  QString m_hostname;
  quint16 m_port;
  quint16 m_blockSize;
  bool m_master_mode;

  QQueue<tcp_connection> * m_pTcpMessages;
  QList<tcp_connection *> m_tcp_connections;
};
#endif  // TCP_THREAD_HPP_
