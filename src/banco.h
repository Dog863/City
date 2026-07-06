#ifndef BANCO_H
#define BANCO_H

#include <stdio.h>

/**
 * @file banco.h
 * @brief Banco de dados para o SIG - T1
 * 
 * Gerencia quadras e pessoas usando hashfiles
 */

typedef struct quadra Quadra;
typedef struct pessoa Pessoa;

// ==================== INICIALIZAÇÃO ====================

/**
 * Define o diretório de saída para os arquivos
 */
void banco_set_output_dir(char *dir);

/**
 * Inicializa o banco de dados
 */
void banco_init(void);

/**
 * Fecha o banco de dados e salva estado
 */
void banco_close(void);

// ==================== QUADRAS ====================

/**
 * Adiciona uma nova quadra
 */
void banco_addQuadra(char *cep, double x, double y, double w, double h);

/**
 * Busca uma quadra pelo CEP
 * @return Ponteiro para Quadra (deve ser liberado com free)
 */
Quadra* banco_getQuadra(char *cep);

/**
 * Remove uma quadra
 */
void banco_removeQuadra(char *cep);

/**
 * Define o estilo padrão das quadras
 */
void banco_setQuadraStyle(char *fill, char *stroke, char *sw);

/**
 * Desenha todas as quadras no SVG
 */
void banco_desenhar_todas_quadras(void);

// ==================== ACESSORES QUADRA ====================

const char* quadra_get_cep(Quadra *q);
double quadra_get_x(Quadra *q);
double quadra_get_y(Quadra *q);
double quadra_get_w(Quadra *q);
double quadra_get_h(Quadra *q);
const char* quadra_get_fill(Quadra *q);
const char* quadra_get_stroke(Quadra *q);
const char* quadra_get_stroke_width(Quadra *q);

// ==================== PESSOAS ====================

/**
 * Adiciona uma nova pessoa (habitante)
 */
void banco_addPessoa(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc);

/**
 * Busca uma pessoa pelo CPF
 * @return Ponteiro para Pessoa (deve ser liberado com free)
 */
Pessoa* banco_getPessoa(char *cpf);

/**
 * Remove uma pessoa
 */
void banco_removePessoa(char *cpf);

/**
 * Registra que uma pessoa mora em um endereço
 */
void banco_morar(char *cpf, char *cep, char face, int num, char *compl);

/**
 * Despeja uma pessoa (torna sem-teto)
 */
void banco_despejar(char *cpf);

/**
 * Muda uma pessoa para outro endereço
 */
void banco_mudar(char *cpf, char *cep, char face, int num, char *compl);

// ==================== ACESSORES PESSOA ====================

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

// ==================== ITERAÇÃO ====================

/**
 * Obtém uma pessoa pelo índice (para iteração)
 */
Pessoa* banco_getPessoaIndex(int i);

/**
 * Número total de pessoas
 */
int banco_getPessoaCount(void);

/**
 * Conta moradores de uma quadra por face
 * counts[0] = total, counts[1]=N, counts[2]=S, counts[3]=L, counts[4]=O
 */
void banco_getQuadraResidents(char *cep, int counts[5]);

// ==================== PERSISTÊNCIA ====================

/**
 * Salva o estado atual em arquivo
 */
void banco_salvar_estado(void);

/**
 * Carrega o estado de arquivo
 */
void banco_carregar_estado(void);

/**
 * Verifica se o estado foi carregado
 */
int banco_estado_foi_carregado(void);

#endif