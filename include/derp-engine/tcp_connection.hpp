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
#ifndef DERP_ENGINE__TCP_CONNECTION_HPP_
#define DERP_ENGINE__TCP_CONNECTION_HPP_

#include <QtNetwork>
#include <QObject>
#include <QtCore>

#include <iostream>

class tcp_connection : public QObject
{
  Q_OBJECT

public:
  explicit tcp_connection(
    QString & _hostname,
    QTcpSocket * _p_socket,
    QObject * _p_parent = nullptr
  );
  virtual ~tcp_connection();

  bool operator==(const tcp_connection & tcp1)
  {
    /* compare the socket */
    return (tcp1.m_p_socket->peerName() == m_p_socket->peerName()) &&
           (tcp1.m_p_socket->peerPort() == m_p_socket->peerPort());
  }

  Q_SLOT void disconnect();       /* @todo replace disconnect with a lambda */

  const QString & get_hostname() {return m_hostname;}
  const QTcpSocket * get_socket() {return m_p_socket;}

private:
  QString m_hostname;
  QTcpSocket * m_p_socket;
};
#endif  // DERP_ENGINE__TCP_CONNECTION_HPP_
