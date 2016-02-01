/*
* CT30A5002 Games and Networking
* udpexample, UDP server and client example
*
* Contains simple UDP server that waits for a char from some client and
* quits after it has been received.
*
* Contains simple UDP client that connects to given IP address and sends
* characters then quits.
*


TAA ON HYVÄ POHJA LÄHTEE LIIKKEELLE
*/

#include "server_unpackers.h"
#include "server_functions.h"
#define SIZE 256
#define TCPPORT "2056"

int addAckToList(){

}
int sendMsg(struct sockaddr *to, struct Ack *ack, struct Packet packet){
/* Wait what? !? */
}


int server(char* port) {
	struct Game game;
  int socketfd = -1, activity, fdmax, listener = -2, newfd, nbytes;
	fd_set readset, master;
	struct timeval tvSelect, tvUpdate1, tvUpdate2, tv;
	long time1, time2;

  struct addrinfo hints = { .ai_flags = AI_PASSIVE,	/* Get addresses suitable for bind */
                            .ai_family = PF_UNSPEC,
                            .ai_socktype = SOCK_DGRAM,	/* Datagram socket - UDP */
                            .ai_protocol = IPPROTO_UDP};/* UDP protocol */

  struct addrinfo *result = NULL, *iter = NULL;
  struct sockaddr_storage client_addr;

  char hostbuffer[NI_MAXHOST] = { 0 };
  char portbuffer[NI_MAXSERV] = { 0 };
  char recvbuffer[SIZE] = { 0 };
	char sendbuffer[SIZE] = {0};
	tv.tv_usec = 1000000;
	tv.tv_sec = 0;

  socklen_t addrlen = 0;
  unsigned int optval = 0;
  int dgramlen = 0;

	FD_ZERO(&master);
	FD_ZERO(&readset);

  printf("| - - - - - S E R V E R - - - - - |\n");

  if (atoi(port) >= 1024 && atoi(port) <= 65000) {
    // Get my (AI_PASSIVE) addresses that are suitable for bind
    if(getaddrinfo(NULL,port,&hints,&result)) {
      perror("cannot get addresses for server UDP");
      return -1;
    }

    // Go through all addresses returned with specified hints and use the first one
    for(iter = result; iter != NULL; iter = iter->ai_next) {
      if ((socketfd = socket(iter->ai_family,iter->ai_socktype,iter->ai_protocol)) < 0) {
        perror("socket()");
        return -1;
      }

      //Try to bind to this address
      if (bind(socketfd,iter->ai_addr, iter->ai_addrlen) < 0 ) {
        close(socketfd); /* Even when bind fails, socket remains, close it */
        perror("bind()");
        return -1;
      }

      break;
    }

		/* TCP socket initialization */
		struct addrinfo hintstcp = { .ai_flags = AI_PASSIVE,	/* Get addresses suitable for bind */
	                            .ai_family = AF_UNSPEC,
	                            .ai_socktype = SOCK_STREAM};/*  protocol */
		/****************************/
		result = NULL;
		iter = NULL;

		if(getaddrinfo(NULL,"4567",&hintstcp,&result)) {
      perror("cannot get addresses for server TCP");
      return -1;
    }
		for(iter = result; iter != NULL; iter = iter->ai_next) {
      if ((listener = socket(iter->ai_family,iter->ai_socktype,iter->ai_protocol)) < 0) {
        perror("socket()");
        return -1;
      }

      //Try to bind to this address
      if (bind(listener,iter->ai_addr, iter->ai_addrlen) < 0 ) {
        close(socketfd); /* Even when bind fails, socket remains, close it */
        perror("bind()");
        return -1;
      }

      break;
    }
		/*****************************/

    freeaddrinfo(result);

		if(listen(listener, 10) == -1){
			perror("Listen");
			exit(3);
		}

    /* Try to get the maximum length for read buffer */
    socklen_t optlen = sizeof(optval);
    if(getsockopt(socketfd,SOL_SOCKET,SO_RCVBUF,&optval,&optlen) == -1) perror("Cannot get read buffer size getsockopt()");
    else printf("Server: Read buffer in bytes: %u\n",optval);

    memset(&recvbuffer,0,SIZE);
    printf("Server: Waiting for datagram..\n");

    addrlen = sizeof(client_addr);
    struct sockaddr* client_address = (struct sockaddr*) &client_addr;

		gettimeofday(&tvUpdate1, NULL);
		gettimeofday(&tvUpdate2, NULL);

		time1 = tvUpdate1.tv_sec * 1000 + tvUpdate1.tv_usec / 1000;
		time2 = tvUpdate2.tv_sec * 1000 + tvUpdate2.tv_usec / 1000;

		tvSelect.tv_sec = 0;
		tvSelect.tv_usec = 1000000;

		// Add socket to the master set
		FD_SET(socketfd, &master);
		FD_SET(listener, &master);
		fdmax = listener+1;
		while (1) {

			// Refresh select() set
			//FD_ZERO(&readset);
			//FD_SET(socketfd, &readset);
			readset = master; // /* Copy master fd_set, so that won't change*/

			/* rest timeout values */
			//tvSelect.tv_sec = 0;
			//tvSelect.tv_usec = 1000000;


			// Start waiting for socket activity
			if((activity = select(fdmax+1, &readset, NULL, NULL, NULL)) == -1){
				perror("select");
				exit(4);
			}

			/* Run through existing connections, see if any data to read */
			for(int i = 0; i <= socketfd; i++){
				if(FD_ISSET(i, &readset)) {
					if(i == listener){
						/* New TCP connection */
						addrlen = sizeof(client_addr);
						newfd = accept(listener, (struct sockaddr*) &client_addr, &addrlen);
						if(newfd == -1){
							perror("Accept");
						}
						else {
							/* Add to master set */
							FD_SET(newfd, &master);
							if(newfd > fdmax)
								fdmax = newfd;

								/* Add tcp connection to player */
						}
					}
						else if(i == socketfd) {
							/* UDP msg */
							/* Try to receive something (expecting a char - length = 1 byte).. */
							dgramlen = recvfrom(socketfd,&recvbuffer,SIZE,0,client_address,&addrlen);
							struct Packet packet;
							packet = unpackPacket(recvbuffer, client_address);

							switch (packet.msgType) {

								// Game message packet
								case GAME_MESSAGE:
									printf("Game message packet received!\n");

									switch (packet.subType) {

										case JOIN:
											printf("Player joins game!\n");
											newPlayer(&game.sPlayers, packet, game.nPlayers);
											game.nPlayers++;
											msgPacker(sendbuffer, &game, packet.ID, ACK, JOIN, 0,1);
											sendto(socketfd, sendbuffer, SIZE, 0, &packet.senderAddr, addrlen);
											break;

										case NICK:
											printf("Player inserts nick!\n");
											break;

										case EXIT:
											printf("Player exits the game!\n");
											break;
									}

									break;

								// Ack packet
								case ACK:
									printf("Ack packet received!\n");
									break;

								// Player movement packet
								case PLAYER_MOVEMENT:
									printf("Player movement packet received!\n");
									break;

								// Statisic packet
								case STATISTICS_MESSAGE:
									printf("Player movement packet received!\n");
									break;

								default:
									printf("Invalid packet received!\n");

							}

							printf("PacketID: %d\n", packet.ID);
							printf("msgType: %d\n", packet.msgType);
							printf("subtype: %d\n", packet.subType);
							/*************/
						}
						else{
							/* TCP CHAT MSG */
							if((nbytes = recv(i, recvbuffer, sizeof(recvbuffer), 0)) <= 0){
								/* Connection closed or error */

							}
						}
					}
				}
			}

			// UPD activity


			if ((time2 - time1) >= 5000) {
				printf("Game update!\n");
				gettimeofday(&tvUpdate1, NULL);
				time1 = tvUpdate1.tv_sec * 1000 + tvUpdate1.tv_usec / 1000;
			}
			gettimeofday(&tvUpdate2, NULL);
			time2 = tvUpdate2.tv_sec * 1000 + tvUpdate2.tv_usec / 1000;


			// Run game update if enough time passed since last update

/* Question:
* Since we know the address and port client is using, it should be easy to send something
* back. Think about the operation and reverse it without initializing new address.
* What can be changed in sendto()-function parameters if message is going to be sent to
* someone else?
*/

    close(socketfd); /* REMEMBER ME! */
  }
  else {
    printf("Server: Invalid port. Choose something between 1024 - 65000\n");
    return -1;
  }

  return 0;
}

int client(char* port, char *serverip)
{
  int socketfd = -1, length = 0, rval = 0;
  char dgram[SIZE];

  struct addrinfo hints = { .ai_flags = AI_NUMERICHOST|AI_NUMERICSERV,
                            .ai_family = PF_UNSPEC,
                            .ai_socktype = SOCK_DGRAM,
                            .ai_protocol = IPPROTO_UDP};

  struct addrinfo *result = NULL, *iter = NULL;

  memset(&dgram,1,SIZE);

  printf("| - - - - - C L I E N T - - - - - |\n");

  /* Why only IP address works ? */
  if(getaddrinfo(serverip,port,&hints,&result) < 0) perror("Cannot resolve address");
  else {
    // Go through every returned address and attempt to connect to each
    for (iter = result; iter != NULL; iter = iter->ai_next) {

      /* Can socket be created? */
      if ((socketfd = socket(iter->ai_family, iter->ai_socktype,iter->ai_protocol)) < 0) {
        perror("socket()");
        rval = -1;
        break;
      }

      /* Try to send data to server:
      * sendto(socket, data , data length, flags, destination, struct length)
      * see 'man sendto'
			GAME_MESSAGE:JOIN  packet
      */
			int index = 0;
      uint16_t uid = 32;
      *(uint16_t*)&dgram[index] = htons(uid);
      index += sizeof(uint16_t);

      uint32_t gametime = 25;
      uint8_t msgtype = GAME_MESSAGE;
      *(uint32_t*)&dgram[index] = htonl(gametime);
      index += sizeof(uint32_t);

      *(uint8_t*)&dgram[index] = msgtype;
			index += sizeof(uint8_t);

			uint32_t pllength = 23;
			*(uint32_t*)&dgram[index] = htonl(gametime);
      index += sizeof(uint32_t);

			uint8_t subtype = JOIN;
			*(uint8_t*)&dgram[index] = subtype;

      if((length = sendto(socketfd,&dgram,SIZE,0,iter->ai_addr,iter->ai_addrlen)) < 0) {
        perror("sendto()");
        rval = -1;
        break;
      }
      else printf("Client: Sent datagram length = %d\n", length);

    }
  }

  freeaddrinfo(result);

  close(socketfd); /* REMEMBER ME! */

  return rval;
}


int main(int argc, char *argv[])
{
  /* server */
  if (argc == 2)
  {
    if (server(argv[1]) == 0) printf("Server: exited with success\n");
    else
    {
      printf("Errors with server\n");
      return -1;
    }
  }
  /* client */
  else if (argc == 3)
  {
    if (client(argv[1],argv[2]) == 0) printf("Client: exited with success\n");
    else
    {
      printf("Errors with client\n");
      return -1;
    }
  }
  /* error */
  else
  {
    printf("Invalid amount of arguments.\nUsage:\n\
      server: %s <portnumber>\n\
      client: %s <portnumber> <server ip>\n",argv[0],argv[0]);
    return -1;
  }
  return 0;
}