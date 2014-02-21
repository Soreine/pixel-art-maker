#include <iostream>
#define n 3
using namespace std;

int main() {
  int tab[n][n];

  
  for (int i=0; i < n; i++) {
    for (int j=0; j < n; j++) {
      tab[i][j] = (j + n*i);
      tab[i][j]++;
      cout << tab[i][j] << endl;
    }
  }

  
  
  return 0;
}
