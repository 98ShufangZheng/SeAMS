#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "unionfind.h"

int main(int argc, char* argv[]) {
  int n = 100;
  struct unionfind *uf = unionfind_new(n);

  int s1, s2, r;
  int i;
  for (i = 0; i < n/10; i++) {
    s1 = rand() % n;
    s2 = rand() % n;
    r = unionfind_union(uf, s1, s2);
    printf("union %i %i %i\n", s1, s2, r);
    s1 = unionfind_find(uf, s1);
    s2 = unionfind_find(uf, s2);
    assert(r == s1);
    assert(r == s2);
  }
  for (i = 0; i < 100; i++) {
    r = unionfind_find(uf, i);
    printf("find %i %i\n", i, r);
  }
}

