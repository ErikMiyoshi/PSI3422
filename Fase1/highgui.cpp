// highgui.cpp
#include <cekeikon.h>
int estado = 0; // 0=nao_apertado, 1=apertou_botao_1 2=apertou_botao_2
void on_mouse(int event, int c, int l, int flags, void *userdata) {
  if (event == EVENT_LBUTTONDOWN) {
    if (0 <= l && l < 100 && 0 <= c && c < 100)
      estado = 1;
    else if (0 <= l && l < 100 && 100 <= c && c < 200)
      estado = 2;
    else
      estado = 0;
  } else if (event == EVENT_LBUTTONUP) {
    estado = 0;
  }
}
int main() {
  COR cinza(128, 128, 128);
  COR vermelho(0, 0, 255);
  Mat_<COR> imagem(100, 200, cinza);
  namedWindow("janela", WINDOW_NORMAL);
  resizeWindow("janela", 2 * imagem.cols, 2 * imagem.rows);
  setMouseCallback("janela", on_mouse);
  imshow("janela", imagem);
  while (waitKey(1) != 27) { // ESC=27 sai do programa
    imagem.setTo(cinza);
    if (estado == 1) {
      for (int l = 0; l < 100; l++)
        for (int c = 0; c < 100; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 2) {
      for (int l = 0; l < 100; l++)
        for (int c = 100; c < 200; c++)
          imagem(l, c) = vermelho;
    }
    imshow("janela", imagem);
  }
}