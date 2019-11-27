#include <cekeikon.h>
#include <iostream>
#include <vector>

using namespace std;

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
  // Avisa problemas de erro de sintaxe se o numero de argumentos nao for 2
  // Para imagem inexistente, mensagem amigavel ja eh gerada
  if (argc != 4) {
    cout << " ep1: Detecta placa proibido virar" << endl;
    cout << " sintaxe: fase3 entrada.avi quadrado.png saida.avi" << endl;
    cout << " Erro: Numero de argumentos invalido" << endl;
  }
  TimePoint t1 = timePoint();
  // Abre arquivo especificado pelo usiário
  string file = argv[1];
  VideoCapture vi(file);

  // Pega dados da camera
  float fps = vi.get(CV_CAP_PROP_FPS);
  int nc = vi.get(CV_CAP_PROP_FRAME_WIDTH);
  int nl = vi.get(CV_CAP_PROP_FRAME_HEIGHT);
  int frames = vi.get(CV_CAP_PROP_FRAME_COUNT);
  string nome_saida = argv[3];
  VideoWriter vo(nome_saida, CV_FOURCC('X', 'V', 'I', 'D'), fps,
                 Size(nc + 200, nl));
  cout << "Numero de frames no video especificado: " << frames << endl;

  le(imgTemplate, argv[2]);

  namedWindow("abc", CV_WINDOW_AUTOSIZE);

  MNIST mnist(14, true, true);
  mnist.le("/home/erik/Documentos/GitHub/PSI3422/Fase5/mnist");
  flann::Index ind(mnist.ax, flann::KDTreeIndexParams(4));

  for (int i = 1; i < frames; i++) {
    cout << "Frame: " << i << endl;

    vi >> original;
    converte(original, original_flt);
    imgTemplate = trataModelo(imgTemplate, 1.0);
    novo_tamanho = melhor_template_nnorm(0, imgTemplate, original_flt);

    Mat_<FLT> imgTemplateTemp;
    resize(imgTemplate, imgTemplateTemp, Size(novo_tamanho, novo_tamanho), 0, 0,
           INTER_AREA);
    imgTemplateTemp = somaAbsDois(dcReject(imgTemplateTemp));

    matchTemplate(original_flt, imgTemplateTemp, resultado, CV_TM_CCORR);
    minMaxLoc(resultado, &minVal, &maxVal, &minLoc, &maxLoc);

    // mostra(resultado);
    // cout << "maxVal nao norm: " << maxVal << endl;
    cout << "maxVal nao norm: " << maxVal << endl;
    cout << "tamanho: " << novo_tamanho << endl;
    cout << "matchloc : " << maxLoc << endl;

    if (maxVal > 0.2) {
      resize(imgTemplate, imgTemplateTemp, Size(novo_tamanho, novo_tamanho), 0,
             0, INTER_AREA);
      matchTemplate(original_flt, imgTemplateTemp, resultado_norm,
                    CV_TM_CCOEFF_NORMED);
      minMaxLoc(resultado_norm, &minVal_norm, &maxVal_norm, &minLoc_norm,
                &maxLoc_norm);
      cout << "maxVal norm: " << maxVal_norm << endl;
      cout << "tamanho: " << novo_tamanho << endl;
      cout << "maxLocnorm : " << maxLoc_norm << endl;
      // Se a posicao dos dois templates esta proxima. d^2 = a^2 + b^2
      if (((maxLoc.x - maxLoc_norm.x) * (maxLoc.x - maxLoc_norm.x) +
           (maxLoc.y - maxLoc_norm.y) * (maxLoc.y - maxLoc_norm.y)) < 400 &&
          maxVal_norm > 0.5) {
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
    cout << "matchLoc : " << matchLoc << endl;
    // vo << original;

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
    cout << " " << digito_14.cols << " (" << digito_14.rows << ") " << endl;
    for (int l = 0; l < digito_14.rows; l++) {
      for (int c = 0; c < digito_14.cols; c++) {
        if (digito_14(l, c) < 0.5) {
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

    cout << "localizou: " << localizou << endl;
    // imshow("abc", digito_14);
    // waitKey(500);
    // qx2_bbox = mnist.bbox(digito_14);
    // cout << "cols: " << qx2.cols << "rows: " << qx2.rows << endl;
    // cout << "cols: " << qx2_bbox.cols << "rows: " << qx2_bbox.rows << endl;
    cout << "teste" << endl;
    // converte(digito_14_3b, qx2);
    int resultadoReconhecimento;
    vector<int> indices(1);
    vector<float> dists(1);
    ind.knnSearch(qx2, indices, dists, 1);
    resultadoReconhecimento = mnist.ay(indices[0]);
    cout << "qp: " << resultadoReconhecimento << endl;
    resize(d, digito_maior, Size(200, 200), 0, 0, INTER_AREA);
    // resize(qx2_bbox, digito_maior_bbox, Size(200, 200), 0, 0, INTER_AREA);
    Mat_<COR> saida;
    // putTxt(saida, to_string(resultadoReconhecimento), maxLoc.x, maxLoc.y, );
    Mat_<COR> digito_maior_cor;
    converte(digito_maior, digito_maior_cor);
    saida = grudaH(original, digito_maior_cor);
    vo << saida;
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
    imshow("abc", saida);
    cv::waitKey(1);
  }
  // TimePoint t2 = timePoint();
  // double t = timeSpan(t1, t2);
  impTempo(t1);
}