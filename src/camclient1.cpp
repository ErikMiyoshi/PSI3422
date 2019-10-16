// camclient1.cpp
#include "CLIENT.hpp"
#include "projeto.hpp"
#include <cekeikon.h>
int main(int argc, char *argv[]) {
  if (argc != 2)
    perror("client6 servidorIpAddr\n");
  CLIENT client(argv[1]);
  Mat_<COR> a;
  int ch;
  namedWindow("janela");
  do {
    client.receiveImg(a);
    imshow("janela", a);
    client.sendUint(1); // Envia confirmação de recebimento de imagem
    ch = waitKey(25);
    ch = ch % 256;
    if (ch == 27)          // Se esc foi apertado
      client.sendUint(27); // envia 27
    else
      client.sendUint(26); // senao envia 26
  } while (ch != 27);
}