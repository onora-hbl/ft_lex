TEST e|f
%%
a(b|c)*d{TEST}+"jkl"{3,5}  printf("Matched complex pattern: %s\n", yytext);
.   printf("Unknown character: %c\n", yytext[0]);
%%
int main(void)
{
    yylex();
    return 0;
}