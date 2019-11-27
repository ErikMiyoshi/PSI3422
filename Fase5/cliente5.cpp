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
  Mat_<FLT> saida_flt;
  Mat_<COR> imgRecebida;

  const int digitoTamanho = 100;

  Mat_<FLT> digito_ajustado;
  Mat_<COR> digito_3b;
  Mat_<COR> digito_3b_maior(digitoTamanho, digitoTamanho, COR(0, 0, 0));
  Mat_<FLT> digito_14;
  Mat_<FLT> digito_maior;
  Mat_<FLT> digito_tratado;

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
  const double maxValNormLimiar = 0.5;
  Point minLoc_norm;
  Point maxLoc_norm;
  Point matchLoc_norm;

  int novo_tamanho;
  const int tamanhoLimiar = 64;
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
  cout << "teste1 " << matchLoc.x << endl;

  MNIST mnist(14, true, true);
  mnist.le("/home/erik/Documentos/GitHub/PSI3422/Fase5/mnist");
  flann::Index ind(mnist.ax, flann::KDTreeIndexParams(4));

  do {
    TimePoint t1 = timePoint();
    client.sendUint(estado);            // Envia estado do mouse
    client.receiveImgComp(imgRecebida); // Recebe imagem compactada

    /*vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    imwrite("teste.jpg", imgRecebida, compression_params);
    // imshow("janela", imgRecebida);*/
    client.sendUint(1);         // Envia confirmação de recebimento de imagem
    TimePoint t2 = timePoint(); // Tempo do inicio até enviar a confirmacao
    ch = waitKey(5);
    ch = ch % 256;
    if (ch == 27)          // Se esc foi apertado
      client.sendUint(27); // envia 27
    else
      client.sendUint(26); // senao envia 26
    TimePoint t3 = timePoint();
    le(imgTemplate, "quadrado.png");
    original = imgRecebida.clone();
    flip(original, original, -1);
    converte(original, original_flt);
    imgTemplate = trataModelo(imgTemplate, 1.0);
    novo_tamanho = melhor_template_nnorm(0, imgTemplate, original_flt);

    achaTemplate(original_flt, imgTemplate, novo_tamanho, CV_TM_CCORR, minVal,
                 maxVal, minLoc, maxLoc);
    TimePoint t4 =
        timePoint(); // t3 a t4 tempo para achar o tamanho certo do template
    if (maxVal > maxValLimiar) {
      achaTemplate(original_flt, imgTemplate, novo_tamanho, CV_TM_CCOEFF_NORMED,
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
    int corte = novo_tamanho * 0.5;
    int borda = novo_tamanho * (1 - 0.5);
    Mat_<FLT> digito(corte, corte, 0.0);

    if (maxVal > maxValLimiar && maxVal_norm > maxValNormLimiar) {
      for (int l = 0; l < corte; l++) {
        for (int c = 0; c < corte; c++) {
          digito(l, c) =
              original_flt(maxLoc.y + l + borda / 2, maxLoc.x + c + borda / 2);
        }
      }
      Mat_<COR> digito_3b_14(14, 14);
      converte(digito, digito_3b_14);
      resize(digito, digito_14, Size(14, 14), 0, 0, INTER_AREA);
      Mat_<FLT> qx2(1, 14 * 14);
      converte(digito_3b_14, qx2);

      vector<int> indices(1);
      vector<float> dists(1);
      ind.knnSearch(qx2, indices, dists, 1);
      mnist.qp(1) = mnist.ay(indices[0]);
      cout << "qp: " << mnist.qp(1) << endl;
    } else {
      for (int l = 0; l < digito_3b_maior.rows; l++) {
        for (int c = 0; c < digito_3b_maior.cols; c++) {
          digito_3b_maior(l, c) = 0;
        }
      }
      for (int l = 0; l < digito.rows; l++) {
        for (int c = 0; c < digito.cols; c++) {
          digito(l, c) = 0.0;
        }
      }
    }
    TimePoint t5 = timePoint(); // t4 a t5 - tempo até fazer o maching norm
    if (video)
      vo << original;

    converte(digito, digito_3b_maior);
    resize(digito_3b_maior, digito_3b_maior, Size(digitoTamanho, digitoTamanho),
           0, 0, INTER_AREA);
    saida = grudaH(original, digito_3b_maior);
    // saida = grudaH(saida, digito_3b);
    // saida = GUI.pintaBotao(saida, estado, altura, largura);
    imshow("janela", saida);
    TimePoint t6 = timePoint(); // t5 a t6 - tempo até mostrar a imagem na tela
    // cout << "tamanho: " << novo_tamanho << endl;
    // cout << "matchLoc.x: " << matchLoc.x << endl;

    int posicaoX = matchLoc.x + novo_tamanho / 2;
    float posicaoX_norm = (float)posicaoX / largura;
    uint pwm = 70;
    if (novo_tamanho > tamanhoLimiar ||
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
    /*cout << "tamanho: " << novo_tamanho << endl;
    cout << "comando: " << comando << endl;
    cout << "maxval: " << maxVal << endl;
    cout << "maxVal_norm: " << maxVal_norm << endl;
    cout << "posicaoX: " << posicaoX << " posicaox norm: " << posicaoX_norm
         << endl;
    cout << "pwm: " << pwm << endl;*/

    comando = 5;
    maxVal = 0;
    maxVal_norm = 0;

    // matchLoc.x = largura / 2;
    // matchLoc.y = altura / 2;

  } while (ch != 27);
}