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

class SERVER {
private:
  /* data */
public:
  SERVER(/* args */);
  ~SERVER();
  void waitConnection();
  void sendBytes(int nBytesToSend, BYTE *buf);
  void receiveBytes(int nBytesToReceive, BYTE *buf);
  void *get_in_addr(struct sockaddr *sa);

  socklen_t sin_size;
  struct their_addr;
  int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  struct sigaction sa;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  int rv;
};

SERVER::SERVER() {
  std::cout << "t1" << std::endl;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  std::cout << "t2" << std::endl;
  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    // return 1;
  }
  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    break;
  }
  freeaddrinfo(servinfo); // all done with this structure
  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }
  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
}

SERVER::~SERVER() {}

void SERVER::waitConnection() {
  cout << "WaitConnection" << endl;
  while (1) {
    cout << "esperando" << endl;
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    } else
      break;
  }
  inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
  printf("server: got connection from %s\n", s);
  close(sockfd); // doesn't need the listener anymore
}

void SERVER::sendBytes(int nBytesToSend, BYTE *buf) {
  for (int i = 0; i < nBytesToSend; i++) {
    if (send(new_fd, &buf[i], 1, 0) == -1)
      perror("send");
  }

  /*if (send(new_fd, buf, strlen((char *)buf) + 1, 0) == -1)
    perror("send");*/
}

void SERVER::receiveBytes(int nBytesToReceive, BYTE *buf) {
  for (int i = 0; i < nBytesToReceive; i++) {
    if (recv(new_fd, &buf[i], 1, 0) == -1)
      perror("recv");
  }
  /*if (recv(new_fd, buf, MAXDATASIZE, 0) == -1)
    perror("recv");*/
}

void *SERVER::get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

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

bool testaBytes(BYTE *buf, BYTE b, int n) {
  // Testa se n bytes da memoria buf possuem valor b
  bool igual = true;
  for (unsigned i = 0; i < n; i++)
    if (buf[i] != b) {
      igual = false;
      break;
    }
  return igual;
}
