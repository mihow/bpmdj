#include <stdio.h>

int main(int argc, char* argv[])
{
  int c;
  FILE * f = fopen(argv[1],"rb");
  while((c=fgetc(f))!=-1)
    {
      if (c==' ' || c=='[' || c==']' || c=='(' || c==')')
	putchar('\\');
      putchar(c);
    }
  fclose(f);
}
