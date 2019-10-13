// server6b.cpp
// testa sendBytes e receiveBytes
#include "projeto.hpp"
//#include <cekeikon.h>

int main() {
  std::cout << "teste" << std::endl;
  SERVER server;
  std::cout << "teste2" << std::endl;
  server.waitConnection();
  std::cout << "teste3" << std::endl;
  const int n = 100000;
  BYTE buf[n];
  memset(buf, 111, n);
  server.sendBytes(n, buf);
  server.receiveBytes(n, buf);
  if (testaBytes(buf, 214, n))
    printf("Recebeu corretamente %d bytes %d\n", n, 214);
  else
    printf("Erro na recepcao de %d bytes %d\n", n, 214);
  memset(buf, 111, n);
  server.sendBytes(n, buf);
}