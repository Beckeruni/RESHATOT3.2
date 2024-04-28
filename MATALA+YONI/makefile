CC = gcc
CFLAGS = -Wall -g

all: RUDP_Sender RUDP_Reciever

RUDP_Sender: RUDP_Sender.c RUDP.h
	$(CC) $(CFLAGS) -o RUDP_Sender RUDP_Sender.c
TCP_Reciever: RUDP_Reciever.c RUDP.h
	$(CC) $(CFLAGS) -o RUDP_Reciever RUDP_Reciever.c
clean:
	rm -f *.o *.a RUDP_Sender RUDP_Reciever