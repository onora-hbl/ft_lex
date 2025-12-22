 #include <stdio.h>
%{
/* ===== Header C code ===== */

int line = 1;
%}

DIGIT      [0-9]
LETTER     [a-zA-Z]
ID         {LETTER}({LETTER}|{DIGIT})*

%pointer
%n 1000

%x COMMENT

%%
"/*"              { BEGIN(COMMENT); printf("COMMENT_START at line %d\n", line); }
<COMMENT>"*/"     { BEGIN(INITIAL); printf("COMMENT_END at line %d\n", line); }
<COMMENT>\n       { line++; }
<COMMENT>.        ;

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
