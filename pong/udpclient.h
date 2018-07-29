#ifndef _UDPCLIENT_H_
#define _UDPCLIENT_H_



void UDP_client_init(int player);
void UDP_stop(void);
void UDP_send_message(char *buf);

#endif
