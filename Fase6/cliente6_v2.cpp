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

void printMat(Mat_<FLT> a) {
  for (int l = 0; l < a.rows; l++) {
    for (int c = 0; c < a.cols; c++) {
      cout << "a(" << l << "," << c << ")" << a(l, c) << endl;
    }
  }
}

void printMat2(Mat_<COR> a) {
  for (int l = 0; l < a.rows; l++) {
    for (int c = 0; c < a.cols; c++) {
      cout << "a(" << l << "," << c << ")" << a(l, c) << endl;
    }
  }
}

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
  if (argc != 2)
    perror("client6 servidorIpAddr\n");

  bool video = false;
  string nome = "";

  if (argc == 3) {
    video = true;
    nome = argv[2];
  }

  const int digitoTamanho = 100;
  const int digitoTamanhoMnist = 14;

  Mat_<FLT> imgTemplate;
  Mat_<COR> original;
  Mat_<FLT> original_flt;
  Mat_<COR> imagem(altura, largura);
  Mat_<FLT> saida_flt;
  Mat_<COR> imgRecebida;

  Mat_<FLT> digito_ajustado;
  Mat_<COR> digito_3b;
  Mat_<COR> digito_3b_maior(digitoTamanho, digitoTamanho, COR(0, 0, 0));
  Mat_<FLT> digito_14;
  Mat_<FLT> digito_maior;
  Mat_<FLT> digito_tratado;

  Mat_<FLT> digitoSemBordaBin(digitoTamanhoMnist, digitoTamanhoMnist, 0.0);
  Mat_<COR> digitoSemBordaBinMaior(digitoTamanho, digitoTamanho, COR(0, 0, 0));
  Mat_<COR> digitoOriginalCorMaior(digitoTamanho, digitoTamanho, COR(0, 0, 0));
  int resultadoReconhecimento;

  Mat_<COR> saida;

  COR cinza(128, 128, 128);
  COR vermelho(0, 0, 255);

  double minVal = 0;
  double maxVal = 0;
  const double maxValLimiar = 0.2;
  Point minLoc(0, 0);
  Point maxLoc(altura / 2, largura / 2);
  Point matchLoc(0, 0);

  double minVal_norm = 0;
  double maxVal_norm = 0;
  const double maxValNormLimiar = 0.5;
  Point minLoc_norm(0, 0);
  Point maxLoc_norm(0, 0);
  Point matchLoc_norm(0, 0);

  int tamanhoTemplate = 0;
  const int tamanhoLimiar = 66;
  uint comando = 5;

  CLIENT client(argv[1]);
  interface GUI;

  VideoWriter vo(nome, CV_FOURCC('X', 'V', 'I', 'D'), 20,
                 Size(largura, altura));

  int ch;

  namedWindow("janela", WINDOW_AUTOSIZE);
  setMouseCallback("janela", on_mouse);

  MNIST mnist(14, true, true);
  mnist.le("/home/erik/Documentos/GitHub/PSI3422/Fase5/mnist");
  flann::Index ind(mnist.ax, flann::KDTreeIndexParams(4));

  le(imgTemplate, "quadrado.png");
  imgTemplate = trataModelo(imgTemplate, 1.0);

  do {
    client.sendUint(estado);            // Envia estado do mouse
    client.receiveImgComp(imgRecebida); // Recebe imagem compactada
    client.sendUint(1); // Envia confirmação de recebimento de imagem

    ch = waitKey(5);
    ch = ch % 256;
    if (ch == 27)          // Se esc foi apertado
      client.sendUint(27); // envia 27
    else
      client.sendUint(26); // senao envia 26

    original = imgRecebida.clone();
    flip(original, original, -1);
    converte(original, original_flt);

    tamanhoTemplate = melhor_template_nnorm(0, imgTemplate, original_flt);

    achaTemplate(original_flt, imgTemplate, tamanhoTemplate, CV_TM_CCORR,
                 minVal, maxVal, minLoc, maxLoc);

    if (maxVal > maxValLimiar) {
      achaTemplate(original_flt, imgTemplate, tamanhoTemplate,
                   CV_TM_CCOEFF_NORMED, minVal_norm, maxVal_norm, minLoc_norm,
                   maxLoc_norm);

      if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) +
           (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) <
          400) { // Se o máximo está proximo nos dois testes, então...
        if (maxVal_norm > maxValNormLimiar) {
          matchLoc = maxLoc_norm;

          desenha_retangulo(
              tamanhoTemplate, original, imgTemplate,
              matchLoc); // Funcao que desenha retangulo na imagem de saida

          Mat_<FLT> qx2(1, mnist.nlado * mnist.nlado, 0.0);
          reconheciDigito(mnist, tamanhoTemplate, original_flt, matchLoc,
                          resultadoReconhecimento, digitoSemBordaBin,
                          digitoSemBordaBinMaior, digitoOriginalCorMaior, qx2);
          vector<int> indices(1);
          vector<float> dists(1);
          ind.knnSearch(qx2, indices, dists, 1);
          resultadoReconhecimento = mnist.ay(indices[0]);

          putText(original, to_string(resultadoReconhecimento),
                  Point(maxLoc.x + tamanhoTemplate / 2,
                        maxLoc.y + tamanhoTemplate / 2),
                  FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
        }
      }
    } else {
      digitoSemBordaBin.setTo(0.0);
      digitoSemBordaBinMaior.setTo(Scalar(255, 0, 0));
      digitoOriginalCorMaior.setTo(Scalar(0, 0, 255));
    }

    Mat_<COR> saidaTmp;
    saidaTmp = grudaV(digitoSemBordaBinMaior, digitoOriginalCorMaior);
    saida = grudaH(original, saidaTmp);
    imshow("janela", saida);
    waitKey(5);

    if (video)
      vo << saida;

    int posicaoX = matchLoc.x + tamanhoTemplate / 2;
    float posicaoX_norm = (float)posicaoX / largura;
    uint pwm = 70;
    if (tamanhoTemplate > tamanhoLimiar ||
        (maxVal < maxValLimiar || maxVal_norm < maxValNormLimiar)) {
      comando = 5;
    } else if (posicaoX < largura / 3) {
      comando = 1;
      pwm += (1 - posicaoX_norm) * 30;
    } else if (posicaoX > 2 * largura / 3) {
      comando = 7;
      pwm += posicaoX_norm * 30;
    } else if (posicaoX > largura / 3 && posicaoX < 2 * largura / 3) {
      comando = 4;
    } else {
      comando = 5;
    }
    client.sendUint(comando);
    client.sendUint(pwm);

    comando = 5;
  } while (ch != 27);
}