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

bool testaVb(const vector<BYTE> vb, BYTE b) {
  // Testa se todos os bytes de vb possuem valor b
  bool igual = true;
  for (unsigned i = 0; i < vb.size(); i++)
    if (vb[i] != b) {
      igual = false;
      break;
    }
  return igual;
}