#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAX_BUFFER_SIZE 1024

typedef struct{
    uint16_t length;
    uint16_t checksum;
    uint16_t seq;
    unsigned int ACK: 1;
    unsigned int FIN: 1;
} RUDPHeader;

typedef struct{
    int sockFD;
    struct sockaddr_in sender_addr;
} RUDPSocket;




struct RUDPSocket* create_rudp_socket(char* ip, int port);

struct RUDPHeader* create_rudp_header();


void rudp_close(struct RUDPSocket* socket);

char* rudp_send(struct RUDPSocket *socket, struct RUDPHeader *header, char* data, size_t data_size);


char* rudp_recv(struct RUDPSocket *socket, struct RUDPHeader *header, char* data, size_t data_size);

int create_handshake(struct RUDPHeader *header, struct RUDPSocket *socket);


struct RUDPHeader* create_rudp_header();

RUDPHeader* get_header(char* buffer);

int recieved_ack(RUDPHeader* header);

int correct_seq(RUDPHeader* header, uint16_t correct);

