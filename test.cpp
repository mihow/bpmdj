#include <stdlib.h>
#include <stdio.h>

class Node
{
public:
  int left;
  int right;
  bool isNull()
  {
    return this==NULL;
  }
};

int main(int argc, char* argv[])
{
  Node* n = (Node*)NULL;
  delete(n);
  n= new Node();
  if (n->isNull())
    printf("Yes\n");
  else
    printf("No\n");
}
