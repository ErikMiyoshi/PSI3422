#include <iostream>
#include <string>

typedef uint8_t BYTE;
using namespace std;

class SERVER {
   private:
    /* data */
   public:
    SERVER(/* args */);
    ~SERVER();
    void waitConnection();
    void sendBytes(int nBytesToSend, BYTE *buf);
    void receiveBytes(int nBytesToReceive, BYTE *buf);
};

SERVER::SERVER(/* args */) {}

SERVER::~SERVER() {}

void SERVER::sendBytes(int nBytesToSend, BYTE *buf) {
    // if (send(new_fd, buf, strlen(buf)+1, 0) == -1) perror("send");
}

void SERVER::receiveBytes(int nBytesToReceive, BYTE *buf) {
    // if (recv(new_fd, buf, MAXDATASIZE, 0) == -1) perror("recv");
}

class CLIENT {
   private:
    /* data */
   public:
    CLIENT(string endereco);
    ~CLIENT();

    string endereco;

    void sendBytes(int nBytesToSend, BYTE *buf);
    void receiveBytes(int nBytesToReceive, BYTE *buf);
};

CLIENT::CLIENT(string endereco) { this->endereco = endereco; }

CLIENT::~CLIENT() {}

void CLIENT::sendBytes(int nBytesToSend, BYTE *buf) {}

void CLIENT::receiveBytes(int nBytesToReceive, BYTE *buf) {}

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
