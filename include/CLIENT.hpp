#include <iostream>
#include <string>
#include <vector>

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

#include "cekeikon.h"

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
  void sendUint(uint32_t m);
  void receiveUint(uint32_t &m);
  void sendString(const string &st);
  void receiveString(string &st);
  void sendVb(const vector<BYTE> &vb);
  void receiveVb(vector<BYTE> &st);
  void sendImg(const Mat_<COR> &img);
  void receiveImg(Mat_<COR> &Img);
  void sendImgComp(const Mat_<COR> &img);
  void receiveImgComp(Mat_<COR> &img);

  void encerra();

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

  while (nBytesToSend > 0) {
    int nSentBytes = send(sockfd, buf, nBytesToSend, 0);
    if (nSentBytes == -1) {
      perror("camserver: send");
      exit(1);
    }
    nBytesToSend -= nSentBytes;
    buf += nSentBytes;
  }

  /*for (int i = 0; i < (nBytesToSend / MAXDATASIZE) + 1; i++) {
    if (send(sockfd, &buf[i], MAXDATASIZE, 0) == -1)
      perror("send");
  }*/
  /*if (send(sockfd, buf, strlen((char *)buf) + 1, 0) == -1)
    perror("send");*/
}

void CLIENT::receiveBytes(int nBytesToReceive, BYTE *buf) {
  while (nBytesToReceive > 0) {
    int nReceivedBytes = recv(sockfd, buf, nBytesToReceive, 0);
    if (nReceivedBytes == -1) {
      perror("recv");
      exit(1);
    }
    nBytesToReceive -= nReceivedBytes;
    buf += nReceivedBytes;
  }

  /*for (int i = 0; i < (nBytesToReceive / MAXDATASIZE) + 1; i++) {
    if (recv(sockfd, &buf[i], MAXDATASIZE, 0) == -1)
      perror("recv");
  }*/
  /*if (recv(sockfd, buf, MAXDATASIZE, 0) == -1)
    perror("recv");*/
}

void *CLIENT::get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void CLIENT::sendUint(uint32_t m) {
  static uint32_t n;
  n = htonl(m);
  sendBytes(4, (BYTE *)&n);
}

void CLIENT::receiveUint(uint32_t &m) {
  static uint32_t n;
  receiveBytes(4, (BYTE *)&n);
  m = ntohl(n);
}

void CLIENT::sendString(const string &st) {
  int tamanho = st.length();
  sendUint(tamanho);
  sendBytes(tamanho, (BYTE *)st.data());
}

void CLIENT::receiveString(string &st) {
  uint tamanho;
  receiveUint(tamanho);
  st.resize(tamanho);
  receiveBytes(tamanho, (BYTE *)st.data());
}

void CLIENT::sendVb(const vector<BYTE> &vb) {
  int tamanho = vb.size();
  sendUint(tamanho);
  sendBytes(tamanho, (BYTE *)vb.data());
}

void CLIENT::receiveVb(vector<BYTE> &vb) {
  uint tamanho;
  receiveUint(tamanho);
  vb.resize(tamanho);
  receiveBytes(tamanho, (BYTE *)vb.data());
}

void CLIENT::sendImg(const Mat_<COR> &img) {
  if (img.isContinuous()) {
    sendUint(img.rows);
    sendUint(img.cols);
    sendBytes((3 * img.rows * img.cols), (BYTE *)img.data);
  }
}
void CLIENT::receiveImg(Mat_<COR> &Img) {
  uint nl, nc;
  receiveUint(nl);
  receiveUint(nc);
  Img.create(nl, nc);
  receiveBytes(3 * nl * nc, Img.data);
}

void CLIENT::sendImgComp(const Mat_<COR> &img) {
  vector<BYTE> vb;
  vector<int> param{CV_IMWRITE_JPEG_QUALITY, 80};
  imencode(".jpg", img, vb, param);
  sendVb(vb);
}
void CLIENT::receiveImgComp(Mat_<COR> &img) {
  vector<BYTE> vb;
  receiveVb(vb);
  img = imdecode(vb, 1); // Numero 1 indica imagem colorida
}

void CLIENT::encerra() { close(sockfd); }