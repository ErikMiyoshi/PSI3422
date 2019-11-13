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