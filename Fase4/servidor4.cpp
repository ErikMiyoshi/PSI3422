// camserver2.cpp
#include "SERVER.hpp"
#include "carro.hpp"
#include "projeto.hpp"
#include <cekeikon.h>
#include <softPwm.h>
#include <wiringPi.h>

int main() {
  carro car;
  SERVER server;
  server.waitConnection();
  VideoCapture vi(0); // 0=default camera
  vi.set(CV_CAP_PROP_FRAME_WIDTH, 320);
  vi.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

  uint estado;
  uint comando;

  if (!vi.isOpened())
    erro("Erro abertura webcam");
  Mat_<COR> a;
  uint resposta = 0, ch;

  do {
    vi >> a;
    server.receiveUint(estado);
    putTxt(a, 4, 4, to_string(estado), COR(0, 0, 255), 2);
    // cout << "nl: " << a.rows << " nc: " << a.cols << endl;
    server.sendImgComp(a);

    while (!resposta) { // Enquanto nao receber resposta, não envia nada
      server.receiveUint(resposta);
    }
    server.receiveUint(ch);
    resposta = 0;

    server.receiveUint(comando);

    car.setPwm() car.comando(comando);

  } while (ch != 27);
}