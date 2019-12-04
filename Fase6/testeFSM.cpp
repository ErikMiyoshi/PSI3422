#include <iostream>

using namespace std;

int main() {

  int comando = 1;
  switch (comando) {
  case 1:
  estado1:
    cout << "hello world1" << endl;
    goto estado3;
    break;
  case 2:
  estado2:
    cout << "hello world2" << endl;
    goto estado6;
    break;
  case 3:
  estado3:
    cout << "hello world3" << endl;
    goto estado5;
    break;
  case 4:
  estado4:
    cout << "hello world4" << endl;
    break;
  case 5:
    break;
  estado5:
    cout << "hello world5" << endl;
    goto estado2;
  case 6:
    break;
  estado6:
    cout << "hello world6" << endl;
    goto fim;
  default:
  fim:
    break;
  }

  return 1;
}