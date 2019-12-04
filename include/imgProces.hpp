#include <cekeikon.h>
#include <iostream>
#include <vector>

int melhor_template_nnorm(
    int ultimo_tamanho, Mat_<FLT> imagem_referencia,
    Mat_<FLT> imagem_original) { // Acha o tamanho ideal da imagem de referencia

  int tamanho = 0;           // variavel que definira tamanho do template
  int minimo = 5;            // minimo tamanho a percorrer
  int maximo = 85;           // maximo tamanho a percorrer
  float max_value_aux = 0.0; // Usado para comparar qual a maior correlacao com
                             // a imagem e o template

  if (ultimo_tamanho < 15) {
    minimo = 5; // Para imagens pequenas x<=100
    maximo = 85;
  }

#pragma omp parallel for
  for (int i = minimo; i < maximo;
       i = i + 3) { // Para cada um dos tamanhos ve qual a melhor correlacao
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

void achaTemplate(Mat_<FLT> img, Mat_<FLT> imgTemplate, int tamanhoTemplate,
                  int metodo, double &minVal, double &maxVal, Point &minLoc,
                  Point &maxLoc) {
  Mat_<FLT> imgTemplateTemp;
  Mat_<FLT> resultado;

  resize(imgTemplate, imgTemplateTemp, Size(tamanhoTemplate, tamanhoTemplate),
         0, 0, INTER_AREA);
  imgTemplateTemp = somaAbsDois(dcReject(imgTemplateTemp));
  matchTemplate(img, imgTemplateTemp, resultado, metodo);
  minMaxLoc(resultado, &minVal, &maxVal, &minLoc, &maxLoc);
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

void reconheciDigito(MNIST mnist, int tamanhoTemplate, Mat_<FLT> original_flt,
                     Point maxLoc, int &resultadoReconhecimento,
                     Mat_<FLT> &digitoSemBordaBin,
                     Mat_<COR> &digitoSemBordaBinMaior,
                     Mat_<COR> &digitoOriginalCorMaior, Mat_<FLT> &qx2) {
  int corte = tamanhoTemplate * 0.4;
  int borda = tamanhoTemplate * (1 - 0.40);
  const int digitoTamanho = 100;

  Mat_<FLT> digitoOriginal(corte, corte);
  Mat_<FLT> digitoOriginalMaior(digitoTamanho, digitoTamanho, 0.0);
  Mat_<FLT> digitoSemBordaBinMaiorFLT(digitoTamanho, digitoTamanho, 0.0);
  Mat_<FLT> digitoMnist;

  for (int l = 0; l < corte; l++) {
    for (int c = 0; c < corte; c++) {
      digitoOriginal(l, c) =
          original_flt(maxLoc.y + l + borda / 2, maxLoc.x + c + borda / 2);
    }
  }

  Mat_<FLT> kerRealceBorda =
      (Mat_<FLT>(3, 3) << -1, -1, -1, -1, 18, -1, -1, -1, -1);
  kerRealceBorda = (1.0 / 10.0) * kerRealceBorda;

  resize(digitoOriginal, digitoMnist, Size(mnist.nlado, mnist.nlado), 0, 0,
         INTER_AREA);

  int esq = digitoMnist.cols, dir = 0, cima = digitoMnist.rows, baixo = 0;
  bool localizou;
  for (int l = 0; l < digitoMnist.rows; l++) {
    for (int c = 0; c < digitoMnist.cols; c++) {
      if (digitoMnist(l, c) < 0.4) {
        digitoMnist(l, c) = 0;
      } else {
        digitoMnist(l, c) = 1;
      }

      if (digitoMnist(l, c) != 1) {
        if (c < esq)
          esq = c;
        if (dir < c)
          dir = c;
        if (l < cima)
          cima = l;
        if (baixo < l)
          baixo = l;
      }
    }
  }
  if (!(esq < dir && cima < baixo)) { // erro na localizacao
    localizou = false;
    digitoSemBordaBin.create(mnist.nlado, mnist.nlado);
    digitoSemBordaBin.setTo(0.0);
  } else {
    localizou = true;
    Mat_<FLT> roi(digitoMnist,
                  Rect(esq, cima, dir - esq + 1, baixo - cima + 1));
    resize(roi, digitoSemBordaBin, Size(mnist.nlado, mnist.nlado), 0, 0,
           INTER_AREA);
  }

  for (int l = 0; l < digitoSemBordaBin.rows; l++) {
    for (int c = 0; c < digitoSemBordaBin.cols; c++) {
      qx2(0, l * digitoSemBordaBin.cols + c) = digitoSemBordaBin(l, c);
    }
  }

  resize(digitoSemBordaBin, digitoSemBordaBinMaiorFLT,
         Size(digitoTamanho, digitoTamanho), 0, 0, INTER_AREA);
  resize(digitoOriginal, digitoOriginalMaior,
         Size(digitoTamanho, digitoTamanho), 0, 0, INTER_AREA);
  converte(digitoSemBordaBinMaiorFLT, digitoSemBordaBinMaior);
  converte(digitoOriginalMaior, digitoOriginalCorMaior);
}