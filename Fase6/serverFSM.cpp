//#include "carro.hpp"
//#include <softPwm.h>
//#include <wiringPi.h>

#define tempoEspera 100 // ms

void printMsg(string msg) { cout << msg << endl; }
void printVetor(int a[10]) {
  for (int i = 0; i < 10; i++) {
    cout << " " << a[i];
  }
  cout << endl;
}

bool reconheceDigito(int contagem[10], int &digitoReconhecido,
                     int digitoAtual) {
  if (digitoAtual > 9)
    return false;

  contagem[digitoAtual] += 1;
  const int threshold = 20;
  for (int i = 0; i < 10; i++) {
    if (contagem[i] > threshold) {
      digitoReconhecido = i;
      cout << "ReconheciDigito" << endl;
      return true;
    }
    cout << " " << contagem[i];
  }
  cout << endl;

  return false;
}

void zeraContagem(int contagem[10]) {
  for (int i = 0; i < 10; i++) {
    contagem[i] = 0;
  }
}

int fsmServer(uint estadoAnterior, int start, carro car, int contagem[10],
              int digitoAtual, int comando, TimePoint &t3, bool &acaoRealizada,
              bool &reconheci) {
  int digitoReconhecido = 10;
  cout << "Var Reconheci: " << reconheci << endl;
  cout << "Var AcaoRealizada: " << acaoRealizada << endl;
  cout << "estadoAtual: " << estadoAnterior << endl;
  cout << "tamanho: " << car.tamanhoTemplateAtual << endl;
  printVetor(contagem);
  switch (estadoAnterior) {
  case 10:
    goto parado;
    break;
  case 11:
    goto segue;
    break;
  case 12:
    goto reconhecendo;
    break;
  case 13:
    goto acabouTrajeto;
    break;
  case 14:
    goto vira180Esquerda;
    break;
  case 15:
    goto vira180Direita;
    break;
  case 16:
    goto vira90Esquerda;
    break;
  case 17:
    goto vira90Direita;
    break;
  case 18:
    goto emFrente;
    break;
  case 19:
    goto nenhum;
    break;
  case 20:
    break;
    goto fim;
  default:
    break;
  }

comeca:
  if (start) {
    goto segue;

  parado:
    car.para();
    return 10;

  segue:
    printMsg("Seguindo placa");
    int tamanho; // template matching ou outra variavel recebida pelo cliente,
                 // indicando dist min até a placa
    car.comando(comando);
    cout << "tamanho segue: " << car.tamanhoTemplateAtual << endl;
    if (car.tamanhoTemplateAtual > car.tamanhoTemplateLimiar &&
        digitoAtual == 10 && estadoAnterior == 11) {
      car.para();
      cout << "parando de seguir placa:  " << endl;
      goto reconhecendo;
    } else
      return 11;

  reconhecendo:
    printMsg("Reconhecendo digito");
    if (acaoRealizada || !reconheci) {
      reconheci = reconheceDigito(contagem, digitoReconhecido, digitoAtual);
      cout << "Digito Reconhecido: " << digitoReconhecido << endl;
      acaoRealizada = false;
      t3 = timePoint();
    }
    if (reconheci) {
      zeraContagem(contagem);
      if (acaoRealizada)
        reconheci = false;
      if (digitoReconhecido == 0 || digitoReconhecido == 1)
        goto acabouTrajeto;
      if (digitoReconhecido == 2)
        goto vira180Esquerda;
      if (digitoReconhecido == 3)
        goto vira180Direita;
      if (digitoReconhecido == 4 || digitoReconhecido == 5)
        goto emFrente;
      if (digitoReconhecido == 6 || digitoReconhecido == 7)
        goto vira90Esquerda;
      if (digitoReconhecido == 8 || digitoReconhecido == 9)
        goto vira90Direita;
      if (digitoReconhecido == 10)
        reconheci = 0;
      goto segue;
    }
    return 12;

  acabouTrajeto:
    printMsg("Acabou trajeto");
    car.para();
    // zeraContagem(contagem);
    waitKey(tempoEspera);
    goto fim;
    return 13;

  vira180Esquerda:
    printMsg("Virando 180 para esquerda");
    car.vira180Esquerda(t3, acaoRealizada);
    car.setTamanhoTemplate(10);
    // zeraContagem(contagem);
    if (acaoRealizada) {
      // waitKey(tempoEspera);
      return 11;
    } else
      return 14;

  vira180Direita:
    printMsg("Virando 180 para direita");
    car.vira180Direita(t3, acaoRealizada);
    // zeraContagem(contagem);
    if (acaoRealizada) {
      // waitKey(tempoEspera);
      return 11;
    }
    return 15;

  vira90Esquerda:
    printMsg("Virando 90 para esquerda");
    car.vira90Esquerda(t3, acaoRealizada);
    // zeraContagem(contagem);
    if (acaoRealizada) {
      // waitKey(tempoEspera);
      return 11;
    }
    return 16;

  vira90Direita:
    printMsg("Virando 90 para direita");
    car.vira90Direita(t3, acaoRealizada);
    car.setTamanhoTemplate(10);
    // zeraContagem(contagem);
    if (acaoRealizada) {
      // waitKey(tempoEspera);
      car.setTamanhoTemplate(10);
      return 11;
    }
    return 17;

  emFrente:
    printMsg("Em frente");
    car.andaFrenteSegundos(8, t3, acaoRealizada);
    // zeraContagem(contagem);
    if (acaoRealizada) {
      // waitKey(tempoEspera);
      return 11;
    }
    return 18;

  nenhum:
    zeraContagem(contagem);
    return 19;

  } else {
  parado2:
    cout << "MEF não iniciada" << endl;
    car.para();
    return 21;
  }
fim:
  return 20;
}