#include "carro.hpp"
#include <softPwm.h>
#include <wiringPi.h>

int main() {
  carro car;
  car.setPwm(100);

  car.vira180Direita();
  wait(200);
  car.vira180Esquerda();
  wait(200);
  car.vira90Direita();
  wait(200);
  car.vira90Esquerda();
  wait(200);
  return 0;
}