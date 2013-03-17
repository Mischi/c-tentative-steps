%{

/*
 * A lexer for  the basic grammar to use for recognizing English sentences.
 */

#include <stdio.h>

//int yylex(void);
void yyerror(const char *);

%}

%token NOUN PRONOUN VERB ADVERB ADJECTIVE PREPOSITION CONJUNCTION

%%

sentence: subject VERB object{ printf("Sentence is valid.\n"); }
      ;

subject:    NOUN
      |     PRONOUN
      ;

object:     NOUN
      ;
%%


extern FILE *yyin;

int 
main(int argc, char const *argv[])
{
//    while(!feof(yyin))
  //  {
        yyparse();
   // }
    return 0;
}

void
yyerror(const char *s)
{
    fprintf(stderr, "%s\n", s);
}
