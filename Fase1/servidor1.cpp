// camserver2.cpp
#include "SERVER.hpp"
#include "projeto.hpp"
#include <cekeikon.h>

/*Mat_<COR> print_estado(Mat_<COR> a, int estado){
    putText(a,to_string(estado)
}*/

int main() {
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
    while (resposta != 1) { // Enquanto nao receber resposta, não envia nada
      server.receiveUint(resposta);
    }
    server.receiveUint(ch);
    resposta = 0;
    
    
  } while (ch != 27);
}
