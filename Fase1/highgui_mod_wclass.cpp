// highgui.cpp
#include "interface.hpp"
#include <cekeikon.h>

#define altura 200
#define largura 200

int estado;

void on_mouse(int event, int c, int l, int flags, void *userdata) {
  std::cout << "(l,c) " << l << " " << c << std::endl;
  if (event == EVENT_LBUTTONDOWN) {
    estado = localizaEstado(l, c, altura, largura);
  } else if (event == EVENT_LBUTTONUP) {
    estado = 0;
  }
  std::cout << "estado: " << estado << std::endl;
}

int main() {
  COR cinza(128, 128, 128);
  COR vermelho(0, 0, 255);
  COR azul(255, 0, 0);
  COR verde(0, 255, 0);

  Mat_<COR> imagem(altura, largura, verde);
  Mat_<COR> imgTeste(altura, largura, azul);
  Mat_<COR> saida;
  interface GUI;

  namedWindow("janela", WINDOW_AUTOSIZE);
  setMouseCallback("janela", on_mouse);
  // imshow("janela", imagem);
  while (waitKey(1) != 27) { // ESC=27 sai do programa
    // imagem.setTo(cinza);
    saida = gruda2Img(imagem, imgTeste);
    saida = GUI.pintaBotao(saida, estado, altura, largura);
    imshow("janela", saida);
  }
}