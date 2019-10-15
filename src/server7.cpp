// server7.cpp
//(insira aqui os includes e a classe SERVER)
#include "SERVER.hpp"

int main(void) {
  SERVER server;
  server.waitConnection();
  server.sendString("Mensagem #1 do server para client");
  string st;
  server.receiveString(st);
  cout << st << endl;
  server.sendString("Mensagem #3 do server para client");
}