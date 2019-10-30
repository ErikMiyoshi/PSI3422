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
  Mat_<FLT> imagem_referencia_temp;
  Mat_<FLT> resultado;

  double minVal;
  double maxVal;
  Point minLoc;
  Point maxLoc;
  Point matchLoc;

  if (ultimo_tamanho < 15) {
    minimo = 15; // Para imagens pequenas x<=100
    maximo = 60;
  } else {
    minimo = ultimo_tamanho - 10;
    maximo = ultimo_tamanho + 10;
  }

  for (int i = minimo; i < maximo;
       i = i + 5) { // Para cada um dos tamanhos ve qual a melhor correlacao
    // cout << " i = " << i;
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
  VideoWriter vo(nome_saida, CV_FOURCC('X', 'V', 'I', 'D'), fps, Size(nc, nl));

  cout << "Numero de frames no video especificado: " << frames << endl;

  le(imgTemplate, argv[2]);

  for (int i = 0; i < frames; i++) {
    cout << "Frame: " << i << endl;

    vi >> original;
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

    if (maxVal > 0.18) {
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
          maxVal_norm > 0.18) {
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
  }
  impTempo(t1);
}
