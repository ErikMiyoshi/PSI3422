#include <cekeikon.h>
#include <iostream>
#include <vector>

#include "CLIENT.hpp"
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

int melhor_template_nnorm(
    int ultimo_tamanho, Mat_<FLT> imagem_referencia,
    Mat_<FLT> imagem_original) { // Acha o tamanho ideal da imagem de referencia

  int tamanho = 100;         // variavel que definira tamanho do template
  int minimo = 30;           // minimo tamanho a percorrer
  int maximo = 130;          // maximo tamanho a percorrer
  float max_value_aux = 0.0; // Usado para comparar qual a maior correlacao com
                             // a imagem e o template

  if (ultimo_tamanho < 15) {
    minimo = 15; // Para imagens pequenas x<=100
    maximo = 60;
  } else {
    minimo = ultimo_tamanho - 10;
    maximo = ultimo_tamanho + 10;
  }

#pragma omp parallel for
  for (int i = minimo; i < maximo;
       i = i + 5) { // Para cada um dos tamanhos ve qual a melhor correlacao
    // cout << " i = " << i << endl;
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;

    Mat_<FLT> imagem_referencia_temp;
    Mat_<FLT> resultado;

    resize(imagem_referencia, imagem_referencia_temp, Size(i, i), 0, 0,
           INTER_AREA);

    imagem_referencia_temp = somaAbsDois(dcReject(imagem_referencia_temp));

    matchTemplate(imagem_original, imagem_referencia_temp, resultado,
                  CV_TM_CCORR);
    minMaxLoc(resultado, &minVal, &maxVal, &minLoc, &maxLoc);
    // cout << " max Val = " << maxVal;
    // Se a imagem eh pequena a correlacao deve ser bem alta para evitar
    // erros. No caso > 0.75
    if (maxVal > max_value_aux) {
      max_value_aux = maxVal;
      // referencia_escolhida = ref; // Atualiza o valor da amostra escolhida
      tamanho = i;
    }
  }
  // Retorna o melhor tamanho encontrado
  // cout << tamanho;
  return tamanho;
}

Mat_<COR> desenha_retangulo(int tamanho, Mat_<COR> imagem, Mat_<FLT> ref,
                            Point loc) {
  resize(ref, ref, Size(tamanho, tamanho), 0, 0, INTER_AREA);

  // line( imagem, loc + Point(templ.cols/2 , templ.rows/2 + 0.02*templ.rows ),
  //	loc + Point( templ.cols/2 , templ.rows/2 - 0.02*templ.rows ),
  // Scalar(0,255,255), 1, 8);

  // line( imagem, Point( matchLoc.x + templ.cols/2 + 0.02*templ.rows ,
  // matchLoc.y + templ.rows/2 ), 		Point( matchLoc.x + templ.cols/2
  // -
  // 0.02*templ.rows , matchLoc.y + templ.rows/2 ), Scalar(0,255,255), 1, 8);

  rectangle(imagem, loc, Point(loc.x + ref.cols, loc.y + ref.rows),
            Scalar(0, 255, 255), 1.5, 8);
  // cout << "loc: " << loc << endl;
  // cout << "loc + temp: " << Point(loc.x + ref.cols, loc.y + ref.rows) <<
  // endl;
  return imagem;
}

int main(int argc, char *argv[]) {
  Mat_<FLT> imgTemplate;
  Mat_<COR> original;
  Mat_<FLT> original_flt;
  Mat_<FLT> resultado;
  Mat_<FLT> resultado_norm;
  Mat_<COR> imagem(altura, largura);
  Mat_<COR> saida;
  Mat_<COR> imgRecebida;

  COR cinza(128, 128, 128);
  COR vermelho(0, 0, 255);

  double minVal;
  double maxVal;
  Point minLoc;
  Point maxLoc;
  Point matchLoc;

  double minVal_norm;
  double maxVal_norm;
  Point minLoc_norm;
  Point maxLoc_norm;
  Point matchLoc_norm;

  int novo_tamanho;
  uint comando;

  if (argc != 2)
    perror("client6 servidorIpAddr\n");

  CLIENT client(argv[1]);

  bool video = false;
  string nome = "";
  if (argc == 3) {
    video = true;
    nome = argv[2];
  }
  VideoWriter vo(nome, CV_FOURCC('X', 'V', 'I', 'D'), 20,
                 Size(largura, altura));
  int ch;

  interface GUI;

  TimePoint t1 = timePoint();

  namedWindow("janela", WINDOW_AUTOSIZE);
  setMouseCallback("janela", on_mouse);

  do {
    client.sendUint(estado);            // Envia estado do mouse
    client.receiveImgComp(imgRecebida); // Recebe imagem compactada

    if (video)
      vo << imgRecebida;

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

    Mat_<FLT> imgTemplateTemp;
    resize(imgTemplate, imgTemplateTemp, Size(novo_tamanho, novo_tamanho), 0, 0,
           INTER_AREA);
    imgTemplateTemp = somaAbsDois(dcReject(imgTemplateTemp));

    matchTemplate(original_flt, imgTemplateTemp, resultado, CV_TM_CCORR);
    minMaxLoc(resultado, &minVal, &maxVal, &minLoc, &maxLoc);

    // mostra(resultado);
    // cout << "maxVal nao norm: " << maxVal << endl;

    if (maxVal > 0.2) {
      resize(imgTemplate, imgTemplateTemp, Size(novo_tamanho, novo_tamanho), 0,
             0, INTER_AREA);
      matchTemplate(original_flt, imgTemplateTemp, resultado_norm,
                    CV_TM_CCOEFF_NORMED);
      minMaxLoc(resultado_norm, &minVal_norm, &maxVal_norm, &minLoc_norm,
                &maxLoc_norm);
      // cout << "maxVal nao norm: " << maxVal_norm << endl;
      // cout << "tamanho: " << novo_tamanho << endl;
      // cout << "matchloc : " << matchLoc << endl;
      // Se a posicao dos dois templates esta proxima. d^2 = a^2 + b^2
      if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) +
           (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 400 &&
          maxVal_norm > 0.7) {
        if (maxVal > maxVal_norm) {
          matchLoc = maxLoc;
        } else {
          matchLoc = maxLoc_norm;
        }
        desenha_retangulo(
            novo_tamanho, original, imgTemplate,
            matchLoc); // Funcao que desenha retangulo na imagemd e saida
        // cout << maxLoc << " - ";
      }
    }

    vo << original;

    saida = gruda2Img(imagem, original);
    saida = GUI.pintaBotao(saida, estado, altura, largura);

    imshow("janela", saida);

    // TimePoint t2 = timePoint();
    // double t = timeSpan(t1, t2);
    // impTempo(t1);
    cout << "tamanho: " << novo_tamanho << endl;
    cout << "matchLoc.x: " << matchLoc.x << endl;
    if (novo_tamanho > 53 || (maxVal < 0.2 && maxVal_norm < 0.7)) {
      comando = 5;
    } else if (matchLoc.x < largura / 3) {
      comando = 1;
    } else if (matchLoc.x > 2 * largura / 3) {
      comando = 7;
    } else if (matchLoc.x > largura / 3 && matchLoc.x < 2 * largura / 3) {
      comando = 4;
    } else {
      comando = 5;
    }
    client.sendUint(comando);
    cout << "comando: " << comando << endl;
    cout << "maxval: " << maxVal << endl;
    cout << "maxVal_norm: " << maxVal_norm << endl;
    comando = 5;
    maxVal = 0;
    maxVal_norm = 0;

    // matchLoc.x = largura / 2;
    // matchLoc.y = altura / 2;
  } while (ch != 27);
}