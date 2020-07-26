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

#include <derp-engine/tcp_connection.hpp>

/**
 * @brief Construct a tcp_connection.
 *
 * A tcp_connection is our way of keeping
 * track of our clients, as they connect
 * and disconnect asynchronously.
 *
 * @param _hostname Hostname of this connection.
 * @param _p_socket Tcp Socket for this connection.
 * @param _p_parent Parent QObject.
 */
tcp_connection::tcp_connection(
  QString & _hostname,
  QTcpSocket * _p_socket,
  QObject * _p_parent)
: QObject(_p_parent),
  m_hostname(_hostname),
  m_p_socket(_p_socket)
{ /* Construct a tcp_connection object. */}

/**
 * @brief destruct a tcp_connection
 */
tcp_connection::~tcp_connection()
{
  /**
   * @todo "Is this all I have to do?"
   */
}

/**
 * @brief Handle disconnects
 *
 * This is the slot that disconnects
 * this socket.
 */
void tcp_connection::disconnect()
{
  /**
       * @todo Implement this function
       */
  m_p_socket->write("BYE\r\n");
  m_p_socket->disconnectFromHost();
}
