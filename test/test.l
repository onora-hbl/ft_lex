 #include <stdio.h>
%{
/* ===== Header C code ===== */

int line = 1;
%}

/* ===== Definitions ===== */
DIGIT      [0-9]
LETTER     [a-zA-Z]
ID         {LETTER}({LETTER}|{DIGIT})*

%%
[ \t]+
\n              { line++; }

"if"            { printf("IF at line %d\n", line); }
"else"          { printf("ELSE at line %d\n", line); }

{ID}            {
                    printf("IDENT(%s) at line %d\n", yytext, line);
                }

{DIGIT}+        printf("NUMBER(%s)\n", yytext);

"=="            { printf("EQ\n"); }
"="             { printf("ASSIGN\n"); }

.               { printf("CHAR(%s)\n", yytext); }

%%
/* ===== User code ===== */

int main(void)
{
    yylex();
    return 0;
}
