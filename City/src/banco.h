#ifndef BANCO_H
#define BANCO_H

#include <stdio.h>

typedef struct quadra Quadra;
typedef struct pessoa Pessoa;

void banco_set_output_dir(char *dir);
void banco_init(void);
void banco_close(void);

// Quadras
void banco_addQuadra(char *cep, double x, double y, double w, double h);
Quadra* banco_getQuadra(char *cep);
void banco_removeQuadra(char *cep);
void banco_setQuadraStyle(char *fill, char *stroke, char *sw);
void banco_desenhar_todas_quadras(void);

// Acessores para Quadra
const char* quadra_get_cep(Quadra *q);
double quadra_get_x(Quadra *q);
double quadra_get_y(Quadra *q);
double quadra_get_w(Quadra *q);
double quadra_get_h(Quadra *q);
const char* quadra_get_fill(Quadra *q);
const char* quadra_get_stroke(Quadra *q);
const char* quadra_get_stroke_width(Quadra *q);

// Pessoas
void banco_addPessoa(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc);
Pessoa* banco_getPessoa(char *cpf);
void banco_removePessoa(char *cpf);
void banco_morar(char *cpf, char *cep, char face, int num, char *compl);
void banco_despejar(char *cpf);
void banco_mudar(char *cpf, char *cep, char face, int num, char *compl);

// Acessores para Pessoa
const char* pessoa_get_cpf(Pessoa *p);
const char* pessoa_get_nome(Pessoa *p);
const char* pessoa_get_sobrenome(Pessoa *p);
char pessoa_get_sexo(Pessoa *p);
const char* pessoa_get_nasc(Pessoa *p);
const char* pessoa_get_cep(Pessoa *p);
char pessoa_get_face(Pessoa *p);
int pessoa_get_num(Pessoa *p);
const char* pessoa_get_compl(Pessoa *p);
int pessoa_eh_morador(Pessoa *p);

// Iteração
Pessoa* banco_getPessoaIndex(int i);
int banco_getPessoaCount(void);
void banco_getQuadraResidents(char *cep, int counts[5]);

// Persistência
void banco_salvar_estado(void);
void banco_carregar_estado(void);
int banco_estado_foi_carregado(void);


#endif