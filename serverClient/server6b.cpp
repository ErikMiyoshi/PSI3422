// server6b.cpp
// testa sendBytes e receiveBytes
#include "projeto.hpp"
//#include <cekeikon.h>

int main(void) {
  SERVER server;
  server.waitConnection();

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
// client6b.cpp
// testa sendBytes e receiveBytes
#include "projeto.hpp"
int main(int argc, char *argv[]) {
  if (argc != 2)
    perror("client6 servidorIpAddr\n");
  CLIENT client(argv[1]);

  const int n = 100000;
  BYTE buf[n];

  client.receiveBytes(n, buf);
  if (testaBytes(buf, 111, n))
    printf("Recebeu corretamente %d bytes %d\n", n, 111);
  else
    printf("Erro na recepcao de %d bytes %d\n", n, 111);
  memset(buf, 214, n);
  client.sendBytes(n, buf);

  client.receiveBytes(n, buf);
  if (testaBytes(buf, 2, n))
    printf("Recebeu corretamente %d bytes %d\n", n, 2);
  else
    printf("Erro na recepcao de %d bytes %d\n", n, 2);
}