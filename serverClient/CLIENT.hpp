#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef uint8_t BYTE;
using namespace std;

#define PORT "3490"     // the port users will be connecting to
#define BACKLOG 1       // how many pending connections queue will hold
#define MAXDATASIZE 256 // max number of bytes we can get at once

class CLIENT {
private:
  /* data */
public:
  CLIENT(string endereco);
  ~CLIENT();

  void sendBytes(int nBytesToSend, BYTE *buf);
  void receiveBytes(int nBytesToReceive, BYTE *buf);
  void *get_in_addr(struct sockaddr *sa);

  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  string endereco;
};

CLIENT::CLIENT(string endereco) {
  std::cout << "teste1" << std::endl;
  this->endereco = endereco;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  char s[this->endereco.size() + 1];
  this->endereco.copy(s, this->endereco.length() + 1);
  s[this->endereco.size()] = '\0';
  if ((rv = getaddrinfo(s, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    // return 1;
  }
  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sockfd);
      continue;
    }
    break;
  }
  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    // return 2;
  }
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
            sizeof s);
  printf("client: connecting to %s\n", s);
  freeaddrinfo(servinfo); // all done with this structure
}

CLIENT::~CLIENT() {}

void CLIENT::sendBytes(int nBytesToSend, BYTE *buf) {
  for (int i = 0; i < nBytesToSend; i++) {
    if (send(sockfd, &buf[i], 1, 0) == -1)
      perror("send");
  }
  /*if (send(sockfd, buf, strlen((char *)buf) + 1, 0) == -1)
    perror("send");*/
}

void CLIENT::receiveBytes(int nBytesToReceive, BYTE *buf) {
  for (int i = 0; i < nBytesToReceive; i++) {
    cout << i << endl;
    if (recv(sockfd, &buf[i], 1, 0) == -1)
      perror("recv");
  }
  /*if (recv(sockfd, buf, MAXDATASIZE, 0) == -1)
    perror("recv");*/
}

void *CLIENT::get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}