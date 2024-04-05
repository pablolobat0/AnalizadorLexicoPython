# Analizador Léxico para Python con Flex

![C Badge](https://img.shields.io/badge/C-C11-blue) ![GCC Badge](https://img.shields.io/badge/GCC-13.2.1-orange.svg) ![Flex Badge](https://img.shields.io/badge/Flex-2.6.4-red.svg)

## Índice

- [Analizador Léxico para Python con Flex](#analizador-léxico-para-python-con-flex)
  - [Índice](#índice)
  - [Introducción](#introducción)
  - [Funcionalidades](#funcionalidades)
  - [Estructura del Proyecto](#estructura-del-proyecto)
  - [Compilación y Ejecución](#compilación-y-ejecución)
  - [Funcionamiento](#funcionamiento)

## Introducción

Este proyecto consiste en un analizador léxico para el lenguaje de programación Python, implementado en el lenguaje C usando la herramienta Flex. El analizador léxico es una parte fundamental de un compilador o intérprete, ya que se encarga de analizar el código fuente y convertirlo en una secuencia de componentes léxicos que posteriormente serán utilizados por el analizador sintáctico. Los componentes léxicos identificados son los definidos en la documentación de Python, concretamente en el [análisis léxico](https://docs.python.org/3/reference/lexical_analysis.html)

## Funcionalidades

- **Análisis de Lexemas**: El analizador léxico reconoce y clasifica los diferentes lexemas presentes en el código fuente, como palabras clave, identificadores, literales, operadores, delimitadores, etc. Este proceso se realiza mediante Flex empleando expresiones regulares para reconocer los diferentes componentes léxicos.
- **Tabla de Símbolos**: Emplea una tabla de símbolos basada en una tabla hash con encadenamiento para almacenar y gestionar los identificadores presentes en el código fuente, lo que facilita su acceso y búsqueda durante el proceso de análisis. Esta tabla dispone de un mecanismo de redispersión para ampliar su capacidad y redispersar sus elementos cuando el factor de carga sea superior a 0,5.

## Estructura del Proyecto

El proyecto está organizado en los siguientes directorios:

- `analizador_lexico`: Contiene la implementación del analizador léxico, que en este caso envuelve a flex.
- `flex`: Contiene los ficheros de la herramienta flex, el archivo con las definiciones y los .c y .h correspondientes.
- `analizador_sintactico`: Contiene la implementación del analizador sintáctico.
- `gestion_de_errores`: Contiene funciones para la gestión de errores durante el análisis.
- `tabla_de_simbolos`: Contiene la implementación de la tabla de símbolos.
- `tabla_hash`: Implementa la estructura de datos de la tabla hash utilizada en la tabla de símbolos.
- `main.c`: Archivo principal que contiene la función `main` del programa.

## Compilación y Ejecución

Para compilar el proyecto, puedes utilizar el siguiente comando:

```bash
make
```

Esto generará el ejecutable analizadorLexicoPython. Para ejecutarlo, puedes utilizar el siguiente comando:

```bash
./analizadorLexicoPython archivo.py
```

Para eliminar los archivos resultantes de la compilación del código puedes emplear el siguiente comando:

```bash
make clean
```

## Funcionamiento

1. Se comprueba si el número de argumentos es correcto y si el archivo del código fuente es un archivo de Python. Tras eso, se inicia el sistema de entrada y se intenta abrir el archivo.

2. Se inicia la tabla de símbolos y se introducen en ella las palabras reservadas de Python.

3. Se inicia el analizador sintáctico, que funciona con el esquema productor-consumidor con el analizador léxico, de modo que el analizador sintáctico pide lexemas y el analizador léxico, a través de flex, reconoce los distintos componentes léxicos mediante las expresiones regulares y se los devuelve al sintáctico, de tratarse de un identificador los busque en la tabla de símbolos, y si no está se inserta. Cuando el analizador léxico imprime el componente léxico lo hace de la siguiente forma: < Lexema Componente Léxico >. Esta dinámica se repite hasta que se llega hasta el final del archivo fuente que se termina el análisis.
