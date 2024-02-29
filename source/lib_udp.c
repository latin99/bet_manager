#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include "udpsocketlib.h"

static struct sockaddr_in reply_to_socket_address;
static int len_of_reply_to_socket_address;


int create_udp_client (void)
{
   int sk;

   /* create a socket descriptor */
   if ((sk = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
   {
      error_handler ("socket() [create_udp_client()]");
      return -1;
   }

   return sk;
}


int create_udp_server (char *ip_address, int port)
{
   int sk;
   struct sockaddr_in srv;

   /* create a socket descriptor */
   if ((sk = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
   {
      error_handler ("socket() [create_udp_server()]");
      return -1;
   }

   /* fill the (used) fields of the socket address with
      the server information (local socket address) */
   bzero ((char *) &srv, sizeof (srv));
   srv.sin_family = AF_INET;
   srv.sin_addr.s_addr = inet_addr (ip_address);
   srv.sin_port = htons (port);

   /* add the local socket address to the socket descriptor */
   if (bind (sk, (struct sockaddr *) &srv, sizeof(srv)) < 0)
   {
      error_handler ("bind() [create_udp_server()]");
      return -1;
   }

   return sk;
}


int close_udp_socket (int sk)
{
   if (close (sk) != 0)
   {
      error_handler ("close() [close_udp_socket()]");
      return 0;
   }

   return 1;
}


void udp_set_non_blocking_mode (int sk)
{
   int flags;
   
   flags = fcntl (sk, F_GETFL, 0);
   fcntl(sk, F_SETFL, flags | O_NONBLOCK);
   
   return;
}


void udp_set_blocking_mode (int sk)
{
   int flags;

   flags = fcntl (sk, F_GETFL, 0);
   fcntl(sk, F_SETFL, flags & ~O_NONBLOCK);

   return;
}


int udp_send (int sk, char *buffer, char *ip_address, int port)
{
   struct sockaddr_in ska;
   int msg_len = strlen(buffer);

   /* fill the (used) fields of the socket address with
      the server information (remote socket address) */
   bzero ((char *) &ska, sizeof (ska));
   ska.sin_family = AF_INET;
   ska.sin_addr.s_addr = inet_addr (ip_address);
   ska.sin_port = htons ((unsigned short) port);

   if (sendto (sk, buffer, msg_len, 0, (struct sockaddr *) &ska, sizeof(ska)) != msg_len)
   {
      error_handler ("sendto() [udp_send()]");
      return 0;
   }

   return 1;
}


int udp_binary_send (int sk, char *buffer, int msg_len, int ip_address, int port)
{
   struct sockaddr_in ska;

   /* fill the (used) fields of the socket address with
      the server information (remote socket address) */
   bzero ((char *) &ska, sizeof (ska));
   ska.sin_family = AF_INET;
   ska.sin_addr.s_addr = ip_address;
   ska.sin_port = htons ((unsigned short) port);

   if (sendto (sk, buffer, msg_len, 0, (struct sockaddr *) &ska, sizeof(ska)) != msg_len)
   {
      error_handler ("sendto() [udp_binary_send()]");
      return 0;
   }

   return 1;
}


int udp_receive (int sk, char *buffer)
{
   int dim, flags;

   len_of_reply_to_socket_address = sizeof (reply_to_socket_address);
   if ((dim = recvfrom (sk, buffer, BUFSIZ, 0,
                        (struct sockaddr *) &reply_to_socket_address,
                        (socklen_t *) &len_of_reply_to_socket_address)) < 0)
   {
      flags = fcntl (sk, F_GETFL, 0);
      if ((flags & O_NONBLOCK) == O_NONBLOCK)
      {   /* non-blocking mode */
         if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            return 0;
         else
         {
            error_handler ("recvfrom() [udp_receive()]");
            return -1;
         }
      }
      else
      {
         error_handler ("recvfrom() [udp_receive()]");
         return -1;
      }
   }

   buffer [dim] = '\0';

   return dim;
}


int udp_binary_receive (int sk, char *buffer)
{
   int dim, flags;

   len_of_reply_to_socket_address = sizeof (reply_to_socket_address);
   if ((dim = recvfrom (sk, buffer, BUFSIZ, 0,
                        (struct sockaddr *) &reply_to_socket_address,
                        (socklen_t *) &len_of_reply_to_socket_address)) < 0)
   {
      flags = fcntl (sk, F_GETFL, 0);
      if ((flags & O_NONBLOCK) == O_NONBLOCK)
      {   /* non-blocking mode */
         if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            return 0;
         else
         {
            error_handler ("recvfrom() [udp_binary_receive()]");
            return -1;
         }
      }
      else
      {
         error_handler ("recvfrom() [udp_binary_receive()]");
         return -1;
      }
   }

   return dim;
}


int udp_receive_and_get_sender_info (int sk, char *buffer, 
                                     char *ip_address, int *pport)
{
   int dim, flags;

   len_of_reply_to_socket_address = sizeof (reply_to_socket_address);
   if ((dim = recvfrom (sk, buffer, BUFSIZ, 0,
                        (struct sockaddr *) &reply_to_socket_address,
                        (socklen_t *) &len_of_reply_to_socket_address)) < 0)
   {
      flags = fcntl (sk, F_GETFL, 0);
      if ((flags & O_NONBLOCK) == O_NONBLOCK)
      {   /* non-blocking mode */
         if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            return 0;
         else
         {
            error_handler ("recvfrom() [udp_binary_receive()]");
            return -1;
         }
      }
      else
      {
         error_handler ("recvfrom() [udp_binary_receive()]");
         return -1;
      }
   }

   *pport = ntohs (reply_to_socket_address.sin_port);
   strcpy (ip_address,
           (char *) inet_ntoa (reply_to_socket_address.sin_addr));

   buffer [dim] = '\0';

   return dim;
}


int udp_binary_receive_and_get_sender_info (int sk, char *buffer, 
                                     char *ip_address, int *pport)
{
   int dim, flags;

   len_of_reply_to_socket_address = sizeof (reply_to_socket_address);
   if ((dim = recvfrom (sk, buffer, BUFSIZ, 0,
                        (struct sockaddr *) &reply_to_socket_address,
                        (socklen_t *) &len_of_reply_to_socket_address)) < 0)
   {
      flags = fcntl (sk, F_GETFL, 0);
      if ((flags & O_NONBLOCK) == O_NONBLOCK)
      {   /* non-blocking mode */
         if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            return 0;
         else
         {
            error_handler ("recvfrom() [udp_binary_receive()]");
            return -1;
         }
      }
      else
      {
         error_handler ("recvfrom() [udp_binary_receive()]");
         return -1;
      }
   }

   *pport = ntohs (reply_to_socket_address.sin_port);
   strcpy (ip_address,
           (char *) inet_ntoa (reply_to_socket_address.sin_addr));

   return dim;
}


int udp_reply (int sk, char *buffer)
{
   int msg_len = strlen(buffer);

   if (sendto (sk, buffer, msg_len, 0,
       (struct sockaddr *) &reply_to_socket_address,
       len_of_reply_to_socket_address) < 0)
   {
      error_handler ("sendto() [udp_reply()]");
      return 0;
   }

   return 1;
}


int udp_binary_reply (int sk, char *buffer, int msg_len)
{
   if (sendto (sk, buffer, msg_len, 0,
       (struct sockaddr *) &reply_to_socket_address,
       len_of_reply_to_socket_address) < 0)
   {
      error_handler ("sendto() [udp_reply()]");
      return 0;
   }

   return 1;
}
