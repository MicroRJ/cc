

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
}

int main(int _)
{
  person_t p;
  p.name.name="Dayan";
  p.name.last="Rodriguez";
  p.age=19;
  ccprintf("person: %s %s, %i\n", p.name.name, p.name.last, p.age);

  ccassert(sizeof p == 24);
}