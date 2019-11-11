#include <cekeikon.h>
#include <softPwm.h>
#include <wiringPi.h>

#define MOTOR1_A 3
#define MOTOR1_B 2
#define MOTOR2_A 1
#define MOTOR2_B 0

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

  void setPwm(int pwm);
  void comando(int estado);

  int pwm = 100;
};

carro::carro() {
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
  softPwmWrite(MOTOR1_A, pwm);
  softPwmWrite(MOTOR1_B, 0);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, pwm);
}

void carro::andaTras() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, pwm);
  softPwmWrite(MOTOR2_A, pwm);
  softPwmWrite(MOTOR2_B, 0);
}

void carro::viraEsquerda() {
  softPwmWrite(MOTOR1_A, pwm);
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
  softPwmWrite(MOTOR2_A, pwm);
  softPwmWrite(MOTOR2_B, 0);
}

void carro::rotacionaDireita() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, pwm);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, pwm);
}

void carro::para() {
  softPwmWrite(MOTOR1_A, 0);
  softPwmWrite(MOTOR1_B, 0);
  softPwmWrite(MOTOR2_A, 0);
  softPwmWrite(MOTOR2_B, 0);
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