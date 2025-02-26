#ifndef NOTAS_H
#define NOTAS_H

// Frequências das notas musicais (em Hz)
#define DO 275
#define RE 294
#define MI 330
#define FA 349
#define SOL 392
#define LA 440
#define SI 494
#define DO_ALTO 530

// Estrutura para representar uma nota
typedef struct {
    int freq;
    int duracao;
} Nota;

#endif