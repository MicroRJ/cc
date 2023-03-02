
struct person_t
{ char  * name;
  char  * last;
  int     age;
  int     pad;
} ___;

int main(int _)
{
  person_t p;

  p.name="Dayan";
  p.last="Rodriguez";
  p.age=19;

  ccprintf("%s %s, %i\n", p.name, p.last, p.age);
}