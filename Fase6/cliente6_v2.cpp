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
  Mat_<FLT> originalFLT;
  Mat_<COR> imagem(altura, largura);
  Mat_<FLT> saidaFLT;
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
  const double maxValLimiar = 0.15;
  Point minLoc(0, 0);
  Point maxLoc(altura / 2, largura / 2);
  Point matchLoc(0, 0);

  double minVal_norm = 0;
  double maxVal_norm = 0;
  const double maxValNormLimiar = 0.35;
  Point minLoc_norm(0, 0);
  Point maxLoc_norm(0, 0);
  Point matchLoc_norm(0, 0);

  int tamanhoTemplate = 0;
  const int tamanhoLimiar = 49;
  uint comando = 5;
  uint estadoAtual = 1;

  CLIENT client(argv[1]);
  interface GUI;

  VideoWriter vo(nome, CV_FOURCC('m', 'p', '4', 'v'), 10,
                 Size(largura + digitoTamanho, altura));
  // VideoWriter vo(nome, CV_FOURCC('P', 'I', 'M', '1'), 15,
  //               Size(largura, altura));
  int ch;

  namedWindow("janela", WINDOW_NORMAL);
  setMouseCallback("janela", on_mouse);

  MNIST mnist(14, true, true);
  mnist.le("/home/erik/Documentos/GitHub/PSI3422/Fase5/mnist");
  flann::Index ind(mnist.ax, flann::KDTreeIndexParams(4));

  le(imgTemplate, "quadrado.png");
  imgTemplate = trataModelo(imgTemplate, 1.0);

  do {

    client.sendUint(estado); // Envia estado do mouse
    TimePoint t1 = timePoint();
    client.receiveImgComp(imgRecebida); // Recebe imagem compactada
    TimePoint t2 = timePoint();
    client.receiveUint(estadoAtual);
    client.sendUint(1); // Envia confirmação de recebimento de imagem

    ch = (signed char)waitKey(1);
    // cout << "ch: " << ch << endl;
    client.sendUint(ch);
    TimePoint t3 = timePoint();
    original = imgRecebida.clone();
    flip(original, original, -1);
    // flip(original, original, -1);
    converte(original, originalFLT);
    // tamanhoTemplate = 10;
    TimePoint t4 = timePoint();

    tamanhoTemplate = melhor_template_nnorm(0, imgTemplate, originalFLT);

    achaTemplate(originalFLT, imgTemplate, tamanhoTemplate, CV_TM_CCORR, minVal,
                 maxVal, minLoc, maxLoc);
    cout << "maxVal nao Norm: " << maxVal << endl;
    if (maxVal > maxValLimiar) {
      achaTemplate(originalFLT, imgTemplate, tamanhoTemplate,
                   CV_TM_CCOEFF_NORMED, minVal_norm, maxVal_norm, minLoc_norm,
                   maxLoc_norm);
      cout << "Achei Template em: (" << maxLoc.x << "," << maxLoc.y << ") "
           << endl;
      cout << "Achei Template Norm em: (" << maxLoc_norm.x << ","
           << maxLoc_norm.y << ") " << endl;
      if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) +
           (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) <
          100) { // Se o máximo está proximo nos dois testes, então...
        cout << "maxValNorm: " << maxVal_norm << endl;
        if (maxVal_norm > maxValNormLimiar) {
          // matchLoc = maxLoc;
          matchLoc = maxLoc_norm;

          desenha_retangulo(
              tamanhoTemplate, original, imgTemplate,
              matchLoc); // Funcao que desenha retangulo na imagem de saida

          putText(original, "Maxval: " + to_string(maxVal),
                  Point(maxLoc.x, maxLoc.y + tamanhoTemplate + 10),
                  FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0));
          putText(original, "MaxvalNorm: " + to_string(maxVal_norm),
                  Point(maxLoc.x, maxLoc.y + tamanhoTemplate + 20),
                  FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0));
          putText(original, "Tamanho: " + to_string(tamanhoTemplate),
                  Point(maxLoc.x, maxLoc.y + tamanhoTemplate + 30),
                  FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0));

          if (estadoAtual == 12) { // Só reconhece o digito se estiver no estado
                                   // de reconhecimento
            Mat_<FLT> qx2(1, mnist.nlado * mnist.nlado, 0.0);
            reconheciDigito(mnist, tamanhoTemplate, originalFLT, matchLoc,
                            resultadoReconhecimento, digitoSemBordaBin,
                            digitoSemBordaBinMaior, digitoOriginalCorMaior,
                            qx2);
            vector<int> indices(1);
            vector<float> dists(1);
            ind.knnSearch(qx2, indices, dists, 1);
            resultadoReconhecimento = mnist.ay(indices[0]);
            putText(original, to_string(resultadoReconhecimento),
                    Point(maxLoc.x + tamanhoTemplate / 2,
                          maxLoc.y + tamanhoTemplate / 2),
                    FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
          } else {
            resultadoReconhecimento = 10;
          }
        }
      }
    } else if (maxVal < maxValLimiar || maxVal_norm < maxValNormLimiar) {
      digitoSemBordaBin.setTo(0.0);
      digitoSemBordaBinMaior.setTo(Scalar(255, 0, 0));
      digitoOriginalCorMaior.setTo(Scalar(0, 0, 255));
      resultadoReconhecimento = 10;
      tamanhoTemplate = 10;
    }
    client.sendUint(uint(tamanhoTemplate));
    TimePoint t5 = timePoint();
    client.sendUint(uint(resultadoReconhecimento));

    putText(original, "Estado: " + to_string(estadoAtual), Point(15, 15),
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);

    Mat_<COR> saidaTmp;
    saidaTmp = grudaV(digitoSemBordaBinMaior, digitoOriginalCorMaior);
    saida = grudaH(original, saidaTmp);
    imshow("janela", saida);
    waitKey(1);

    if (video)
      vo << saida;

    TimePoint t6 = timePoint();
    uint posicaoX = matchLoc.x + tamanhoTemplate / 2;
    float posicaoX_norm = (float)posicaoX / largura;

    uint pwm = 90;
    /*if (tamanhoTemplate > tamanhoLimiar ||
        (maxVal < maxValLimiar || maxVal_norm < maxValNormLimiar)) {
      comando = 5;*/
    if (posicaoX_norm < 0.30) {
      comando = 1;
      pwm = 5;
      pwm += (1 - posicaoX_norm) * 30;
    } else if (posicaoX_norm > 0.70) {
      comando = 7;
      pwm = 5;
      pwm += posicaoX_norm * 30;
    } else if (posicaoX_norm >= 0.30 && posicaoX_norm <= 0.70) {
      comando = 4;
    } else {
      comando = 5;
    }
    cout << "PosicaoX Norm: " << posicaoX_norm << endl;
    client.sendUint(comando);
    client.sendUint(pwm);
    cout << "comando: " << comando << endl;
    comando = 5;
    TimePoint t7 = timePoint();
    cout << "MaxVal: " << maxVal << endl;
    cout << "MaxValNor: " << maxVal_norm << endl;
    cout << "ch: " << ch << endl;
    cout << "estado Atual: " << estadoAtual << endl;
    cout << "pwm: " << pwm << endl;
    // cout << timeSpan(t1, t2) << " " << timeSpan(t2, t3) << " "
    //     << timeSpan(t3, t4) << " " << timeSpan(t4, t5) << " "
    //     << timeSpan(t5, t6) << " " << timeSpan(t6, t7) << " " << endl;
  } while (ch != 27 || estadoAtual != 20);
}