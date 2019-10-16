// client6b.cpp
// testa sendBytes e receiveBytes
#include "CLIENT.hpp"
//#include "SERVER.hpp"
#include "projeto.hpp"

string convertToString(char *a, int size) {
  int i;
  string s = "";
  for (i = 0; i < size; i++) {
    s = s + a[i];
  }
  return s;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    perror("client6 servidorIpAddr\n");
  }
  CLIENT cliente(argv[1]);
  const int n = 100000;
  BYTE buf[n];

  cliente.receiveBytes(n, buf);
  if (testaBytes(buf, 111, n))
    printf("Recebeu corretamente %d bytes %d\n", n, 111);
  else
    printf("Erro na recepcao de %d bytes %d\n", n, 111);
  memset(buf, 214, n);
  cliente.sendBytes(n, buf);

  cliente.receiveBytes(n, buf);
  if (testaBytes(buf, 2, n))
    printf("Recebeu corretamente %d bytes %d\n", n, 2);
  else
    printf("Erro na recepcao de %d bytes %d\n", n, 2);

  cliente.encerra();
}