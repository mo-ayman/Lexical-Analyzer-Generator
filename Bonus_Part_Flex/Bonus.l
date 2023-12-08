/*** Definitions section ***/
addop [\+|\-]
mulop [\*|/]
assign "="
num ([0-9]+)?(([0-9]+\.[0-9]+((\L)?(E[0-9]+)?)))?
id [a-zA-Z_][a-zA-Z0-9_]*
relop (\=\=)?(\!\=)?(\>)?(\>\=)?(\<)?(\<\=)?
keyword (boolean)?(if)?(while)?(for)?(float)?(else)?(int)?
Punctuation [\{|\}|\(|\)|\;|\,]

/*** Rules section ***/
%%
{keyword}     {printf("%s\n",yytext);}
{addop}       {printf("addop\n");}
{mulop}       {printf("mulop\n");}
{num}         {printf("num\n");} 
{relop}       {printf("relop\n");}
{assign}      {printf("assign\n");}
{Punctuation} {printf("%s\n",yytext);}
{id}          {printf("id\n");}
[ \t\r\n] 
.             {printf("Error\n");}
%%
 
/*** User code section***/
int yywrap() {
    return 0;
}

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    yylex();
    return 0;
}

