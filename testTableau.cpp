#include <iostream>
#define n 3
using namespace std;

int main() {
  
  cout << "Test pour variables et pointeurs" << endl;

  int * pointeur;
  int variable;

  pointeur = new int;

  *pointeur = 33;
  variable = 55;

  cout << *pointeur << endl;
  cout << pointeur << endl;
  cout << &pointeur << endl;
  cout << variable << endl;
  cout << &variable << endl;

  
  cout << endl << "Test pour tableaux" << endl;

  delete pointeur;

  int * poinTab;
  int varTab[n];
  
  poinTab = new int[n];

  for (int i = 0; i < n; i++) {
    poinTab[i] = i;
  }
  
  for (int i = 0; i < n; i++) {
    varTab[i] = 10*i;
  }


  for (int i = 0; i < n; i++) {
    cout << poinTab[i] << " ";
  }
  cout << endl;

  for (int i = 0; i < n; i++) {
    cout << varTab[i] << " ";
  }
  cout << endl;

  delete poinTab;

  cout << endl << "Test pour tableaux multidimensionnels" << endl;

  int * poinTabMult;
  int varTabMult[n][n];
  
  poinTabMult = new int[n*n];

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      poinTabMult[i][j] = i;
    }
  }
  
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      varTabMult[i][j] = 10*i;
    }
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cout << poinTabMult[i][j] << " ";
    }
  }
  cout << endl;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cout << varTabMult[i][j] << " ";
    }
  }
  cout << endl;

  return 0;
}
