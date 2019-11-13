#include <cekeikon.h>
#include <iostream>
#include <vector>

#include "CLIENT.hpp"
#include "imgProces.hpp"
#include "interface.hpp"
#include "projeto.hpp"

#define altura 240
#define largura 320

using namespace std;

int estado;

void on_mouse(int event, int c, int l, int flags, void *userdata) {
  // std::cout << "(l,c) " << l << " " << c << std::endl;
  if (event == EVENT_LBUTTONDOWN) {
    estado = localizaEstado(l, c, altura, largura);
  } else if (event == EVENT_LBUTTONUP) {
    estado = 0;
  }
  // std::cout << "estado: " << estado << std::endl;
}

int main(int argc, char *argv[]) {
  Mat_<FLT> imgTemplate;
  Mat_<COR> original;
  Mat_<FLT> original_flt;
  Mat_<COR> imagem(altura, largura);
  Mat_<COR> saida;
  Mat_<COR> imgRecebida;

  COR cinza(128, 128, 128);
  COR vermelho(0, 0, 255);

  double minVal;
  double maxVal;
  const double maxValLimiar = 0.2;
  Point minLoc;
  Point maxLoc;
  Point matchLoc;

  double minVal_norm;
  double maxVal_norm;
  const double maxValNormLimiar = 0.7;
  Point minLoc_norm;
  Point maxLoc_norm;
  Point matchLoc_norm;

  int novo_tamanho;
  const int tamanhoLimiar = 53;
  uint comando;

  if (argc != 2)
    perror("client6 servidorIpAddr\n");

  CLIENT client(argv[1]);
  interface GUI;
  TimePoint t1 = timePoint();

  bool video = false;
  string nome = "";

  if (argc == 3) {
    video = true;
    nome = argv[2];
  }

  VideoWriter vo(nome, CV_FOURCC('X', 'V', 'I', 'D'), 20,
                 Size(largura, altura));

  int ch;

  namedWindow("janela", WINDOW_AUTOSIZE);
  setMouseCallback("janela", on_mouse);

  do {
    client.sendUint(estado);            // Envia estado do mouse
    client.receiveImgComp(imgRecebida); // Recebe imagem compactada

    client.sendUint(1); // Envia confirmação de recebimento de imagem
    ch = waitKey(25);
    ch = ch % 256;
    if (ch == 27)          // Se esc foi apertado
      client.sendUint(27); // envia 27
    else
      client.sendUint(26); // senao envia 26

    le(imgTemplate, "quadrado.png");
    original = imgRecebida.clone();
    flip(original, original, -1);
    converte(original, original_flt);

    imgTemplate = trataModelo(imgTemplate, 0.9);
    novo_tamanho = melhor_template_nnorm(0, imgTemplate, original_flt);

    achaTemplate(original, imgTemplate, novo_tamanho, CV_TM_CCORR, minVal,
                 maxVal, minLoc, maxLoc);

    if (maxVal > maxValLimiar) {
      achaTemplate(original, imgTemplate, novo_tamanho, CV_TM_CCOEFF_NORMED,
                   minVal_norm, maxVal_norm, minLoc_norm, maxLoc_norm);

      if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) +
           (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 400 &&
          maxVal_norm > maxValNormLimiar) {
        if (maxVal > maxVal_norm) {
          matchLoc = maxLoc;
        } else {
          matchLoc = maxLoc_norm;
        }
        desenha_retangulo(
            novo_tamanho, original, imgTemplate,
            matchLoc); // Funcao que desenha retangulo na imagemd e saida
      }
    }

    if (video)
      vo << original;

    saida = gruda2Img(imagem, original);
    saida = GUI.pintaBotao(saida, estado, altura, largura);

    imshow("janela", saida);

    // TimePoint t2 = timePoint();
    // double t = timeSpan(t1, t2);
    // impTempo(t1);
    // cout << "tamanho: " << novo_tamanho << endl;
    // cout << "matchLoc.x: " << matchLoc.x << endl;
    int posicaoX = matchLoc.x + novo_tamanho / 2;
    uint pwm = 100;
    if (novo_tamanho > tamanhoLimiar ||
        (maxVal < maxValLimiar && maxVal_norm < maxValNormLimiar)) {
      comando = 5;
    } else if (posicaoX < largura / 3) {
      comando = 1;
      pwm = largura / 2 - posicaoX;
    } else if (posicaoX > 2 * largura / 3) {
      comando = 7;
      pwm = posicaoX - largura / 2;
    } else if (posicaoX > largura / 3 && posicaoX < 2 * largura / 3) {
      comando = 4;
    } else {
      comando = 5;
    }
    client.sendUint(comando);
    client.sendUint(pwm);
    // cout << "comando: " << comando << endl;
    // cout << "maxval: " << maxVal << endl;
    // cout << "maxVal_norm: " << maxVal_norm << endl;
    comando = 5;
    maxVal = 0;
    maxVal_norm = 0;

    // matchLoc.x = largura / 2;
    // matchLoc.y = altura / 2;
  } while (ch != 27);
}