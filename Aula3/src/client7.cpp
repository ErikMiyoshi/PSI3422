// client7.cpp
//(insira aqui os includes e a classe CLIENT)
#include "CLIENT.hpp"
#include "projeto.hpp"
int main(int argc, char *argv[]) {
  if (argc != 2)
    perror("client7 servidorIpAddr\n");
  CLIENT client(argv[1]);
  string st;
  client.receiveString(st);
  cout << st << endl;
  client.sendString("Mensagem #2 do client para server");
  client.receiveString(st);
  cout << st << endl;
}