// camserver2.cpp
#include "CLIENT.hpp"
#include "projeto.hpp"
#include <cekeikon.h>

int main(int argc, char *argv[]) {
  if (argc != 2)
    perror("client6 servidorIpAddr\n");
  CLIENT client(argv[1]);
  Mat_<COR> a;
  bool video = false;
  string nome = "";
  if (argc == 3) {
    video = true;
    nome = argv[2];
  }
  VideoWriter vo(nome, CV_FOURCC('X', 'V', 'I', 'D'), 20, Size(320, 240));
  int ch;
  // namedWindow("janela");
  do {
    client.receiveImgComp(a);

    if (video)
      vo << a;

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