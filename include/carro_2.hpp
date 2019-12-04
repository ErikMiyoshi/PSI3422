#include <cekeikon.h>

#define MOTOR1_A 3
#define MOTOR1_B 2
#define MOTOR2_A 1
#define MOTOR2_B 0

class carro2 {
private:
  /* data */
public:
  carro2(/* args */);
  ~carro2();

  void andaFrente();
  void andaTras();
  void viraEsquerda();
  void voltaEsquerda();
  void viraDireita();
  void voltaDireita();
  void rotacionaEsquerda();
  void rotacionaDireita();
  void para();

  void vira90Esquerda(TimePoint &t3, bool &acabou);
  void vira90Direita(TimePoint &t3, bool &acabou);
  void vira180Esquerda(TimePoint &t3, bool &acabou);
  void vira180Direita(TimePoint &t3, bool &acabou);
  void andaFrenteSegundos(double t, TimePoint &t3, bool &acabou);

  void setPwm(int pwm);
  void comando(int estado);
  void setComando();
  void setTamanhoTemplate(int tamanhoTemplate);

  int pwm;
  int posicaoX = 0;
  int posicaoY = 0;
  int tamanhoTemplateAtual;
  const int tamanhoTemplateLimiar = 54;
};

carro2::carro2() {
  this->pwm = 100;
  this->tamanhoTemplateAtual = 0;
}

carro2::~carro2() {}

void carro2::andaFrente() {}

void carro2::andaTras() {}

void carro2::viraEsquerda() {}

void carro2::voltaEsquerda() {}

void carro2::viraDireita() {}

void carro2::voltaDireita() {}

void carro2::rotacionaEsquerda() {}

void carro2::rotacionaDireita() {}

void carro2::para() {}

void carro2::vira90Esquerda(TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t = timeSpan(t3, t4);
  // cout << "t: " << t << endl;
  if (t < 1.65) {
    this->rotacionaEsquerda();
  } else {
    acabou = true;
    this->para();
  }
}

void carro2::vira90Direita(TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t = timeSpan(t3, t4);
  // cout << "t: " << t << endl;
  if (t < 1.65) {
    this->rotacionaDireita();
  } else {
    acabou = true;
    this->para();
  }
}

void carro2::vira180Esquerda(TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t = timeSpan(t3, t4);
  // cout << "t: " << t << endl;
  if (t < 3.1) {
    this->rotacionaEsquerda();
  } else {
    acabou = true;
    this->para();
  }
}

void carro2::vira180Direita(TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t = timeSpan(t3, t4);
  // cout << "t: " << t << endl;
  if (t < 3.1) {
    this->rotacionaDireita();
  } else {
    acabou = true;
    this->para();
  }
}

void carro2::andaFrenteSegundos(double t, TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t2 = timeSpan(t3, t4);
  if (t2 < t) {
    this->andaFrente();
  } else {
    acabou = true;
    this->para();
  }
}

void carro2::comando(int estado) {
  switch (estado) {
  case 1:
    viraEsquerda();
    break;
  case 2:
    rotacionaEsquerda();
    break;
  case 3:
    voltaEsquerda();
    break;
  case 4:
    andaFrente();
    break;
  case 5:
    para();
    break;
  case 6:
    andaTras();
    break;
  case 7:
    viraDireita();
    break;
  case 8:
    rotacionaDireita();
    break;
  case 9:
    voltaDireita();
    break;
  default:
    para();
  }
}

void carro2::setPwm(int pwm) { this->pwm = pwm; }

void carro2::setComando() {
  /*if (tamanhoTemplate > tamanhoLimiar ||
      (maxVal < maxValLimiar || maxVal_norm < maxValNormLimiar)) {
    comando = 5;
  } else if (posicaoX < largura / 3) {
    comando = 1;
    pwm += (1 - posicaoX_norm) * 30;
  } else if (posicaoX > 2 * largura / 3) {
    comando = 7;
    pwm += posicaoX_norm * 30;
  } else if (posicaoX > largura / 3 && posicaoX < 2 * largura / 3) {
    comando = 4;
  } else {
    comando = 5;
  }*/
}

void carro2::setTamanhoTemplate(int tamanhoTemplate) {
  this->tamanhoTemplateAtual = tamanhoTemplate;
}