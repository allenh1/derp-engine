#include "tcp_connection.hpp"

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
tcp_connection::tcp_connection(QString & _hostname,
							   QTcpSocket * _p_socket,
							   QObject * _p_parent)
   : m_hostname(_hostname),
	 m_p_socket(_p_socket),
	 QObject(_p_parent)
{ /* Construct a tcp_connection object. */ }

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
