#ifndef __BBSOCKET_H__
#define __BBSOCKET_H__
#include "bbtypes.h"
/**
   \headerfile bbsocket.h "bbsocket.h"
   \ingroup RuntimeInterface
*/

/**
 *   \defgroup BBSOCKETAPI BB Socket APIs
 *   \brief This APIs are used by exchanging messages among GPP/PEs 
 *   \addtogroup BBSOCKETPI
 *  @{
 */

/**
   \brief creates an endpoint for communication and returns a descriptor
   \return return a bb_socket descriptor
 */
int32_t bb_socket(int32_t domain, int32_t type, int32_t protocol);

/**
   \brief assign an address to a bb_socket
   \param[in] sockfd is the bbsocket
   \param[in] addr is the assigned address
   \return return zero on success
 */
int32_t bb_bind(int32_t sockfd, bb_sockaddr_t *addr);

/**
   \brief marks  the  socket  referred  to by sockfd as a passive socket, that is, as a socket that will be used to accept incoming connection requests using bb_accept
   \param[in] sockfd: is the bbsocket
   \param[in] backlog: defines the maximum length to which the queue of pending connections for sockfd may grow
   \return return zero on success
 */
int32_t bb_listen(int32_t sockfd, int32_t backlog);


/**
   \brief bb_accept is used with connection-based socket types.  It extracts the first connection request on the queue of pending connections for the listening socket, sockfd, creates a new  connected  socket,
   \param[in] sockfd: is a socket that has been created with bb_socket
   \param[out] addr: is a pointer to a sockaddr structure.  This structure is filled in with the address of the peer socket as known to the communications layer.
   \return a new file descriptor referring to that socket.
 */
int32_t bb_accept(int32_t sockfd, bb_sockaddr_t *addr);

/**
   \brief bb_connect is used with connection-based socket types.  It connects to the socket corresponding to the addr
   \param[in] sockfd: is a socket that has been created with bb_socket
   \param[in] addr: is a pointer to a sockaddr structure.  That defines the destination socket
   \return a new file descriptor referring to that socket.
 */
int32_t bb_connect(int32_t sockfd, bb_sockaddr_t *addr);

/**
   \brief It's used to receive messages from a socket, and may be used to receive data on a socket whether or not it is connection-oriented.
   \param[in] sockfd: is a socket that has been created with bb_socket
   \param[out] buf: data buffer
   \param[in] len: is the buffer length
   \param[in] flags: receive flags
 */
int32_t bb_recv(int32_t sockfd, void *buf, size_t len, int32_t flags);

/**
   \brief It's used to send messages to a socket.
   \param[in] sockfd: is a socket that has been created with bb_socket
   \param[int] buf: data buffer
   \param[in] len: is the buffer length
   \param[in] flags: receive flags
 */
int32_t bb_send(int32_t sockfd, const void *buf, size_t len, int32_t flags);
/**
 *  @}
 */
#endif
