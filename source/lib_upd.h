int create_udp_client (void);
int create_udp_server (char *ip_address, int port);

int close_udp_socket (int sk);

void udp_set_non_blocking_mode (int sk);
void udp_set_blocking_mode (int sk);

int udp_send (int sk, char *buffer, char *ip_address, int port);
int udp_binary_send (int sk, char *buffer, int msg_len, int ip_address, int port);
int udp_receive (int sk, char *buffer);
int udp_binary_receive (int sk, char *buffer);
int udp_receive_and_get_sender_info (int sk, char *buffer, 
                                     char *ip_address, int *pport);
int udp_binary_receive_and_get_sender_info (int sk, char *buffer, 
                                     char *ip_address, int *pport);
int udp_reply (int sk, char *buffer);
int udp_binary_reply (int sk, char *buffer, int msg_len);

void error_handler (const char *message);
