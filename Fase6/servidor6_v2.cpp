#include "SERVER.hpp"
#include "carro.hpp"
//#include "carro_2.hpp"
#include "projeto.hpp"
#include "serverFSM.cpp"
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
  carro car;
  SERVER server;
  server.waitConnection();
  VideoCapture vi(0); // 0=default camera
  vi.set(CV_CAP_PROP_FRAME_WIDTH, largura);
  vi.set(CV_CAP_PROP_FRAME_HEIGHT, altura);

  uint estado;
  uint comando = 5;
  uint pwm;
  bool start = false;
  uint digitoAtual = 10;
  uint tamanhoTemplate;

  if (!vi.isOpened())
    erro("Erro abertura webcam");
  Mat_<COR> imgEnviada;
  Mat_<COR> imgEnviadaCP;
  uint resposta = 0, ch = 0;
  TimePoint t3;
  bool acaoRealizada = true;
  bool reconheci = false;

  // namedWindow("janela", WINDOW_AUTOSIZE);
  uint estadoAnterior = 1, estadoAtual = 1;

  int contagem[10];
  for (int i = 0; i < 10; i++) {
    contagem[i] = 0;
  }

  do {
    TimePoint t1 = timePoint();
    vi >> imgEnviada;
    // cout << "cols:" << imgEnviada.cols << " rows: " << imgEnviada.rows
    //     << "size: " << imgEnviada.size << endl;
    TimePoint t2 = timePoint();
    server.receiveUint(estado);
    // putTxt(imgEnviada, 4, 4, to_string(estado), COR(0, 0, 255), 2);
    // imgEnviadaCP = correcaoLogitech(imgEnviada);

    // imshow("janela", imgEnviadaCP);
    server.sendImgComp(imgEnviada);
    estadoAnterior = estadoAtual;
    server.sendUint(estadoAtual);

    while (!resposta) { // Enquanto nao receber resposta, não envia nada
      server.receiveUint(resposta);
      // cout << "resposta: " << resposta << endl;
    }
    TimePoint t4 = timePoint();
    server.receiveUint(ch);

    if (ch == 115)
      start = 1;
    else
      start = 0;
    resposta = 0;
    estadoAtual = fsmServer(estadoAnterior, start, car, contagem, digitoAtual,
                            comando, t3, acaoRealizada, reconheci);

    TimePoint t5 = timePoint();
    server.receiveUint(tamanhoTemplate);
    car.setTamanhoTemplate(tamanhoTemplate);
    TimePoint t6 = timePoint();
    server.receiveUint(digitoAtual);
    cout << "Digito Atual: " << digitoAtual << endl;
    server.receiveUint(comando);
    server.receiveUint(pwm);
    car.setPwm(pwm);
    // car.comando(comando);
    // cout << "comando: " << comando << endl;
    // cout << "pwm: " << pwm << endl;
    TimePoint t7 = timePoint();

    // cout << timeSpan(t1, t2) << " " << timeSpan(t2, t3) << " "
    //     << timeSpan(t3, t4) << " " << timeSpan(t4, t5) << " "
    //     << timeSpan(t5, t6) << " " << timeSpan(t6, t7) << " " << endl;
    // cout << "Comando: " << comando << endl;
    // cout << "Tamanho: " << tamanhoTemplate << endl;
    // cout << "ch: " << ch << endl;

    // Comando manual
    if (ch == 105) { //'i'
      cout << "Andando para frente" << endl;
      car.comando(4);
    }
    if (ch == 106) { //'j'
      car.comando(2);
      cout << "Virando para esquerda" << endl;
    }
    if (ch == 107) { // k
      car.comando(6);
      cout << "Dando re" << endl;
    }
    if (ch == 108) { // l
      car.comando(8);
      cout << "Virando para direita" << endl;
    }
    if (ch == 112) { // p
      cout << "Parando carro" << endl;
      car.comando(5);
    }
  } while (ch != 27);
}