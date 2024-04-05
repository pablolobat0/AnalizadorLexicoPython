%{
#include <stdio.h>
#include "gestion_de_errores/gestion_de_errores.h"
#include "tabla_de_simbolos/definiciones.h"
#include "tabla_de_simbolos/tabla_de_simbolos.h"

void abrir_archivo_fuente(char *nombre_archivo);
void siguiente_componente_lexico_flex(ComponenteLexico *componente_lexico);
void cerrar();

%}

%option noyywrap noinput nounput

ESPACIADO    [ \t\n\r]+

DIGIT [0-9]
NONZERODIGIT [1-9]
BINDIGIT [01]
OCTDIGIT [0-7]
HEXDIGIT [0-9a-fA-F]*
DECINTEGER ({NONZERODIGIT}(_|{DIGIT})*)|(0+(_0)*)
BININTEGER 0[bB](_|{BINDIGIT})+
OCTINTEGER 0[oO](_|{OCTDIGIT})+
HEXINTEGER 0[xX](_|{HEXDIGIT})+
INTEGER ({BININTEGER}|{OCTINTEGER}|{HEXINTEGER}|{DECINTEGER})

DIGITPART {DIGIT}(_|{DIGIT})*
EXPONENT (e|E)(\+|\-)?{DIGITPART}
FRACTION \.{DIGITPART}
POINTFLOAT {DIGITPART}?{FRACTION}|{DIGITPART}\.
EXPONENTFLOAT ({DIGITPART}|{POINTFLOAT}){EXPONENT}
FLOATNUMBER ({POINTFLOAT}|{EXPONENTFLOAT})

IMAGNUMBER ({FLOATNUMBER}|{DIGITPART})[jJ]

LETRA [a-zA-Z]
IDENTIFICADOR ({LETRA}|_)(({LETRA}|{DIGIT})|_)*

LONGSTRING \'\'\'([^\\\'\n]|\\.|\n)*\'\'\'|\"\"\"([^\\\"\n]|\\.|\n)*\"\"\"
SHORTSTRINGLITERAL \"([^\"\\]|(\\[^\n]))*\"
SHORTSTRING \'([^\'\\]|(\\[^\n]))*\'
STRINGLITERAL ({LONGSTRING}|{SHORTSTRING}|{SHORTSTRINGLITERAL})

SIMBOLO    (\.|\,|\;|\:|\(|\)|\{|\}|\[|\]|\>|\<|\=|\+|\-|\*|\/|\@|\%|\&|\||\~|\^)
COMENTARIOS #.*

%%

{ESPACIADO}                 {}
{COMENTARIOS}               {}

{IDENTIFICADOR}           { return ID; }
{STRINGLITERAL}           { return STRING; }
{IMAGNUMBER}              { return IMAGINARY; }
{FLOATNUMBER}             { return FLOAT; }
{INTEGER}                 { return INT; }

"**="    { return DOUBLE_STAR_EQUAL; }
"**"     { return DOUBLE_STAR; }
"//="    { return FLOOR_DIVIDE_EQUAL; }
"//"     { return DOUBLE_SLASH; }
">>="    { return RIGHT_SHIFT_EQUAL; }
">>"     { return RIGHT_SHIFT; }
"<<="    { return LEFT_SHIFT_EQUAL; }
"<<"     { return LEFT_SHIFT; }
">="     { return GREATER_OR_EQUAL; }
"<="     { return LESS_OR_EQUAL; }
"!="     { return NOT_EQUAL; }
"=="     { return EQUAL_EQUAL; }
":="     { return ASSIGMENT; }
"->"     { return ARROW_OPERATOR; }
"+="     { return PLUS_EQUAL; }
"-="     { return MINUS_EQUAL; }
"*="     { return STAR_EQUAL; }
"/="     { return DIVIDE_EQUAL; }
"%="     { return MODULO_EQUAL; }
"@="     { return AT_EQUAL; }
"&="     { return AND_EQUAL; }
"|="     { return OR_EQUAL; }
"^="     { return XOR_EQUAL; }
{SIMBOLO}   {return (int) *yytext;}

.                       { lanzar_error(ERR_CARACTER_NO_RECONOCIDO, -1, -1);}

%%

void abrir_archivo_fuente(char *nombre_archivo) {
    yyin = fopen(nombre_archivo, "r");
    if (yyin == NULL) {
        lanzar_error(ERR_FICHERO_NO_ABRE, -1, -1);
        exit(EXIT_FAILURE);
    }
}

void siguiente_componente_lexico_flex(ComponenteLexico *componente_lexico) {
    componente_lexico->componente_lexico = yylex();
    //caso normal
    if(componente_lexico->componente_lexico != 0){
        componente_lexico->lexema = yytext;
    }
    //caso de ser un ID hay que buscar en la tabla
    if(componente_lexico->componente_lexico == ID){
        componente_lexico->componente_lexico = buscar_e_insertar_en_tabla_de_simbolos(componente_lexico->lexema);
    } else if(componente_lexico->componente_lexico == 0){
        componente_lexico->componente_lexico = -1;
        componente_lexico->lexema = "Final de fichero";
    }
}

void cerrar() {
    fclose(yyin);
    yylex_destroy();
}

