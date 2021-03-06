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

#include <derp-engine/tcp_thread.hpp>

#include <memory>

tcp_thread::tcp_thread(
  const QString & _hostname,  /*  hostname for TCP thread construction  */
  const quint16 & _port,      /* port on which we construct this server */
  const bool & _master_mode,  /*         are we in master mode?         */
  QObject * parent            /*         parent of this QObject         */
)
: QObject(parent)
{
  static_cast<void>(_master_mode);  // TODO(allenh1): use this parameter
  m_hostname = _hostname;
  m_port = _port;
  m_master_mode = true;
  m_pServer = std::make_unique<QTcpServer>(this);
  m_pTcpMessages = std::make_unique<QQueue<tcp_connection>>();
}

bool tcp_thread::init()
{
  if (!m_pServer) {
    return false;
  }
  /* forward accepted connections to our accept connection */
  connect(m_pServer.get(), &QTcpServer::newConnection,
    this, &tcp_thread::acceptConnection);

  /* listen for any host on our port */
  if (!m_pServer->listen(QHostAddress::Any, m_port)) {
    return false;
  }
  QString ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
  std::cout << tr("The server is running on\n\nIP: %1\nPort: %2\n")
    .arg(ipAddress).arg(m_pServer->serverPort()).toStdString() << std::endl;
  return true;
}

bool tcp_thread::writeData(QByteArray data, tcp_connection * receiver)
{
  QDataStream out(&data, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_5_5);

  QTcpSocket * to_receive = const_cast<QTcpSocket *>(receiver->get_socket());

  connect(to_receive, &QAbstractSocket::disconnected, to_receive, &QObject::deleteLater);

  to_receive->write(data);
  to_receive->disconnectFromHost();

  return true;
}

void tcp_thread::disconnected()
{
  QTcpSocket * quitter = qobject_cast<QTcpSocket *>(sender());
  /* convert to tcp_connection */
  QString _host = quitter->peerName();

  std::cout << "Client " << QHostAddress(
    quitter->peerAddress().toIPv4Address()).toString().toStdString();
  std::cout << ":" << quitter->peerPort();
  std::cout << " has left the server." << std::endl;
}

void tcp_thread::acceptConnection()
{
  QTcpSocket * client = m_pServer->nextPendingConnection();

  if (client) {
    connect(client, &QAbstractSocket::disconnected,
      client, &QObject::deleteLater);
    connect(client, &QAbstractSocket::disconnected,
      this, &tcp_thread::disconnected);
    connect(client, &QIODevice::readyRead,
      this, &tcp_thread::readFromClient);
  }
}

void tcp_thread::echoReceived(const QString & msg)
{
  std::cout << "I read \"" << msg.toStdString() << "\" from the client!" << std::endl;
}

void tcp_thread::readFromClient()
{
  /* cast as a QTcpSocket */
  QTcpSocket * pClientSocket = qobject_cast<QTcpSocket *>(sender());

  QString text;      /* to store the message */

  QByteArray bae = pClientSocket->readLine();
  QString temp = QString(bae);
  QString hostname = pClientSocket->peerName();
  text += temp.replace("\r\n", "");

  if (text.contains("search?word=")) {
    std::cout << "search requested: " << text.toStdString() << std::endl;
    text.replace("GET /search?word=", "");
    text.replace(" HTTP/1.1", "");
    text.replace(" HTTP/1.0", "");
    text.replace(QRegExp("the"), "");
    text.replace("+", " ");
    text = text.trimmed();
    std::cout << "after trim: \"" << text.toStdString() << "\"" << std::endl;
    if (text.size() == 0) {
      Q_EMIT (got_home_page(pClientSocket));
    } else {
      QString * temp = new QString(text);
      Q_EMIT (got_search(pClientSocket, temp));
    }
  } else {
    std::cout << "home page requested" << std::endl;
    Q_EMIT (got_home_page(pClientSocket));
  }
}

void tcp_thread::sendMessage(QString msg, tcp_connection * request)
{
  if (!writeData(msg.toUtf8(), request)) {
    std::cerr << "Unable to write message \"" <<
      msg.toStdString() << "\" to client." << std::endl;
  }
}

void tcp_thread::disconnect_client(tcp_connection * client, QString * _p_msg)
{
  QTcpSocket * p = const_cast<QTcpSocket *>(client->get_socket());
  p->write(_p_msg->toUtf8());
  delete _p_msg;
  p->disconnectFromHost();
  delete client;
}
