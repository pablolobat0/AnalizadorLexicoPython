#ifndef INC_01_DEFINICIONES_H
#define INC_01_DEFINICIONES_H

typedef struct {
    char *lexema;
    int componente_lexico;
} ComponenteLexico;

#define ID 300
// Palabras reservadas
#define FALSE 301
#define NONE 302
#define TRUE 303
#define AND 304
#define AS 305
#define ASSERT 306
#define ASYNC 307
#define AWAIT 308
#define EXCEPT 309
#define FINALLY 310
#define FOR 311
#define FROM 312
#define GLOBAL 313
#define IF 314
#define IMPORT 315
#define IN 316
#define IS 317
#define LAMBDA 318
#define NONLOCAL 319
#define NOT 320
#define OR 321
#define PASS 322
#define RAISE 323
#define RETURN 324
#define TRY 325
#define WHILE 326
#define WITH 327
#define YIELD 328
// Operadores
#define DOUBLE_SLASH 329
#define LEFT_SHIFT 330
#define RIGHT_SHIFT 331
#define ASSIGMENT 332
#define LESS_OR_EQUAL 333
#define GREATER_OR_EQUAL 334
#define EQUAL_EQUAL 335
#define NOT_EQUAL 336
// Delimitadores
#define ARROW_OPERATOR 337
#define PLUS_EQUAL 338
#define MINUS_EQUAL 339
#define STAR_EQUAL 340
#define DIVIDE_EQUAL 341
#define FLOOR_DIVIDE_EQUAL 342
#define MODULO_EQUAL 343
#define AT_EQUAL 344
#define AND_EQUAL 345
#define OR_EQUAL 346
#define XOR_EQUAL 347
#define RIGHT_SHIFT_EQUAL 348
#define LEFT_SHIFT_EQUAL 349
#define DOUBLE_STAR_EQUAL 350


#endif //INC_01_DEFINICIONES_H
