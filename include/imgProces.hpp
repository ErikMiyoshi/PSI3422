#include <cekeikon.h>
#include <iostream>
#include <vector>

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
    maximo = 70;
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

int reconheciDigito(MNIST mnist, flann::Index ind, int novo_tamanho,
                    Mat_<FLT> original_flt, Point maxLoc,
                    Mat_<FLT> &digitoSemBordaBin,
                    Mat_<COR> &digitoSemBordaBinMaior,
                    Mat_<COR> &digitoOriginalCorMaior) {

  int corte = novo_tamanho * 0.4;
  int borda = novo_tamanho * (1 - 0.40);
  const int digitoTamanho = 200;

  Mat_<FLT> digito(corte, corte);
  Mat_<FLT> digito_maior;
  Mat_<FLT> digito_maior_bbox;
  Mat_<FLT> digitoSemBordaBinMaiorFLT;
  Mat_<FLT> digito_14;
  Mat_<COR> digito_14_3b;
  Mat_<FLT> digito_inv(corte, corte);
  Mat_<FLT> digito_tratado;
  Mat_<FLT> qx2;
  Mat_<FLT> qx2_bbox;
  Mat_<FLT> d_maior;

  for (int l = 0; l < corte; l++) {
    for (int c = 0; c < corte; c++) {
      digito(l, c) =
          original_flt(maxLoc.y + l + borda / 2, maxLoc.x + c + borda / 2);
    }
  }
  resize(digito, digito_14, Size(14, 14), 0, 0, INTER_AREA);
  converte(digito_14, digito_14_3b);
  int esq = digito_14.cols, dir = 0, cima = digito_14.rows, baixo = 0;
  bool localizou;
  for (int l = 0; l < digito_14.rows; l++) {
    for (int c = 0; c < digito_14.cols; c++) {
      if (digito_14(l, c) < 0.4) {
        digito_14(l, c) = 0;
      } else {
        digito_14(l, c) = 1;
      }

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
    }
  }
  if (!(esq < dir && cima < baixo)) { // erro na localizacao
    localizou = false;
    digitoSemBordaBin.create(14, 14);
    digitoSemBordaBin.setTo(128);
  } else {
    localizou = true;
    Mat_<FLT> roi(digito_14, Rect(esq, cima, dir - esq + 1, baixo - cima + 1));
    resize(roi, digitoSemBordaBin, Size(14, 14), 0, 0, INTER_AREA);
  }
  qx2.create(digitoSemBordaBin.rows * digitoSemBordaBin.cols, 1);
  for (int l = 0; l < digitoSemBordaBin.rows; l++) {
    for (int c = 0; c < digitoSemBordaBin.cols; c++) {
      cout << "dsb(" << l << "," << c << "): " << digitoSemBordaBin(l, c)
           << endl;
      qx2(0, l * digitoSemBordaBin.cols + c) = digitoSemBordaBin(l, c);
      cout << "qxw(" << l << "," << c << "): " << qx2(0, l * digito_14.cols + c)
           << endl;
    }
  }
  int resultadoReconhecimento;
  vector<int> indices(1);
  vector<float> dists(1);
  cout << "size: " << qx2.size() << endl;
  cout << "entrou aqui1" << endl;
  ind.knnSearch(qx2.row(0), indices, dists, 1);
  cout << "entrou aqui2" << endl;
  resultadoReconhecimento = mnist.ay(indices[0]);
  cout << "reconheci: " << resultadoReconhecimento << endl;

  resize(digitoSemBordaBin, digitoSemBordaBinMaiorFLT,
         Size(digitoTamanho, digitoTamanho), 0, 0, INTER_AREA);
  resize(digito, digito_maior, Size(digitoTamanho, digitoTamanho), 0, 0,
         INTER_AREA);
  converte(digitoSemBordaBinMaiorFLT, digitoSemBordaBinMaior);
  converte(digito_maior, digitoOriginalCorMaior);

  // namedWindow("janela", WINDOW_AUTOSIZE);
  // imshow("janela", digitoSemBordaBinMaiorFLT);
  // waitKey(2000);
  return resultadoReconhecimento;
}