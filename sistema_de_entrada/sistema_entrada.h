#ifndef INC_01_SISTEMA_ENTRADA_H
#define INC_01_SISTEMA_ENTRADA_H

/**
 * Inicializa el sistema de entrada para leer desde un archivo.
 *
 * @param nombre_archivo El nombre del archivo desde el cual leer.
 */
void iniciar_sistema_entrada(char *nombre_archivo);

/**
 * Obtiene el siguiente carácter del archivo.
 *
 * @return El siguiente carácter leído del archivo.
 */
int siguiente_caracter();

/**
 * Obtiene el lexema actualmente apuntado por el sistema de entrada.
 *
 * @return Un puntero a la cadena de caracteres que representa el lexema.
 */
char *obtener_lexema();

/**
 * Retrocede un carácter en el archivo.
 */
void retroceder_caracter();

/**
 * Salta al siguiente carácter en el archivo, sin guardarlo en el lexema.
 */
void saltar_caracter();

/**
 * Cierra el sistema de entrada y el archivo asociado.
 */
void cerrar_sistema_de_entrada();

#endif //INC_01_SISTEMA_ENTRADA_H
