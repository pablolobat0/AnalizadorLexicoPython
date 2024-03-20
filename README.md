# Analizador Léxico para Python

![C Badge](https://img.shields.io/badge/C-C11-blue) ![GCC Badge](https://img.shields.io/badge/GCC-13.2.1-orange.svg) 



## Índice
- [Analizador Léxico para Python](#analizador-léxico-para-python)
  - [Índice](#índice)
  - [Introducción](#introducción)
  - [Funcionalidades](#funcionalidades)
  - [Estructura del Proyecto](#estructura-del-proyecto)
  - [Compilación y Ejecución](#compilación-y-ejecución)
  - [Funcionamiento](#funcionamiento)

## Introducción

Este proyecto consiste en un analizador léxico para el lenguaje de programación Python, implementado en el lenguaje C. El analizador léxico es una parte fundamental de un compilador o intérprete, ya que se encarga de analizar el código fuente y convertirlo en una secuencia de componentes léxicos que posteriormente serán utilizados por el analizador sintáctico. Los componentes léxicos identificados son los definidos en la documentación de Python, concretamente en el [análisis léxico](https://docs.python.org/3/reference/lexical_analysis.html)

## Funcionalidades

- **Sistema de entrada**: Implementa un buffer con 2 bloques utilizando el modelo de doble centinela. Este enfoque divide el buffer físico en 2 bloques lógicos, con un marcador de fin de archivo (EOF) al final de cada uno. El sistema utiliza dos punteros: uno de inicio y otro delantero, que indican el principio y el final de una cadena, respectivamente. Cada vez que el puntero delantero se mueve, se verifica si el carácter que lee es un EOF. Si es así, se comprueba si está en el final de alguno de los 2 bloques o si está en el final del archivo.
- **Análisis de Lexemas**: El analizador léxico reconoce y clasifica los diferentes lexemas presentes en el código fuente, como palabras clave, identificadores, literales, operadores, delimitadores, etc.
- **Tabla de Símbolos**: Emplea una tabla de símbolos basada en una tabla hash con encadenamiento para almacenar y gestionar los identificadores presentes en el código fuente, lo que facilita su acceso y búsqueda durante el proceso de análisis. Esta tabla dispone de un mecanismo de redispersión para ampliar su capacidad y redispersar sus elementos cuando el factor de carga sea superior a 0,5.
- **Automatización**: Utiliza autómatas finitos deterministas (AFD) para reconocer y procesar diferentes tipos de lexemas de manera eficiente, analizándolos carácter a carácter.

## Estructura del Proyecto

El proyecto está organizado en los siguientes directorios:

- `analizador_lexico`: Contiene la implementación del analizador léxico.
- `analizador_sintactico`: Contiene la implementación del analizador sintáctico.
- `gestion_de_errores`: Contiene funciones para la gestión de errores durante el análisis.
- `sistema_de_entrada`: Implementa funciones para la lectura del código fuente desde archivos.
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

## Funcionamiento

1. Se comprueba si el número de argumentos es correcto y si el archivo del código fuente es un archivo de Python. Tras eso, se inicia el sistema de entrada y se intenta abrir el archivo.
  
2. Se inicia la tabla de símbolos y se introducen en ella las palabras reservadas de Python.

3. Se inicia el analizador sintáctico, que funciona con el esquema productor-consumidor con el analizador léxico, de modo que el analizador sintáctico pide lexemas y el analizador sintáctico va pidiendo carácteres al sistema de entrada uno a uno, hasta que alguno de sus autómatas reconoce el lexema y se lo devuelve al sintáctico. Esta dinámica se repite hasta que se llega hasta el final del archivo fuente que se termina el análisis.