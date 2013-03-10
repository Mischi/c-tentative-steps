#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/queue.h>

SLIST_HEAD(, person) head;

struct person {
    char firstname[30];
    char lastname[30];

    SLIST_ENTRY(person) entries; 
};

struct person *p;

void insertPerson();
void printPersons();

int main(int argc, char const *argv[])
{
    int input = -1;
    char c;

    SLIST_INIT(&head);
    
    while(1) {
        printf("Menu:\n");
        printf("(0) - Insert Person\n");
        printf("(1) - Print all Persons\n");
        printf("(2) - Exit\n");
        printf("Choose: ");
        scanf("%d", &input);

        while((c = getchar()) != '\n' && c != EOF)
            /* discard */;

        switch(input)
        {
            case 0:
                insertPerson();
                break;
            case 1:
                printPersons();
                break;
            case 2:
                return 0;
            default:
                printf("Invalid Input - Try again!\n");
                break;
        }
    }
}

void printPersons() {
    SLIST_FOREACH(p, &head, entries) {
        printf("%s %s\n", p->firstname, p->lastname);
    }
}

void insertPerson()
{
    struct person *person;
    size_t ln;
    
    person = malloc(sizeof(struct person));

    printf("Enter Firstname: ");
    if(fgets(person->firstname, sizeof(person->firstname), stdin) != NULL)
        person->firstname[strcspn(person->firstname, "\n")] = '\0';


    printf("Enter Lastname: ");
    if(fgets(person->lastname, sizeof(person->lastname), stdin) != NULL)
        person->lastname[strcspn(person->lastname, "\n")] = '\0';

    SLIST_INSERT_HEAD(&head, person, entries);
}
