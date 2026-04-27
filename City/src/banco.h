#ifndef BANCO_H
#define BANCO_H

/**
 * @file banco.h
 * @brief Interface do banco de dados para gerenciar quadras e pessoas
 * 
 * Este módulo gerencia o armazenamento de quadras e pessoas usando hashfiles.
 * As estruturas são opacas (implementation hiding) - os detalhes internos
 * estão escondidos no arquivo .c
 */

// Tipos opacos - implementação escondida no .c
// O usuário só vê ponteiros para essas structs, não seus campos
typedef struct quadra Quadra;
typedef struct pessoa Pessoa;

/* ==================== FUNÇÕES DO BANCO ==================== */

/**
 * Inicializa o banco de dados
 * - Cria os hashfiles para quadras e pessoas
 * - Aloca memória para o array de pessoas
 * - Deve ser chamada antes de qualquer outra função
 */
void banco_init(void);

/**
 * Finaliza o banco de dados
 * - Gera arquivos .hfd (dump dos hashfiles)
 * - Fecha os arquivos e libera memória
 * - Deve ser chamada ao final do programa
 */
void banco_close(void);

/* ==================== OPERAÇÕES COM QUADRAS ==================== */

/**
 * Adiciona uma nova quadra ao banco
 * @param cep CEP da quadra (chave única)
 * @param x Coordenada X do canto superior esquerdo
 * @param y Coordenada Y do canto superior esquerdo
 * @param w Largura da quadra
 * @param h Altura da quadra
 */
void banco_addQuadra(char *cep, double x, double y, double w, double h);

/**
 * Busca uma quadra pelo CEP
 * @param cep CEP da quadra a buscar
 * @return Ponteiro para a quadra (deve ser liberado com free()) ou NULL se não existir
 */
Quadra* banco_getQuadra(char *cep);

/**
 * Remove uma quadra do banco
 * @param cep CEP da quadra a remover
 * Observação: Moradores da quadra removida se tornam sem-teto
 */
void banco_removeQuadra(char *cep);

/**
 * Define o estilo padrão para novas quadras
 * @param fill Cor de preenchimento (ex: "red", "blue", "lightblue")
 * @param stroke Cor da borda
 * @param sw Espessura da borda em pixels
 */
void banco_setQuadraStyle(char *fill, char *stroke, char *sw);

/* ==================== ACESSORES PARA QUADRA ==================== */
// Estas funções permitem acessar campos da struct opaca Quadra

const char* quadra_get_cep(Quadra *q);
double quadra_get_x(Quadra *q);
double quadra_get_y(Quadra *q);
double quadra_get_w(Quadra *q);
double quadra_get_h(Quadra *q);
const char* quadra_get_fill(Quadra *q);
const char* quadra_get_stroke(Quadra *q);
const char* quadra_get_stroke_width(Quadra *q);

/* ==================== OPERAÇÕES COM PESSOAS ==================== */

/**
 * Adiciona uma nova pessoa ao banco
 * @param cpf CPF da pessoa (chave única)
 * @param nome Primeiro nome
 * @param sobrenome Sobrenome
 * @param sexo 'M' ou 'F'
 * @param nasc Data de nascimento (formato dd/mm/aaaa)
 */
void banco_addPessoa(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc);

/**
 * Busca uma pessoa pelo CPF
 * @param cpf CPF da pessoa
 * @return Ponteiro para a pessoa (deve ser liberado com free()) ou NULL
 */
Pessoa* banco_getPessoa(char *cpf);

/**
 * Remove uma pessoa do banco
 * @param cpf CPF da pessoa a remover
 */
void banco_removePessoa(char *cpf);

/**
 * Registra que uma pessoa mora em um endereço
 * @param cpf CPF da pessoa
 * @param cep CEP da quadra
 * @param face Face da quadra ('N', 'S', 'L', 'O')
 * @param num Número do endereço (distância da âncora)
 * @param compl Complemento (apto, casa, etc)
 */
void banco_morar(char *cpf, char *cep, char face, int num, char *compl);

/**
 * Despeja uma pessoa (torna sem-teto)
 * @param cpf CPF da pessoa
 */
void banco_despejar(char *cpf);

/**
 * Muda uma pessoa de endereço
 * @param cpf CPF da pessoa
 * @param cep Novo CEP
 * @param face Nova face
 * @param num Novo número
 * @param compl Novo complemento
 */
void banco_mudar(char *cpf, char *cep, char face, int num, char *compl);

/* ==================== ACESSORES PARA PESSOA ==================== */

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

/* ==================== FUNÇÕES DE ITERAÇÃO ==================== */

/**
 * Obtém uma pessoa pelo índice (para iterar sobre todas)
 * @param i Índice (0 a banco_getPessoaCount()-1)
 * @return Ponteiro para a pessoa (NÃO precisa liberar) ou NULL
 */
Pessoa* banco_getPessoaIndex(int i);

/**
 * Retorna o número total de pessoas cadastradas
 */
int banco_getPessoaCount(void);

/**
 * Conta moradores de uma quadra por face
 * @param cep CEP da quadra
 * @param counts Array de 5 inteiros para resultados:
 *        counts[0] = total
 *        counts[1] = Norte
 *        counts[2] = Sul
 *        counts[3] = Leste
 *        counts[4] = Oeste
 */
void banco_getQuadraResidents(char *cep, int counts[5]);

#endif
