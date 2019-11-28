#include "SERVER.hpp"
//#include "carro.hpp"
#include "projeto.hpp"
#include <cekeikon.h>
//#include <softPwm.h>
//#include <wiringPi.h>

#define altura 240
#define largura 320

Mat_<COR> correcaoLogitech(Mat_<COR> a) {
  std::cout << "teste1" << std::endl;
  Mat_<COR> b(a.rows, a.cols);
  for (int c = 0; c < a.cols; c++) {
    for (int l = 0; l < a.rows; l++) {
      // std::cout << "c: " << c << " l: " << l << " a(l,c): " << a(l, c) << " "
      //          << std::endl;
      b(l, c) = a(l, c);
    }
  }
  return b;
}

int main() {
  // carro car;
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
  Mat_<COR> imgEnviadaCP;
  uint resposta = 0, ch = 0;

  // namedWindow("janela", WINDOW_AUTOSIZE);

  do {
    vi >> imgEnviada;
    // cout << "cols:" << imgEnviada.cols << " rows: " << imgEnviada.rows
    //     << "size: " << imgEnviada.size << endl;
    server.receiveUint(estado);
    putTxt(imgEnviada, 4, 4, to_string(estado), COR(0, 0, 255), 2);
    // imgEnviadaCP = correcaoLogitech(imgEnviada);

    // imshow("janela", imgEnviadaCP);
    ch = waitKey(25);
    server.sendImgComp(imgEnviada);

    while (!resposta) { // Enquanto nao receber resposta, não envia nada
      server.receiveUint(resposta);
      // cout << "resposta: " << resposta << endl;
    }

    server.receiveUint(ch);
    resposta = 0;

    server.receiveUint(comando);
    server.receiveUint(pwm);
    // car.setPwm(pwm);
    // car.comando(comando);
    cout << "comando: " << comando << endl;
    cout << "pwm: " << pwm << endl;

  } while (ch != 27);
}