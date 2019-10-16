// CPP program to illustrate memset
#include <cstring>
#include <iostream>

using namespace std;

int main() {
  int a, b;
  char str[] = "geeksforgeeks";
  memset(str, 't', sizeof(str));
  cout << str;
  a = 1;
  a = 10;
  return 0;
}