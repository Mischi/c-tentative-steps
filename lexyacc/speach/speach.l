%{
/*
 * Word recognizer with a symbol table.
 */

#include <string.h>

#include <sys/queue.h>

#include "y.tab.h" /* token codes from the parser */

#define LOOKUP 0 /* default - not a defined word type */

int state;

%}

%%

        /*end of line, return to default state */
\n      { state = LOOKUP; }


\.\n    { 
            state = LOOKUP;
            return 0; /* end of sentence */
        }


        /* whenever a line starts with a reserved part of speech name */
        /* start defining words of that type */
^verb   { state = VERB; }
^adj    { state = ADJECTIVE; }
^adv    { state = ADVERB; }
^noun   { state = NOUN; }
^prep   { state = PREPOSITION; }
^pron   { state = PRONOUN; }
^conj   { state = CONJUNCTION; }

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
                    case VERB: return(VERB); 
                    case ADJECTIVE: return(ADJECTIVE);
                    case ADVERB: return(ADVERB); 
                    case NOUN: return(NOUN);
                    case PREPOSITION: return(PREPOSITION);
                    case PRONOUN: return(PRONOUN);
                    case CONJUNCTION: return(CONJUNCTION); 
                    default:
                        printf("%s: don't recognize\n", yytext);
                        /* don't return, just ignore it */
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
int initialized = 0;
void
init()
{
    if(initialized == 0)
    {
        SLIST_INIT(&word_list);
        initialized = 1;
    }
}

int
add_word(int type, char *word)
{
    init();

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
    init();

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
