#ifndef INC_01_SISTEMA_ENTRADA_H
#define INC_01_SISTEMA_ENTRADA_H

void iniciar_sistema_entrada(char *nombre_archivo);

int siguiente_caracter();

char *get_lexema();

void retroceder_caracter();

void saltar_caracter();

void cerrar_sistema_de_entrada();

#endif //INC_01_SISTEMA_ENTRADA_H
