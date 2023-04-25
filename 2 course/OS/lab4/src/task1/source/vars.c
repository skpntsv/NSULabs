#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int global_not_inited[8192];
int global_inited = 10;

const int global_const;

//char *global_pstr = "global hello";

void f() {
  int local;
  static int static_not_inited;
  static int static_inited = 5;
  
  //char *local_pstr = "local hello";
  
  const int local_const = 30;
  
  //char local_array[] = "local hello array";
  
  printf("\nfunction variables:\n");
  printf("local:             %p (%d)\n", &local, local);
  printf("static inited:     %p (%d)\n", &static_inited, static_inited);
  printf("static not inited: %p (%d)\n", &static_not_inited, static_not_inited);
  //printf("local pstr:        %p %p (%s)\n", &local_pstr, local_pstr, local_pstr);
  printf("local const        %p (%d)\n", &local_const, local_const);
  //printf("local array:       %p %p (%s)\n", &local_array, &local_array[5], local_array);
}

int *f2() {
  int local = 1234;
  
  printf("\nlocal var from f2 in f2: %p (%d)\n", &local, local);

  return &local;
}

void environ() {
  char* env = getenv("PORT");
  printf("\nPORT=%s\n", env);

  if (setenv("PORT", "new_value", 1) == -1) {
    perror("setenv");
  }
  env = getenv("PORT");
  printf("PORT=%s\n", env);
}

int main() {
  
  printf("function f():      %p\n", f);
  
  printf("\nglobal variables:\n");
  printf("global ininted:    %p (%d)\n", &global_inited, global_inited);
  printf("global not inited: %p %p (%d)\n", &global_not_inited, &global_not_inited[8100], global_not_inited[8100]);
  //printf("global pstr:       %p %p (%s)\n", &global_pstr, global_pstr, global_pstr);
  printf("global const       %p (%d)\n", &global_const, global_const);
  
  f();

  // Возвращаем адресс локальной переменной из функции
  int *local2 = f2();
  printf("Address local from func: %p\n", local2);
  //printf("Value of local from func: %d\n", *local2); 

  // Переменая окружения
  environ();

  printf("\npid: %d\n", getpid());
  sleep(30);
  
  return 0;
}