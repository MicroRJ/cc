
struct person_t
{ char  *name;
  char  *last_name;
  int age;
} ___;

int main(int _)
{

  person_t person;
  person.name="Dayan";
  person.last_name="Rodriguez";

  ccassert(sizeof(person) == 20);
  ccassert(sizeof(person_t) == 20);

  ccprintf("%s %s, %i\n", person.name, person.last_name, person.age);
}