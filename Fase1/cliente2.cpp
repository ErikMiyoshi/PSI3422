// camserver2.cpp
#include "CLIENT.hpp"
#include "projeto.hpp"
#include <cekeikon.h>

uint estado = 0; // 0=nao_apertado, 1=apertou_botao_1 2=apertou_botao_2

void on_mouse(int event, int c, int l, int flags, void *userdata) {
  if (event == EVENT_LBUTTONDOWN) {
    if (0 <= l && l < 80 && 0 <= c && c < 80)
      estado = 1;
    else if (80 <= l && l < 160 && 0 <= c && c < 80)
      estado = 2;
    else if (120 <= l && l < 240 && 0 <= c && c < 80)
      estado = 3;
    else if (0 <= l && l < 80 && 80 <= c && c < 160)
      estado = 4;
    else if (80 <= l && l < 160 && 80 <= c && c < 160)
      estado = 5;
    else if (120 <= l && l < 240 && 80 <= c && c < 160)
      estado = 6;
    else if (0 <= l && l < 80 && 160 <= c && c < 240)
      estado = 7;
    else if (80 <= l && l < 160 && 160 <= c && c < 240)
      estado = 8;
    else if (120 <= l && l < 240 && 160 <= c && c < 240)
      estado = 9;
    else
      estado = 0;
  } else if (event == EVENT_LBUTTONUP) {
    estado = 0;
  }
}

int main(int argc, char *argv[]) {
  COR cinza(128, 128, 128);
  COR vermelho(0, 0, 255);
  Mat_<COR> imagem(240, 560, cinza); // 240 linha x 320 + 240 colunas = 240x560
  namedWindow("janela");
  // resizeWindow("janela", 2 * imagem.cols, imagem.rows); //Janela vai ter
  // 240x640
  setMouseCallback("janela", on_mouse);
  imshow("janela", imagem);

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
    client.sendUint(estado);
    client.receiveImgComp(a);

    if (video)
      vo << a;

    imagem.setTo(cinza);
    if (estado == 1) {
      for (int l = 0; l < 80; l++)
        for (int c = 0; c < 80; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 2) {
      for (int l = 80; l < 160; l++)
        for (int c = 0; c < 80; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 3) {
      for (int l = 160; l < 240; l++)
        for (int c = 0; c < 80; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 4) {
      for (int l = 0; l < 80; l++)
        for (int c = 80; c < 160; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 5) {
      for (int l = 80; l < 160; l++)
        for (int c = 80; c < 160; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 6) {
      for (int l = 160; l < 240; l++)
        for (int c = 80; c < 160; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 7) {
      for (int l = 0; l < 80; l++)
        for (int c = 160; c < 240; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 8) {
      for (int l = 80; l < 160; l++)
        for (int c = 160; c < 240; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 9) {
      for (int l = 160; l < 240; l++)
        for (int c = 160; c < 240; c++)
          imagem(l, c) = vermelho;
    }

    for (int l = 0; l < imagem.rows; l++) {
      for (int c = 0; c < imagem.cols; c++) {
        if (c > 240)
          imagem(l, c) = a(l, c - 240);
      }
    }
    imshow("janela", imagem);
    client.sendUint(1); // Envia confirmação de recebimento de imagem
    ch = waitKey(25);
    ch = ch % 256;
    if (ch == 27)          // Se esc foi apertado
      client.sendUint(27); // envia 27
    else
      client.sendUint(26); // senao envia 26
  } while (ch != 27);
}
