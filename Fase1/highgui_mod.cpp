// highgui.cpp
#include <cekeikon.h>
int estado = 0; // 0=nao_apertado, 1=apertou_botao_1 2=apertou_botao_2
void on_mouse(int event, int c, int l, int flags, void *userdata) {
  if (event == EVENT_LBUTTONDOWN) {
    if (0 <= l && l < 80 && 0 <= c && c < 40)
      estado = 1;
    else if (80 <= l && l < 160 && 0 <= c && c < 40)
      estado = 2;
    else if (120 <= l && l < 240 && 0 <= c && c < 40)
      estado = 3;
    else if (0 <= l && l < 80 && 40 <= c && c < 80)
      estado = 4;
    else if (80 <= l && l < 160 && 40 <= c && c < 80)
      estado = 5;
    else if (120 <= l && l < 240 && 40 <= c && c < 80)
      estado = 6;
    else if (0 <= l && l < 80 && 80 <= c && c < 120)
      estado = 7;
    else if (80 <= l && l < 160 && 80 <= c && c < 120)
      estado = 8;
    else if (120 <= l && l < 240 && 80 <= c && c < 120)
      estado = 9;
    else
      estado = 0;
  } else if (event == EVENT_LBUTTONUP) {
    estado = 0;
  }
}
int main() {
  COR cinza(128, 128, 128);
  COR vermelho(0, 0, 255);
  Mat_<COR> imagem(240, 120, cinza);
  namedWindow("janela", WINDOW_NORMAL);
  resizeWindow("janela", 2 * imagem.cols, 2 * imagem.rows);
  setMouseCallback("janela", on_mouse);
  imshow("janela", imagem);
  while (waitKey(1) != 27) { // ESC=27 sai do programa
    imagem.setTo(cinza);
    if (estado == 1) {
      for (int l = 0; l < 80; l++)
        for (int c = 0; c < 40; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 2) {
      for (int l = 80; l < 160; l++)
        for (int c = 0; c < 40; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 3) {
      for (int l = 160; l < 240; l++)
        for (int c = 0; c < 40; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 4) {
      for (int l = 0; l < 80; l++)
        for (int c = 40; c < 80; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 5) {
      for (int l = 80; l < 160; l++)
        for (int c = 40; c < 80; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 6) {
      for (int l = 160; l < 240; l++)
        for (int c = 40; c < 80; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 7) {
      for (int l = 0; l < 80; l++)
        for (int c = 80; c < 120; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 8) {
      for (int l = 80; l < 160; l++)
        for (int c = 80; c < 120; c++)
          imagem(l, c) = vermelho;
    } else if (estado == 9) {
      for (int l = 160; l < 240; l++)
        for (int c = 80; c < 120; c++)
          imagem(l, c) = vermelho;
    }
    imshow("janela", imagem);
  }
}