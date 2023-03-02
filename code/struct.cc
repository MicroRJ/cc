
struct person_t
{ char  *name;
  char  *last_name;
  int    age;
  int    pad;
} ___;

int printint(int *i)
{
  int _i=*i;
  ccprintf("%i\n",_i);
}

int printperson(person_t *_p)
{
  person_t p=*_p;

  ccprintf("%s %s, %i", p.name, p.last_name, p.age);

  return 1;
}

int main(int _)
{
  person_t person;

  person.name="Dayan";
  person.last_name="Rodriguez";
  person.age=19;
  person.age=19;

  person_t *pointer=&person;
  printperson(pointer);

  int l=777;
  printint(&l);
}