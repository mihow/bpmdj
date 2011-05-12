#include <stdio.h>
#include "avltree.cpp"

class MyNode : public Node<char>
{
  char t;
public:
  MyNode(char p)
  {
    t=p;
  }
  virtual int compareAddData(Node<char>* other)
  {
    MyNode * m = (MyNode*)other;
    compareSearchData(m->t);
  }
  virtual int compareSearchData(char key)
  {
    if (key<t) return -1;
    if (key==t) return 0;
    return 1;
  }
  virtual void print()
  {
    printf("%c",t);
  }
};

int main(int argc, char* argv[])
{
  AvlTree<char> *test;
  test = new AvlTree<char>;
  for(char c = 'a' ; c < 'z' ; c ++)
    test->add(new MyNode(c));
  test->print();
}
