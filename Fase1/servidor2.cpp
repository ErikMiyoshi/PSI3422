// camserver2.cpp
#include "SERVER.hpp"
#include "projeto.hpp"
#include <cekeikon.h>
#include <wiringPi.h>
#include <softPwm.h>

/*Mat_<COR> print_estado(Mat_<COR> a, int estado){
    putText(a,to_string(estado)
}*/

int main() {
  wiringPiSetup();
  if (softPwmCreate(0, 0, 100)) erro("erro");
  if (softPwmCreate(1, 0, 100)) erro("erro");
  if (softPwmCreate(2, 0, 100)) erro("erro");
  if (softPwmCreate(3, 0, 100)) erro("erro");
	
  
  SERVER server;
  server.waitConnection();
  VideoCapture vi(0); // 0=default camera
  vi.set(CV_CAP_PROP_FRAME_WIDTH, 320);
  vi.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
  
  uint estado;
  
  if (!vi.isOpened())
    erro("Erro abertura webcam");
  Mat_<COR> a;
  uint resposta = 0, ch;

  do {
    vi >> a;
    server.receiveUint(estado);
    putTxt(a,4,4,to_string(estado),COR(0,0,255),2);
    // cout << "nl: " << a.rows << " nc: " << a.cols << endl;
    server.sendImgComp(a);
    
    switch(estado){
		case 1:
			softPwmWrite(0, 100); softPwmWrite(1, 0);
			softPwmWrite(2, 0); softPwmWrite(3, 0);
			break;
		case 2:
			softPwmWrite(0, 100); softPwmWrite(1, 0);
			softPwmWrite(2, 0); softPwmWrite(3, 100);
			break;
		case 3:
			softPwmWrite(0, 0); softPwmWrite(1, 100);
			softPwmWrite(2, 0); softPwmWrite(3, 0);
			break;
		case 4:
			softPwmWrite(0, 100); softPwmWrite(1, 0);
			softPwmWrite(2, 100); softPwmWrite(3, 0);
			break;
		case 5:
			softPwmWrite(0, 0); softPwmWrite(1, 0);
			softPwmWrite(2, 0); softPwmWrite(3, 0);
			break;
		case 6:
			softPwmWrite(0, 0); softPwmWrite(1, 100);
			softPwmWrite(2, 0); softPwmWrite(3, 100);
			break;
		case 7:
			softPwmWrite(0, 0); softPwmWrite(1, 0);
			softPwmWrite(2, 100); softPwmWrite(3, 0);
			break;
		case 8:
			softPwmWrite(0, 0); softPwmWrite(1, 100);
			softPwmWrite(2, 100); softPwmWrite(3, 0);
			break;
		case 9:
			softPwmWrite(0, 0); softPwmWrite(1, 0);
			softPwmWrite(2, 0); softPwmWrite(3, 100);
			break;
		default:
			softPwmWrite(0, 0); softPwmWrite(1, 0);
			softPwmWrite(2, 0); softPwmWrite(3, 0);
    }
    
    while (!resposta) { // Enquanto nao receber resposta, não envia nada
      server.receiveUint(resposta);
    }
    server.receiveUint(ch);
    resposta = 0;
    
    
    
  } while (ch != 27);
}
