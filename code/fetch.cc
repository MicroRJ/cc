

struct ccstruct_t
{
  int a;
  int b;
  int c;
} _t;

int printstruct(ccstruct_t h)
{
  ccprintf("value: a := %i, b := %i, c := %i\n", h.a, h.b, h.c);
}


int main(int _)
{
  ccstruct_t v;
  v.a=777;
  v.b=888;
  v.c=999;

  ccassert(v.a==777);
  ccassert(v.b==888);
  ccassert(v.c==999);

  printstruct(v);

  ccassert(v.a==777);
  ccassert(v.b==888);
  ccassert(v.c==999);

  ccassert(sizeof v == 12);
  ccassert(sizeof v.a == 4);
  ccassert(sizeof v.b == 4);
  ccassert(sizeof v.c == 4);

}