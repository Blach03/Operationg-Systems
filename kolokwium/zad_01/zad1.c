#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dlfcn.h>
#include "bibl1.h"

int main (int l_param, char * wparam[]){
  int i;
  int tab[20]={1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};

  void *handle = dlopen("./lib/libbibl1.so", RTLD_LAZY);
  if (!handle) {
      fprintf(stderr, "Błąd podczas ładowania biblioteki: %s\n", dlerror());
      return 1;
  }

  double (*f1)(int *, int) = dlsym(handle, "srednia");
  int (*f2)(int *, int) = dlsym(handle, "sumuj");
  if (!f1 || !f2) {
    fprintf(stderr, "Nie znaleziono funkcji: %s\n", dlerror());
    dlclose(handle);
      return 1;
  }

  for (i = 0; i < 5; i++) {
    printf("Wynik: %lf, %d\n", f1(tab + i, 20 - i), f2(tab + i, 20 - i));
  }

  dlclose(handle);
  return 0;
}


/*
1) otworz biblioteke
2) przypisz wskaznikom f1 i f2 adresy funkcji z biblioteki sumuj i srednia
3) stworz Makefile kompilujacy biblioteke 'bibl1' ladowana dynamicznie oraz kompilujacy ten program
4) Stosowne pliki powinny znajdowac sie w folderach '.', './bin', './'lib'. Nalezy uzyc: LD_LIBRARY_PATH
5) W Makefile nalezy dodac: test:  xxxxxx
*/

