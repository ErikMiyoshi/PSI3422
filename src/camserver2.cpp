// camserver2.cpp
#include "SERVER.hpp"
#include "projeto.hpp"
#include <cekeikon.h>
int main() {
  SERVER server;
  server.waitConnection();
  VideoCapture vi(0); // 0=default camera
  vi.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  vi.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  if (!vi.isOpened())
    erro("Erro abertura webcam");
  Mat_<COR> a;
  uint resposta = 0, ch;

  namedWindow("janela");
  do {
    vi >> a;
    // cout << "nl: " << a.rows << " nc: " << a.cols << endl;
    server.sendImgComp(a);
    while (!resposta) { // Enquanto nao receber resposta, não envia nada
      server.receiveUint(resposta);
    }
    server.receiveUint(ch);
    resposta = 0;
  } while (ch != 27);
}