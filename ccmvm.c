#ifndef _CCMVM
#define _CCMVM

#define ARG 22
#include "cc.c"

ccglobal cci64_t stack[256];
ccglobal cci64_t saved;
ccglobal cci64_t value[256];

int main(int argc, char **argv)
{
  // int i;
  // int b;
  // int c,d,e;
  // i=1;
  // int *f;
  // f=&b;
  // f=f+2;
  value['i']=1;
  value['f']=(cci64_t)&value['b'];
  value['f']=value['f']+2;
}

#endif