%{
#include <stdio.h>
#include "gestion_de_errores/gestion_de_errores.h"
#include "tabla_de_simbolos/definiciones.h"

void abrir_archivo_fuente(char *nombre_archivo);
void siguiente_componente_lexico_flex(ComponenteLexico *componente_lexico);
void cerrar();

%}

%option noyywrap noinput nounput

DELIMITADOR    [ \t\n\r]
ESPACIADO    {DELIMITADOR}+

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
POINTFLOAT {DIGITPART}{FRACTION}|{DIGITPART}\.
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

[ \t\n\r]+              { }
{COMENTARIOS}               {}

{IDENTIFICADOR}           { return ID; }
{STRINGLITERAL}           { return STRING; }
{IMAGNUMBER}              { return IMAGINARY; }
{FLOATNUMBER}             { return FLOAT; }
{INTEGER}                 { return INT; }


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

void cerrar(){
    fclose(yyin);
    yylex_destroy();
}

