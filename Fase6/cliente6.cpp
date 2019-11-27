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
  const int tamanhoLimiar = 66;
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

    int corte = novo_tamanho * 0.4;
    int borda = novo_tamanho * (1 - 0.40);
    cout << corte << endl;
    Mat_<FLT> digito(corte, corte);
    Mat_<FLT> digito_maior;
    Mat_<FLT> digito_maior_bbox;
    Mat_<FLT> digito_14;
    Mat_<COR> digito_14_3b;
    Mat_<FLT> digito_inv(corte, corte);
    Mat_<FLT> digito_tratado;
    Mat_<FLT> qx2(1, 14 * 14);
    Mat_<FLT> qx2_bbox;

    for (int l = 0; l < corte; l++) {
      for (int c = 0; c < corte; c++) {
        digito(l, c) =
            original_flt(maxLoc.y + l + borda / 2, maxLoc.x + c + borda / 2);
      }
    }
    resize(digito, digito_14, Size(14, 14), 0, 0, INTER_AREA);
    converte(digito_14, digito_14_3b);
    int esq = digito_14.cols, dir = 0, cima = digito_14.rows, baixo = 0;
    // cout << "esq: " << esq << " dir: " << dir << " cima: " << cima
    //     << " baixo: " << baixo << endl;
    bool localizou;
    // cout << " " << digito_14.cols << " (" << digito_14.rows << ") " << endl;
    for (int l = 0; l < digito_14.rows; l++) {
      for (int c = 0; c < digito_14.cols; c++) {
        if (digito_14(l, c) < 0.4) {
          digito_14(l, c) = 0;
          // qx2(0, l * digito_14.cols + c) = 0.0;
        } else {
          digito_14(l, c) = 1;
          // qx2(0, l * digito_14.cols + c) = 1.0;
        }
        // cout << "[l,c]: " << l << "," << c << " " << digito_14(l, c) << " "
        //     << endl;

        if (digito_14(l, c) != 1) {
          if (c < esq)
            esq = c;
          if (dir < c)
            dir = c;
          if (l < cima)
            cima = l;
          if (baixo < l)
            baixo = l;
        }
        // cout << "esq: " << esq << " dir: " << dir << " cima: " << cima
        //     << " baixo: " << baixo << endl;
      }
    }

    Mat_<FLT> d;
    Mat_<FLT> d_maior;
    Mat_<COR> d_maior_cor;
    if (!(esq < dir && cima < baixo)) { // erro na localizacao
      localizou = false;
      d.create(14, 14);
      d.setTo(128);
    } else {
      localizou = true;
      Mat_<FLT> roi(digito_14,
                    Rect(esq, cima, dir - esq + 1, baixo - cima + 1));
      // imshow("abc", roi);
      // cv::waitKey(1);
      resize(roi, d, Size(14, 14), 0, 0, INTER_AREA);
    }

    for (int l = 0; l < d.rows; l++) {
      for (int c = 0; c < d.cols; c++) {
        qx2(0, l * digito_14.cols + c) = d(l, c);
      }
    }
    // imshow("abc", digito_14);
    // waitKey(500);
    // qx2_bbox = mnist.bbox(digito_14);
    // cout << "cols: " << qx2.cols << "rows: " << qx2.rows << endl;
    // cout << "cols: " << qx2_bbox.cols << "rows: " << qx2_bbox.rows << endl;
    // converte(digito_14_3b, qx2);
    int resultadoReconhecimento;
    vector<int> indices(1);
    vector<float> dists(1);
    ind.knnSearch(qx2, indices, dists, 1);
    resultadoReconhecimento = mnist.ay(indices[0]);
    cout << "reconheci: " << resultadoReconhecimento << endl;
    resize(d, digito_maior, Size(200, 200), 0, 0, INTER_AREA);
    // resize(qx2_bbox, digito_maior_bbox, Size(200, 200), 0, 0, INTER_AREA);
    Mat_<COR> saida;
    // putTxt(saida, to_string(resultadoReconhecimento), maxLoc.x, maxLoc.y, );
    Mat_<COR> digito_maior_cor;
    converte(digito_maior, digito_maior_cor);
    // saida = grudaH(original, digito_maior_cor);

    if (maxVal > 0.2 && maxVal_norm > 0.5) {
      putText(saida, to_string(resultadoReconhecimento),
              Point(maxLoc.x + novo_tamanho / 2, maxLoc.y + novo_tamanho / 2),
              FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
    } else {
      for (int c = 0; c < d_maior.cols; c++) {
        for (int l = 0; l < d_maior.rows; l++) {
          d(l, c) = 0;
        }
      }
    }
    resize(d, d_maior, Size(200, 200), 0, 0, INTER_AREA);

    converte(digito, digito_3b_maior);
    converte(d_maior, d_maior_cor);
    resize(digito_3b_maior, digito_3b_maior, Size(digitoTamanho, digitoTamanho),
           0, 0, INTER_AREA);
    resize(d_maior_cor, d_maior_cor, Size(digitoTamanho, digitoTamanho), 0, 0,
           INTER_AREA);
    saida = grudaV(d_maior_cor, digito_3b_maior);
    saida = grudaH(original, saida);

    if (video)
      vo << saida;
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
    cout << "tamanho: " << novo_tamanho << endl;
    cout << "comando: " << comando << endl;
    cout << "maxval: " << maxVal << endl;
    cout << "maxVal_norm: " << maxVal_norm << endl;
    cout << "posicaoX: " << posicaoX << " posicaox norm: " << posicaoX_norm
         << endl;
    cout << "pwm: " << pwm << endl;

    comando = 5;
    maxVal = 0;
    maxVal_norm = 0;

    // matchLoc.x = largura / 2;
    // matchLoc.y = altura / 2;

  } while (ch != 27);
}