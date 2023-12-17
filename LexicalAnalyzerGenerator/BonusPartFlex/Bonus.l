/*** Definitions section ***/
letter [[:alpha:]]
digit [[:digit:]]
id {letter}({letter}|{digit})*
digits {digit}+
boolean boolean
int int
float float
num ({digit}+)|({digit}+\.{digits}(E{digits})?)
relop (==)|(!=)|(>)|(>=)|(<)|(<=)
assign =
if if
else else
while while
Punctuation [{}();,]
addop [+-]
mulop [*\/]

/*** Rules section ***/
%%
{mulop}       { printf("mulop\n"); }
{addop}       { printf("addop\n"); }
{Punctuation} { printf("%s\n",yytext); }
{while}       { printf("%s\n",yytext); }
{else}        { printf("%s\n",yytext); }
{if}          { printf("%s\n",yytext); }
{assign}      { printf("assign\n"); }
{relop}       { printf("relop\n"); }
{num}         { printf("num\n"); }
{float}       { printf("%s\n",yytext); }
{int}         { printf("%s\n",yytext); }
{boolean}     { printf("%s\n",yytext); }
{id}          { printf("id\n"); }
[ \t\r\n] 
.             { printf("Error\n"); }
%%
 
/*** User code section***/
int yywrap() {
    return 1;
}

int main(int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    yylex();
    return 0;
}

