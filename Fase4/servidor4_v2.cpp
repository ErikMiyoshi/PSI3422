#include "SERVER.hpp"
#include "carro.hpp"
#include "projeto.hpp"
#include <cekeikon.h>
#include <softPwm.h>
#include <wiringPi.h>

#define altura 240
#define largura 320

int main() {
  carro car;
  SERVER server;
  server.waitConnection();
  VideoCapture vi(0); // 0=default camera
  vi.set(CV_CAP_PROP_FRAME_WIDTH, largura);
  vi.set(CV_CAP_PROP_FRAME_HEIGHT, altura);

  uint estado;
  uint comando;
  uint pwm;

  if (!vi.isOpened())
    erro("Erro abertura webcam");
  Mat_<COR> imgEnviada;
  uint resposta = 0, ch;

  do {
    vi >> imgEnviada;
    server.receiveUint(estado);
    putTxt(imgEnviada, 4, 4, to_string(estado), COR(0, 0, 255), 2);
    // cout << "nl: " << a.rows << " nc: " << a.cols << endl;
    server.sendImgComp(imgEnviada);

    while (!resposta) { // Enquanto nao receber resposta, não envia nada
      server.receiveUint(resposta);
    }
    server.receiveUint(ch);
    resposta = 0;

    server.receiveUint(comando);
    server.receiveUint(pwm);
    car.setPwm(pwm);
    car.comando(comando);

  } while (ch != 27);
}