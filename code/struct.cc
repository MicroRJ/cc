struct person_t
{
  struct
  { char  * name;
    char  * last;
  } name;
  int     age;
  int     pad;
} ___;

int passbyvalue(person_t v)
{
  v.name.name="Dayan 1";

  ccprintf("person: %s %s, %i\n", v.name.name, v.name.last, v.age);

  return 1;
}

int main(int _)
{
  ccassert(sizeof(person_t) == 24);

  person_t p;
  p.name.name="Dayan";
  p.name.last="Rodriguez";
  p.age=19;

  ccassert(sizeof p == 24);

  ccprintf("person: %s %s, %i\n", p.name.name, p.name.last, p.age);

  passbyvalue(p);
}



















