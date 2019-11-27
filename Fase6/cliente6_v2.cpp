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
  Mat_<FLT> imgTemplate;
  Mat_<COR> original;
  Mat_<FLT> original_flt;
  Mat_<COR> imagem(altura, largura);
  Mat_<FLT> saida_flt;
  Mat_<COR> imgRecebida;

  const int digitoTamanho = 100;

  Mat_<FLT> digito_ajustado;
  Mat_<COR> digito_3b;
  Mat_<COR> digito_3b_maior(digitoTamanho, digitoTamanho, COR(0, 0, 0));
  Mat_<FLT> digito_14;
  Mat_<FLT> digito_maior;
  Mat_<FLT> digito_tratado;

  Mat_<COR> saida;
  int resultadoReconhecimento;

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

  int novo_tamanho = 0;
  const int tamanhoLimiar = 66;
  uint comando = 5;

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
  cout << "teste1 " << matchLoc.x << endl;

  MNIST mnist(14, true, true);
  mnist.le("/home/erik/Documentos/GitHub/PSI3422/Fase5/mnist");
  flann::Index ind(mnist.ax, flann::KDTreeIndexParams(4));

  do {
    client.sendUint(estado);            // Envia estado do mouse
    client.receiveImgComp(imgRecebida); // Recebe imagem compactada
    cout << "Rebebi imagem" << endl;
    client.sendUint(1); // Envia confirmação de recebimento de imagem
    ch = waitKey(5);
    ch = ch % 256;
    if (ch == 27)          // Se esc foi apertado
      client.sendUint(27); // envia 27
    else
      client.sendUint(26); // senao envia 26

    Mat_<FLT> digitoSemBordaBin(14, 14, 0.5);
    Mat_<COR> digitoSemBordaBinMaior(200, 200, COR(0, 0, 0));
    Mat_<COR> digitoOriginalCorMaior(200, 200, COR(0, 0, 0));
    // printMat2(digitoSemBordaBinMaior);

    le(imgTemplate, "quadrado.png");
    original = imgRecebida.clone();
    flip(original, original, -1);
    converte(original, original_flt);
    imgTemplate = trataModelo(imgTemplate, 1.0);
    novo_tamanho = melhor_template_nnorm(0, imgTemplate, original_flt);
    cout << "teste imagem" << endl;
    achaTemplate(original_flt, imgTemplate, novo_tamanho, CV_TM_CCORR, minVal,
                 maxVal, minLoc, maxLoc);
    cout << "teste imagem2" << endl;
    if (maxVal > maxValLimiar) {
      achaTemplate(original_flt, imgTemplate, novo_tamanho, CV_TM_CCOEFF_NORMED,
                   minVal_norm, maxVal_norm, minLoc_norm, maxLoc_norm);

      if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) +
           (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 400 &&
          maxVal_norm > maxValNormLimiar) {
        if (maxVal > maxVal_norm) {
          matchLoc = maxLoc;
          desenha_retangulo(
              novo_tamanho, original, imgTemplate,
              matchLoc); // Funcao que desenha retangulo na imagemd e saida

          cout << "maxVal: " << maxVal << endl;
          cout << "maxValNorm: " << maxVal_norm << endl;
          cout << "entrei no if" << endl;
          putText(
              original_flt, to_string(resultadoReconhecimento),
              Point(maxLoc.x + novo_tamanho / 2, maxLoc.y + novo_tamanho / 2),
              FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
        } else {
          matchLoc = maxLoc_norm;
        }
      } else {
        cout << "entrei no else1" << endl;
        for (int c = 0; c < digitoSemBordaBin.cols; c++) {
          for (int l = 0; l < digitoSemBordaBin.rows; l++) {
            digitoSemBordaBin(l, c) = 0;
          }
        }
        for (int c = 0; c < digitoSemBordaBinMaior.cols; c++) {
          for (int l = 0; l < digitoSemBordaBinMaior.rows; l++) {
            digitoSemBordaBinMaior(l, c) = COR(0, 0, 0);
          }
        }
        for (int c = 0; c < digitoOriginalCorMaior.cols; c++) {
          for (int l = 0; l < digitoOriginalCorMaior.rows; l++) {
            digitoOriginalCorMaior(l, c) = COR(0, 0, 0);
          }
        }
      }
    } else {
      cout << "entrei no else2" << endl;
      for (int c = 0; c < digitoSemBordaBin.cols; c++) {
        for (int l = 0; l < digitoSemBordaBin.rows; l++) {
          digitoSemBordaBin(l, c) = 0;
        }
      }
      for (int c = 0; c < digitoSemBordaBinMaior.cols; c++) {
        for (int l = 0; l < digitoSemBordaBinMaior.rows; l++) {
          digitoSemBordaBinMaior(l, c) = 0;
        }
      }
      for (int c = 0; c < digitoOriginalCorMaior.cols; c++) {
        for (int l = 0; l < digitoOriginalCorMaior.rows; l++) {
          digitoOriginalCorMaior(l, c) = 0;
        }
      }
    }
    cout << "maxloc1111: " << maxLoc << endl;
    cout << "novo tamanho: " << novo_tamanho << endl;
    resultadoReconhecimento = reconheciDigito(
        mnist, ind, novo_tamanho, original_flt, maxLoc, digitoSemBordaBin,
        digitoSemBordaBinMaior, digitoOriginalCorMaior);

    // printMat(digitoSemBordaBin);
    // printMat2(digitoSemBordaBinMaior);
    // printMat2(digitoOriginalCorMaior);

    if (video)
      vo << saida;

    // saida = grudaV(digitoSemBordaBinMaior, digitoOriginalCorMaior);
    // saida = grudaH(original, saida);
    imshow("janela", original);
    waitKey(5);
    cout << "tamanho: " << novo_tamanho << endl;
    cout << "matchLoc.x: " << matchLoc.x << endl;

    int posicaoX = matchLoc.x + novo_tamanho / 2;
    float posicaoX_norm = (float)posicaoX / largura;
    uint pwm = 70;
    /* if (novo_tamanho > tamanhoLimiar ||
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
     }*/
    client.sendUint(comando);
    client.sendUint(pwm);
    // cout << "tamanho: " << novo_tamanho << endl;
    // cout << "comando: " << comando << endl;
    // cout << "maxval: " << maxVal << endl;
    // cout << "maxVal_norm: " << maxVal_norm << endl;
    // cout << "posicaoX: " << posicaoX << " posicaox norm: " << posicaoX_norm
    //      << endl;
    // cout << "pwm: " << pwm << endl;

    comando = 5;
    maxVal = 0;
    maxVal_norm = 0;

    // matchLoc.x = largura / 2;
    // matchLoc.y = altura / 2;

  } while (ch != 27);
}