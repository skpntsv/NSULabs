#include <stdio.h>
#include <unistd.h>

int global_not_inited[8192];
int global_inited = 10;

const int global_const;

char *global_pstr = "global hello";

void f2() {
  int a = 123456;
  
  printf("f2: %d\n", a);
}

void f() {
  int local;
  static int static_not_inited;
  static int static_inited = 5;
  
  char *local_pstr = "local hello";
  
  const int local_const = 30;
  
  char local_array[] = "local hello array";
  
  printf("\nfunction variables:\n");
  printf("local:             %p (%d)\n", &local, local);
  printf("static inited:     %p (%d)\n", &static_inited, static_inited);
  printf("static not inited: %p (%d)\n", &static_not_inited, static_not_inited);
  printf("local pstr:        %p %p (%s)\n", &local_pstr, local_pstr, local_pstr);
  printf("local const        %p (%d)\n", &local_const, local_const);
  printf("local array:       %p %p (%s)\n", &local_array, &local_array[5], local_array);
}

int main() {
  
  printf("function f():      %p\n", f);
  
  printf("\nglobal variables:\n");
  printf("global ininted:    %p (%d)\n", &global_inited, global_inited);
  printf("global not inited: %p %p (%d)\n", &global_not_inited, &global_not_inited[8100], global_not_inited[8100]);
  printf("global pstr:       %p %p (%s)\n", &global_pstr, global_pstr, global_pstr);
  printf("global const       %p (%d)\n", &global_const, global_const);
  
  f();
  f2();
  
  printf("pid: %d\n", getpid());
  sleep(30);
  
  return 0;
}