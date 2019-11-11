#include <cekeikon.h>

class interface {
private:
  /* data */
public:
  interface(/* args */);
  ~interface();

  int setMouse(Mat_<COR> imagem);
  Mat_<COR> pintaBotao(Mat_<COR> imagem, int estado, int altura, int largura);
};

interface::interface() {}

interface::~interface() {}

int interface::setMouse(Mat_<COR> imagem) {
  // return estado;
}

int localizaEstado(int l, int c, int altura, int largura) {
  int estado;
  if (0 <= l && l < altura / 3 && 0 <= c && c < largura / 3)
    estado = 1;
  else if (altura / 3 <= l && l < 2 * altura / 3 && 0 <= c && c < largura / 3)
    estado = 2;
  else if (2 * altura / 3 <= l && l < altura && 0 <= c && c < largura / 3)
    estado = 3;
  else if (0 <= l && l < altura / 3 && largura / 3 <= c && c < 2 * largura / 3)
    estado = 4;
  else if (altura / 3 <= l && l < 2 * altura / 3 && largura / 3 <= c &&
           c < 2 * largura / 3)
    estado = 5;
  else if (2 * altura / 3 <= l && l < altura && largura / 3 <= c &&
           c < 2 * largura / 3)
    estado = 6;
  else if (0 <= l && l < altura / 3 && 2 * largura / 3 <= c && c < largura)
    estado = 7;
  else if (altura / 3 <= l && l < 2 * altura / 3 && 2 * largura / 3 <= c &&
           c < largura)
    estado = 8;
  else if (2 * altura / 3 <= l && l < altura && 2 * largura / 3 <= c &&
           c < largura)
    estado = 9;
  else
    estado = 0;
  return estado;
}

Mat_<COR> interface::pintaBotao(Mat_<COR> imagem, int estado, int altura,
                                int largura) {
  COR cinza(128, 128, 128);
  COR vermelho(0, 0, 255);
  if (estado == 1) {
    for (int l = 0; l < altura / 3; l++)
      for (int c = 0; c < largura / 3; c++)
        imagem(l, c) = vermelho;
  } else if (estado == 2) {
    for (int l = altura / 3; l < 2 * altura / 3; l++)
      for (int c = 0; c < largura / 3; c++)
        imagem(l, c) = vermelho;
  } else if (estado == 3) {
    for (int l = 2 * altura / 3; l < altura; l++)
      for (int c = 0; c < largura / 3; c++)
        imagem(l, c) = vermelho;
  } else if (estado == 4) {
    for (int l = 0; l < altura / 3; l++)
      for (int c = largura / 3; c < 2 * largura / 3; c++)
        imagem(l, c) = vermelho;
  } else if (estado == 5) {
    for (int l = altura / 3; l < 2 * altura / 3; l++)
      for (int c = largura / 3; c < 2 * largura / 3; c++)
        imagem(l, c) = vermelho;
  } else if (estado == 6) {
    for (int l = 2 * altura / 3; l < altura; l++)
      for (int c = largura / 3; c < 2 * largura / 3; c++)
        imagem(l, c) = vermelho;
  } else if (estado == 7) {
    for (int l = 0; l < altura / 3; l++)
      for (int c = 2 * largura / 3; c < largura; c++)
        imagem(l, c) = vermelho;
  } else if (estado == 8) {
    for (int l = altura / 3; l < 2 * altura / 3; l++)
      for (int c = 2 * largura / 3; c < largura; c++)
        imagem(l, c) = vermelho;
  } else if (estado == 9) {
    for (int l = 2 * altura / 3; l < altura; l++)
      for (int c = 2 * largura / 3; c < largura; c++)
        imagem(l, c) = vermelho;
  }
  return imagem;
}

// Gruda imagens de mesmo tamanho
Mat_<COR> gruda2Img(Mat_<COR> imagem1, Mat_<COR> imagem2) {
  COR cinza(128, 128, 128);
  Mat_<COR> saida(max(imagem1.rows, imagem2.rows), imagem1.cols + imagem2.cols,
                  cinza);
  saida.setTo(cinza);
  int offset = 0;
  int maior = 0;
  if (imagem1.rows < imagem1.rows || imagem1.rows > imagem2.rows) {
    offset = (imagem1.rows - imagem2.rows);
  }
  // cout << offset << endl;
  for (int l = 0; l < saida.rows; l++) {
    for (int c = 0; c < saida.cols; c++) {
      if (c > imagem1.cols)
        saida(l + offset / 2, c) = imagem2(l, c - imagem1.cols);
      else
        saida(l, c) = imagem1(l, c);
    }
  }

  return saida;
}