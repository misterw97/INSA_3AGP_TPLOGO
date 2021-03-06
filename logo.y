%{
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>

  #include "logo_type.h"
  #include "logo_functions.h"

  void yyerror(const char* str) {
    printf("Error : %s\n", str);
  }
  int yywrap() {
    return 1;
  }

  char programName[50] = "out.svg";
  int var[26];

%}

%token NAME_
%token ENTIER VAR
%token FORWARD_ LEFT_ RIGHT_ REPEAT_ HIDE_ SHOW COLOR_ DELTA SCALE_ NAME GTX GTY
%token RECT POLYGON CIRCLE

//type de yylval
%union {
  char name[50];
  struct model_step* node;
  int value;
  char vname;
};

//type des  symboles
%type <vname> VAR VARIABLE // char
%type <name> NAME_ // char*
%type <value> VALUE ENTIER COLOR_ FACTOR TERM EXPR // int
%type <node> INSTRUCTION PROGRAM // Node*

%%

FINAL:
  PROGRAM {
    printLogo($1,0);
    writeSVG($1,programName);
    freeLogo(&$1);
  }

PROGRAM:
  NAME NAME_ PROGRAM
  {
    strcpy(programName,$2);
    $$ = $3;
  }
  | INSTRUCTION
  {
    $$ = InitLogo();
    addNode(&$$,$1);
  }
  | PROGRAM INSTRUCTION
  {
    addNode(&$1,$2);
    $$ = $1;
  }

INSTRUCTION:
  // Instructions de base
  FORWARD_ VALUE {
    $$ = createNode(FORWARD,$2,NULL);
  }
  | LEFT_ VALUE {
    $$ = createNode(LEFT,$2,NULL);
  }
  | RIGHT_ VALUE {
    $$ = createNode(RIGHT,$2,NULL);
  }
  | SCALE_ VALUE {
    $$ = createNode(SCALE,$2,NULL);
  }
  | HIDE_ {
    $$ = createNode(HIDE,1,NULL);
  }
  | SHOW {
    $$ = createNode(HIDE,0,NULL);
  }
  | GTX VALUE {
    $$ = createNode(GOTOX,$2,NULL);
  }
  | GTY VALUE {
    $$ = createNode(GOTOY,$2,NULL);
  }
  // Imbrications
  | REPEAT_ VALUE '[' PROGRAM ']' {
    $$ = createNode(REPEAT,$2,$4);
  }
  // Couleur
  | COLOR_ VALUE {
    $$ = createNode(COLOR,$2*10+$1,NULL);
  }
  | COLOR_ DELTA VALUE {
    int d = ($3>0?1:-1);
    $$ = createNode(DCOLOR,(d*(10*d*$3+$1)),NULL);
  }
  // Alias
  | RECT VALUE VALUE {
    Program rect = InitLogo();
    addNode(&rect,createNode(FORWARD,$2,NULL));
    addNode(&rect,createNode(RIGHT,90,NULL));
    addNode(&rect,createNode(FORWARD,$3,NULL));
    addNode(&rect,createNode(RIGHT,90,NULL));
    $$ = createNode(REPEAT,2,rect);
  }
  | POLYGON VALUE VALUE {
    int angle = (int)(360/$2);
    Program poly = InitLogo();
    addNode(&poly,createNode(FORWARD,$3,NULL));
    addNode(&poly,createNode(RIGHT,angle,NULL));
    $$ = createNode(REPEAT,$2,poly);
  }
  | CIRCLE VALUE {
    Program poly = InitLogo();
    addNode(&poly,createNode(FORWARD,$2,NULL));
    addNode(&poly,createNode(RIGHT,1,NULL));
    $$ = createNode(REPEAT,360,poly);
  }

// Gestion des expressions mathématiques

VALUE: EXPR {
    $$ = $1;
  }

EXPR: EXPR '+' TERM {
    $$ = $1 + $3;
  }
  | EXPR '-' TERM {
    $$ = $1 - $3;
  }
  | TERM {
    $$ = $1;
  }

TERM: TERM '*' FACTOR {
    $$ = $1 * $3;
  }
  | TERM '/' FACTOR {
    $$ = $1 / $3;
  }
  | FACTOR {
    $$ = $1;
  }

FACTOR:
  ENTIER {
    $$ = $1;
  }
  | '-' FACTOR {
    $$ = -$2;
  }
  | VARIABLE {
    $$ = var[$1-'a'];
  }
  | '(' EXPR ')' {
    $$ = $2;
  }

VARIABLE:
  VAR {
    $$ = $1;
  }
  | VARIABLE '=' VALUE {
    var[$1-'a'] = $3;
    $$ = $1;
  }

%%

int main() {
  yyparse();
  return 0;
}
