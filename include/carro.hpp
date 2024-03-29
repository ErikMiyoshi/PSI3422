#include <cekeikon.h>
#include <softPwm.h>
#include <wiringPi.h>

#define MOTOR1_A 3
#define MOTOR1_B 2
#define MOTOR2_A 1
#define MOTOR2_B 0
#define COMPENSA 7

class carro {
private:
  /* data */
public:
  carro(/* args */);
  ~carro();

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
  const int tamanhoTemplateLimiar = 44;
};

carro::carro() {
  this->pwm = 100;
  this->tamanhoTemplateAtual = 0;
  wiringPiSetup();
  if (softPwmCreate(0, 0, 100))
    erro("erro");
  if (softPwmCreate(1, 0, 100))
    erro("erro");
  if (softPwmCreate(2, 0, 100))
    erro("erro");
  if (softPwmCreate(3, 0, 100))
    erro("erro");
}

carro::~carro() {}

void carro::andaFrente() {
  softPwmWrite(MOTOR1_A, pwm - COMPENSA);
  softPwmWrite(MOTOR1_B, 0);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, pwm);
}

void carro::andaTras() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, pwm - COMPENSA);
  softPwmWrite(MOTOR2_A, pwm);
  softPwmWrite(MOTOR2_B, 0);
}

void carro::viraEsquerda() {
  softPwmWrite(MOTOR1_A, pwm - COMPENSA);
  softPwmWrite(MOTOR1_B, 0);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, 0);
}

void carro::voltaEsquerda() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, pwm);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, 0);
}

void carro::viraDireita() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, 0);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, pwm);
}

void carro::voltaDireita() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, 0);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, pwm);
}

void carro::rotacionaEsquerda() {
  softPwmWrite(MOTOR1_A, pwm);
  softPwmWrite(MOTOR1_B, 0);
  softPwmWrite(MOTOR2_A, pwm - COMPENSA);
  softPwmWrite(MOTOR2_B, 0);
}

void carro::rotacionaDireita() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, pwm);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, pwm - COMPENSA);
}

void carro::para() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, 0);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, 0);
}

void carro::vira90Esquerda(TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t = timeSpan(t3, t4);
  // cout << "t: " << t << endl;
  if (t < 1.4) {
    this->rotacionaEsquerda();
  } else {
    acabou = true;
    this->para();
  }
}

void carro::vira90Direita(TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t = timeSpan(t3, t4);
  // cout << "t: " << t << endl;
  if (t < 1.4) {
    this->rotacionaDireita();
  } else {
    acabou = true;
    this->para();
  }
}

void carro::vira180Esquerda(TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t = timeSpan(t3, t4);
  // cout << "t: " << t << endl;
  if (t < 2.9) {
    this->rotacionaEsquerda();
  } else {
    acabou = true;
    this->para();
  }
}

void carro::vira180Direita(TimePoint &t3, bool &acabou) {
  TimePoint t4 = timePoint();
  this->pwm = 100;
  double t = timeSpan(t3, t4);
  // cout << "t: " << t << endl;
  if (t < 2.9) {
    this->rotacionaDireita();
  } else {
    acabou = true;
    this->para();
  }
}

void carro::andaFrenteSegundos(double t, TimePoint &t3, bool &acabou) {
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

void carro::comando(int estado) {
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

void carro::setPwm(int pwm) { this->pwm = pwm; }

void carro::setComando() {
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

void carro::setTamanhoTemplate(int tamanhoTemplate) {
  this->tamanhoTemplateAtual = tamanhoTemplate;
}