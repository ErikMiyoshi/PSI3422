//projeto.hpp - incluir no projeto
//eliminei MAXDATASIZE
#include <cekeikon.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <signal.h>

class SERVER {
  const string PORT="3490";  // the port users will be connecting to
  const int BACKLOG=1;   // how many pending connections queue will hold

  int listensock, sockfd;  // listen on sock_fd, new connection on sockfd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  // get sockaddr, IPv4 or IPv6:
  static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
public:
  SERVER() {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(NULL, PORT.c_str(), &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); exit(1);
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((listensock = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) {
        perror("server: socket"); continue;
      }
      if (setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &yes,
          sizeof(int)) == -1) {
        perror("setsockopt"); exit(1);
      }
      if (bind(listensock, p->ai_addr, p->ai_addrlen) == -1) {
        close(listensock); perror("server: bind"); continue;
      }
      break;
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (p == NULL)  {
      fprintf(stderr, "server: failed to bind\n"); exit(1);
    }
    if (listen(listensock, BACKLOG) == -1) {
      perror("listen"); exit(1);
    }
  }

  ~SERVER() {
    close(listensock);
    close(sockfd);
    printf("server: fechei conexao.\n");
  }

  void waitConnection() {
    printf("server: Esperando conexao...\n");
    while (true) {
      sin_size = sizeof their_addr;
      sockfd = accept(listensock, (struct sockaddr *)&their_addr, &sin_size);
      if (sockfd == -1) {
        perror("accept"); continue;
      } else break;
    }
    inet_ntop(their_addr.ss_family,
      get_in_addr((struct sockaddr *)&their_addr),
      s, sizeof s);
    printf("server: recebi conexao de %s\n", s);
  }

  void sendBytes(int nBytesToSend, BYTE *buf) {
    while (nBytesToSend > 0) {
      int nSentBytes = send(sockfd, buf, nBytesToSend, 0);
      if (nSentBytes==-1) {
        perror("camserver: send"); exit(1);
      }
      nBytesToSend -= nSentBytes;
      buf += nSentBytes;
    }
  }

  void receiveBytes(int nBytesToReceive, BYTE *buf) {
    while (nBytesToReceive > 0) {
      int nReceivedBytes = recv(sockfd, buf, nBytesToReceive, 0);
      if (nReceivedBytes==-1) { perror("recv"); exit(1); }
      nBytesToReceive -= nReceivedBytes;
      buf += nReceivedBytes;
    }
  }

  void sendUint(uint32_t m) {
    static uint32_t n;
    n=htonl(m);
    sendBytes(4,(BYTE*)&n);
  }

  void sendInt(int32_t m) {
    static int32_t n;
    n=htonl(m);
    sendBytes(4,(BYTE*)&n);
  }

  void sendChar(char ch) {
    static char n;
    n=ch;
    sendBytes(1,(BYTE*)&ch);
  }

  void sendVb(const vector<BYTE>& vb) {
    sendUint(vb.size());
    sendBytes(vb.size(),(BYTE*)vb.data());
  }

  void sendString(const string& st) {
    sendUint(st.length());
    sendBytes(st.length(),(BYTE*)st.data());
  }
  
  void sendImg(const Mat_<COR>& img) {
    if (!img.isContinuous()) erro("Erro sendImg: Imagem nao-continua");
    sendUint(img.rows);
    sendUint(img.cols);
    sendBytes(3*img.total(),img.data);
  }
  
  void sendImgComp(const Mat_<COR>& img) {
    if (!img.isContinuous()) erro("Erro sendImgComp: Imagem nao-continua");
    vector<BYTE> vb;
    vector<int> param{CV_IMWRITE_JPEG_QUALITY,80};
    imencode(".jpg",img,vb,param);
    sendVb(vb);
  }

  void receiveChar(char& ch) {
    receiveBytes(1,(BYTE*)&ch);
  }

  void receiveUint(uint32_t& m) {
    receiveBytes(4,(BYTE*)&m);
    m=ntohl(m);
  }

  void receiveInt(int32_t& m) {
    receiveBytes(4,(BYTE*)&m);
    m=ntohl(m);
  }

  void receiveVb(vector<BYTE>& vb) {
    uint32_t t;
    receiveUint(t);
    vb.resize(t);
    receiveBytes(vb.size(),(BYTE*)vb.data());
  }

  void receiveString(string& st) {
    uint32_t t;
    receiveUint(t);
    st.resize(t);
    receiveBytes(st.size(),(BYTE*)st.data());
  }
  
  void receiveImg(Mat_<COR>& img) {
    uint32_t nl; receiveUint(nl);
    uint32_t nc; receiveUint(nc);
    img.create(nl,nc);
    receiveBytes(3*img.total(),img.data);
  }
  
  void receiveImgComp(Mat_<COR> &img) {
    vector<BYTE> vb;
    receiveVb(vb);
    img=imdecode(vb,1);
  }    
};

class CLIENT {
  const string PORT="3490"; // the port client will be connecting to 
  
  int sockfd, numbytes;  
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  // get sockaddr, IPv4 or IPv6:
  static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
public:
  CLIENT(string endereco) {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(endereco.c_str(), PORT.c_str(), &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); exit(1);
    }
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) {
        perror("client: socket"); continue;
      }
      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        perror("client: connect"); close(sockfd); continue;
      }
      break;
    }
    if (p == NULL) {
      fprintf(stderr, "client: failed to connect\n"); exit(1);
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
        s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure
  }
  
  ~CLIENT() {
    close(sockfd);
  }
  
  void sendBytes(int nBytesToSend, BYTE *buf) {
    while (nBytesToSend>0) {
      int nSentBytes = send(sockfd, buf, nBytesToSend, 0);
      if (nSentBytes==-1) {   
        perror("camclient: send"); exit(1);
      }
      nBytesToSend -= nSentBytes;
      buf += nSentBytes;
    }
  }

  void receiveBytes(int nBytesToReceive, BYTE *buf) {
    while (nBytesToReceive>0) {
      int nReceivedBytes = recv(sockfd, buf, nBytesToReceive, 0);
      if (nReceivedBytes==-1) { perror("recv"); exit(1); }
      nBytesToReceive -= nReceivedBytes;
      buf += nReceivedBytes;
    }
  }

  void sendUint(uint32_t m) {
    static uint32_t n;
    n=htonl(m);
    sendBytes(4,(BYTE*)&n);
  }

  void sendInt(int32_t m) {
    static int32_t n;
    n=htonl(m);
    sendBytes(4,(BYTE*)&n);
  }

  void sendChar(char ch) {
    static char n;
    n=ch;
    sendBytes(1,(BYTE*)&ch);
  }

  void sendVb(const vector<BYTE>& vb) {
    sendUint(vb.size());
    sendBytes(vb.size(),(BYTE*)vb.data());
  }
  
  void sendString(const string& st) {
    sendUint(st.length());
    sendBytes(st.length(),(BYTE*)st.data());
  }
  
  void sendImg(const Mat_<COR>& img) {
    if (!img.isContinuous()) erro("Erro sendImg: Imagem nao-continua");
    sendUint(img.rows);
    sendUint(img.cols);
    sendBytes(3*img.total(),img.data);
  }
  
  void sendImgComp(const Mat_<COR>& img) {
    if (!img.isContinuous()) erro("Erro sendImgComp: Imagem nao-continua");
    vector<BYTE> vb;
    vector<int> param{CV_IMWRITE_JPEG_QUALITY,80};
    imencode(".jpg",img,vb,param);
    sendVb(vb);
  }

  void receiveChar(char& ch) {
    receiveBytes(1,(BYTE*)&ch);
  }

  void receiveUint(uint32_t& m) {
    receiveBytes(4,(BYTE*)&m);
    m=ntohl(m);
  }

  void receiveInt(int32_t& m) {
    receiveBytes(4,(BYTE*)&m);
    m=ntohl(m);
  }

  void receiveVb(vector<BYTE>& vb) {
    uint32_t t;
    receiveUint(t);
    vb.resize(t);
    receiveBytes(vb.size(),(BYTE*)vb.data());
  }
  
  void receiveString(string& st) {
    uint32_t t;
    receiveUint(t);
    st.resize(t);
    receiveBytes(st.size(),(BYTE*)st.data());
  }
  
  void receiveImg(Mat_<COR>& img) {
    uint32_t nl; receiveUint(nl);
    uint32_t nc; receiveUint(nc);
    img.create(nl,nc);
    receiveBytes(3*img.total(),img.data);
  }
  
  void receiveImgComp(Mat_<COR> &img) {
    vector<BYTE> vb;
    receiveVb(vb);
    img=imdecode(vb,1);
  }    
};

bool testaBytes(BYTE* buf, BYTE b, int n) {
  //Testa se n bytes da memoria buf possuem valor b
  bool igual=true;
  for (unsigned i=0; i<n; i++)
    if (buf[i]!=b) { igual=false; break; }
  return igual;
}

bool testaVb(const vector<BYTE> vb, BYTE b) {
  //Testa se todos os bytes de vb possuem valor b
  bool igual=true;
  for (unsigned i=0; i<vb.size(); i++)
    if (vb[i]!=b) { igual=false; break; }
  return igual;
}





