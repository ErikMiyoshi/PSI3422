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
#include <cekeikon.h>
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
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
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
  cout << "EsperandoConexão" << endl;
  while (1) {
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

void SERVER::sendUint(uint32_t m) {
  static uint32_t n;
  n = htonl(m);
  sendBytes(4, (BYTE *)&n);
}

void SERVER::receiveUint(uint32_t &m) {
  static uint32_t n;
  receiveBytes(4, (BYTE *)&n);
  m = ntohl(n);
}

void SERVER::sendString(const string &st) {
  int tamanho = st.length();
  sendUint(tamanho);
  sendBytes(tamanho, (BYTE *)st.data());
}

void SERVER::receiveString(string &st) {
  uint tamanho;
  receiveUint(tamanho);
  st.resize(tamanho);
  receiveBytes(tamanho, (BYTE *)st.data());
}

void SERVER::sendVb(const vector<BYTE> &vb) {
  int tamanho = vb.size();
  sendUint(tamanho);
  sendBytes(tamanho, (BYTE *)vb.data());
}

void SERVER::receiveVb(vector<BYTE> &vb) {
  uint tamanho;
  receiveUint(tamanho);
  vb.resize(tamanho);
  receiveBytes(tamanho, (BYTE *)vb.data());
}

void SERVER::sendImg(const Mat_<COR> &img) {
  if (img.isContinuous()) {
    sendUint(img.rows);
    sendUint(img.cols);
    sendBytes((3 * img.rows * img.cols), (BYTE *)img.data);
  }
}
void SERVER::receiveImg(Mat_<COR> &Img) {
  uint nl, nc;
  receiveUint(nl);
  receiveUint(nc);
  Mat_<COR> img(nl, nc);
  receiveBytes(3 * nl * nc, img.data);
}

void SERVER::sendImgComp(const Mat_<COR> &img) {
  vector<BYTE> vb;
  vector<int> param{CV_IMWRITE_JPEG_QUALITY, 80};
  imencode(".jpg", img, vb, param);
  sendVb(vb);
}
void SERVER::receiveImgComp(Mat_<COR> &img) {
  vector<BYTE> vb;
  receiveVb(vb);
  img = imdecode(vb, 1); // Numero 1 indica imagem colorida
}

void SERVER::encerra() { close(new_fd); }