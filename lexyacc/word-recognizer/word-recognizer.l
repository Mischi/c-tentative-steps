%{
/*
 * Word recognizer with a symbol table.
 */

#include <string.h>

#include <sys/queue.h>

enum {
    LOOKUP = 0, /* default - looking rather than defining. */
    VERB = 1,
    ADJ = 2,
    ADV = 3,
    NOUN = 4,
    PREP = 5,
    PRON = 6,
    CONJ = 7
};

int state;

int add_word(int type, char *word);
int lookup_word(char *word);
%}

%%

        /*end of line, return to default state */
\n      { state = LOOKUP; }

        /* whenever a line starts with a reserved part of speech name */
        /* start defining words of that type */
^verb   { state = VERB; }
^adj    { state = ADJ; }
^adv    { state = ADV; }
^noun   { state = NOUN; }
^prep   { state = PREP; }
^pron   { state = PRON; }
^conj   { state = CONJ; }

        /* a normal word, define it or look it up */
[a-zA-Z]+ {
            if(state != LOOKUP) 
            {
                /* define the current word */
                add_word(state, yytext);
            }
            else
            {
                switch(lookup_word(yytext))
                {
                    case VERB: printf("%s: verb\n", yytext); break;
                    case ADJ: printf("%s: adjective\n", yytext); break;
                    case ADV: printf("%s: adverb\n", yytext); break;
                    case NOUN: printf("%s: noun\n", yytext); break;
                    case PREP: printf("%s: preposition\n", yytext); break;
                    case PRON: printf("%s: pronoun\n", yytext); break;
                    case CONJ: printf("%s: conjunction\n", yytext); break;
                    default:
                        printf("%s: don't recognize\n", yytext);
                        break;
                }
            }
          }

.       /* ignore anythjing else */ ;

%%


struct word {
    char *word_name;
    int word_type;
    SLIST_ENTRY(word) next_word;
};

SLIST_HEAD(, word) word_list;


extern void *malloc();


int
main()
{
    SLIST_INIT(&word_list);
    yylex();
    return 0;
}


int
add_word(int type, char *word)
{
    struct word *wp;

    if(lookup_word(word) != LOOKUP)
    {
        printf("!!! warning: word %s already defined \n", word);
        return 0;
    }

    /* word not there, allocate a new entry and link it on the list */

    wp = (struct word *) malloc(sizeof(struct word));


    /* have to copy the word itself as well */

    wp->word_name = (char *) malloc(strlen(word) + 1);
    strcpy(wp->word_name, word); 

    wp->word_type = type;
    
    SLIST_INSERT_HEAD(&word_list, wp, next_word);
    return 1; /* it worked */
}


int
lookup_word(char *word)
{
    struct word *wp;

    SLIST_FOREACH(wp, &word_list, next_word)
    {
        if(strcmp(wp->word_name, word) == 0)
        {
            return wp->word_type;
        }
    }

    return LOOKUP; /* not found */
}
