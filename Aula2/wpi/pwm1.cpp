//pwm1.cpp
//compila pwm1 -c -w
#include <cekeikon.h>
#include <wiringPi.h>
#include <softPwm.h>
int main () {
 wiringPiSetup () ;
 if (softPwmCreate(0, 0, 100)) erro("erro");
 if (softPwmCreate(1, 0, 100)) erro("erro");
 if (softPwmCreate(2, 0, 100)) erro("erro");
 if (softPwmCreate(3, 0, 100)) erro("erro");
 for (int i=0; i<20; i++) {
 softPwmWrite(0, 70);
 softPwmWrite(1, 70);
 softPwmWrite(2, 70);
 softPwmWrite(3, 70);
 delay (2000) ;
 softPwmWrite(0, 30);
 softPwmWrite(1, 30);
 softPwmWrite(2, 30);
 softPwmWrite(3, 30);
 delay (2000) ;
 }
}
