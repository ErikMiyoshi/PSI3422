// client6b.cpp
// testa sendBytes e receiveBytes
#include "CLIENT.hpp"
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
  cout << "testestest" << endl;
  cout << argv[1] << endl;
  if (argc != 2) {
    perror("client6 servidorIpAddr\n");
  }
  CLIENT cliente("10.42.0.26");
  // CLIENTE *cliente =
  //     new CLIENTE(convertToString(argv[1], sizeof(argv[1]) / sizeof(char)));
  cout << "client criado" << endl;
  const int n = 100000;
  BYTE buf[n];

  cliente.receiveBytes(n, buf);
  cout << "Recebi buff" << endl;
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
}