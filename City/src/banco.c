#include "banco.h"
#include "hashfile.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @file banco.c
 * @brief Implementação do banco de dados
 * 
 * Este arquivo contém a implementação completa do banco de dados,
 * incluindo as estruturas de dados (que são opacas no .h)
 */

/* ==================== ESTRUTURAS DE DADOS ==================== */

/**
 * Estrutura que representa uma quadra
 * Os campos são acessíveis apenas através das funções acessoras
 */
struct quadra {
    char cep[20];           // CEP da quadra (chave)
    double x, y;            // Coordenadas do canto superior esquerdo
    double w, h;            // Largura e altura
    char fill[20];          // Cor de preenchimento
    char stroke[20];        // Cor da borda
    char stroke_width[20];    // Espessura da borda
};

/**
 * Estrutura que representa uma pessoa
 * Os campos são acessíveis apenas através das funções acessoras
 */
struct pessoa {
    char cpf[20];           // CPF (chave)
    char nome[50];          // Primeiro nome
    char sobrenome[50];     // Sobrenome
    char sexo;              // 'M' ou 'F'
    char nasc[20];          // Data de nascimento
    char cep[20];           // CEP onde mora (vazio se sem-teto)
    char face;              // Face da quadra ('N','S','L','O')
    int num;                // Número do endereço
    char compl[100];        // Complemento
    int ehMorador;          // 1 se mora em algum lugar, 0 se sem-teto
};

/* ==================== VARIÁVEIS ESTÁTICAS ==================== */

static Hash hQuadra;        // Hashfile para quadras
static Hash hPessoa;        // Hashfile para pessoas
static Pessoa *pessoas = NULL;  // Array de pessoas (fonte da verdade)
static int pCount = 0;      // Número atual de pessoas
static int pCapacity = 0;   // Capacidade do array (para realloc)
static char current_fill[20] = "lightblue";    // Cor padrão das quadras
static char current_stroke[20] = "black";      // Cor da borda padrão
static char current_sw[20] = "1.0px";            // Espessura da borda padrão

/* ==================== ACESSORES PARA QUADRA ==================== */
// Estas funções permitem acesso seguro aos campos da struct quadra

const char* quadra_get_cep(Quadra *q) { return q ? q->cep : NULL; }
double quadra_get_x(Quadra *q) { return q ? q->x : 0; }
double quadra_get_y(Quadra *q) { return q ? q->y : 0; }
double quadra_get_w(Quadra *q) { return q ? q->w : 0; }
double quadra_get_h(Quadra *q) { return q ? q->h : 0; }
const char* quadra_get_fill(Quadra *q) { return q ? q->fill : NULL; }
const char* quadra_get_stroke(Quadra *q) { return q ? q->stroke : NULL; }
const char* quadra_get_stroke_width(Quadra *q) { return q ? q->stroke_width : NULL; }

/* ==================== ACESSORES PARA PESSOA ==================== */

const char* pessoa_get_cpf(Pessoa *p) { return p ? p->cpf : NULL; }
const char* pessoa_get_nome(Pessoa *p) { return p ? p->nome : NULL; }
const char* pessoa_get_sobrenome(Pessoa *p) { return p ? p->sobrenome : NULL; }
char pessoa_get_sexo(Pessoa *p) { return p ? p->sexo : 0; }
const char* pessoa_get_nasc(Pessoa *p) { return p ? p->nasc : NULL; }
const char* pessoa_get_cep(Pessoa *p) { return p ? p->cep : NULL; }
char pessoa_get_face(Pessoa *p) { return p ? p->face : 0; }
int pessoa_get_num(Pessoa *p) { return p ? p->num : 0; }
const char* pessoa_get_compl(Pessoa *p) { return p ? p->compl : NULL; }
int pessoa_eh_morador(Pessoa *p) { return p ? p->ehMorador : 0; }

/* ==================== FUNÇÕES DO BANCO ==================== */

void banco_init(void) {
    // Cria hashfiles com 101 buckets iniciais
    hQuadra = hf_create("quadras.hf", 101, sizeof(Quadra));
    hPessoa = hf_create("pessoas.hf", 101, sizeof(Pessoa));
    
    // Aloca array dinâmico para pessoas (começa com capacidade 1000)
    pCapacity = 1000;
    pessoas = (Pessoa*)malloc(pCapacity * sizeof(Pessoa));
    pCount = 0;
}

void banco_close(void) {
    // Gera arquivos de dump para debug
    hf_dump(hQuadra, "quadras");
    hf_dump(hPessoa, "pessoas");
    
    // Fecha hashfiles
    hf_close(hQuadra);
    hf_close(hPessoa);
    
    // Libera array de pessoas
    free(pessoas);
    pessoas = NULL;
}

/* ==================== OPERAÇÕES COM QUADRAS ==================== */

void banco_addQuadra(char *cep, double x, double y, double w, double h) {
    // Aloca quadra temporária
    Quadra *q = (Quadra*)malloc(sizeof(Quadra));
    strcpy(q->cep, cep);
    q->x = x; q->y = y; q->w = w; q->h = h;
    strcpy(q->fill, current_fill);
    strcpy(q->stroke, current_stroke);
    strcpy(q->stroke_width, current_sw);     
    // Insere no hashfile
    hf_insert(hQuadra, cep, q);
    
    // Libera memória (hashfile fez cópia)
    free(q);
}

Quadra* banco_getQuadra(char *cep) {
    // Busca no hashfile
    return (Quadra*)hf_search(hQuadra, cep);
}

void banco_removeQuadra(char *cep) {
    // Primeiro, torna sem-teto todos os moradores desta quadra
    for (int i = 0; i < pCount; i++) {
        if (pessoas[i].ehMorador && strcmp(pessoas[i].cep, cep) == 0) {
            pessoas[i].ehMorador = 0;
            pessoas[i].cep[0] = '\0';
            
            // Atualiza também no hashfile
            Pessoa *p = banco_getPessoa(pessoas[i].cpf);
            if (p) {
                p->ehMorador = 0;
                p->cep[0] = '\0';
                hf_insert(hPessoa, pessoas[i].cpf, p);
                free(p);
            }
        }
    }
    
    // Remove a quadra
    hf_remove(hQuadra, cep);
}

void banco_setQuadraStyle(char *fill, char *stroke, char *sw) {
    // Atualiza as variáveis estáticas para próximas quadras
    strcpy(current_fill, fill);
    strcpy(current_stroke, stroke);
    strcpy(current_sw, sw);
}
/* ==================== OPERAÇÕES COM PESSOAS ==================== */

void banco_addPessoa(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc) {
    // Expande array se necessário
    if (pCount >= pCapacity) {
        pCapacity *= 2;
        pessoas = (Pessoa*)realloc(pessoas, pCapacity * sizeof(Pessoa));
    }
    
    // Preenche dados no array
    Pessoa *p = &pessoas[pCount++];
    strcpy(p->cpf, cpf);
    strcpy(p->nome, nome);
    strcpy(p->sobrenome, sobrenome);
    p->sexo = sexo;
    strcpy(p->nasc, nasc);
    p->ehMorador = 0;
    p->cep[0] = '\0';
    p->compl[0] = '\0';
    
    // Insere no hashfile
    hf_insert(hPessoa, cpf, p);
}

Pessoa* banco_getPessoa(char *cpf) {
    return (Pessoa*)hf_search(hPessoa, cpf);
}

void banco_removePessoa(char *cpf) {
    // Remove do hashfile
    hf_remove(hPessoa, cpf);
    
    // Remove do array (shift para a esquerda)
    for (int i = 0; i < pCount; i++) {
        if (strcmp(pessoas[i].cpf, cpf) == 0) {
            for (int j = i; j < pCount - 1; j++) {
                pessoas[j] = pessoas[j + 1];
            }
            pCount--;
            break;
        }
    }
}

void banco_morar(char *cpf, char *cep, char face, int num, char *compl) {
    // Atualiza no array pessoas (fonte da verdade)
    for (int i = 0; i < pCount; i++) {
        if (strcmp(pessoas[i].cpf, cpf) == 0) {
            strcpy(pessoas[i].cep, cep);
            pessoas[i].face = face;
            pessoas[i].num = num;
            strcpy(pessoas[i].compl, compl);
            pessoas[i].ehMorador = 1;
            break;
        }
    }
    
    // Atualiza no hashfile também
    Pessoa *p = banco_getPessoa(cpf);
    if (p) {
        strcpy(p->cep, cep);
        p->face = face;
        p->num = num;
        strcpy(p->compl, compl);
        p->ehMorador = 1;
        hf_insert(hPessoa, cpf, p);
        free(p);
    }
}

void banco_despejar(char *cpf) {
    // Torna sem-teto no array
    for (int i = 0; i < pCount; i++) {
        if (strcmp(pessoas[i].cpf, cpf) == 0) {
            pessoas[i].ehMorador = 0;
            pessoas[i].cep[0] = '\0';
            break;
        }
    }
    
    // Atualiza no hashfile
    Pessoa *p = banco_getPessoa(cpf);
    if (p && p->ehMorador) {
        p->ehMorador = 0;
        p->cep[0] = '\0';
        hf_insert(hPessoa, cpf, p);
        free(p);
    }
}

void banco_mudar(char *cpf, char *cep, char face, int num, char *compl) {
    // Verifica se a quadra de destino existe
    Quadra *q_destino = banco_getQuadra(cep);
    if (!q_destino) {
        return;  // Quadra não existe, não pode mudar
    }
    free(q_destino);
    
    // Atualiza no array pessoas
    for (int i = 0; i < pCount; i++) {
        if (strcmp(pessoas[i].cpf, cpf) == 0) {
            strcpy(pessoas[i].cep, cep);
            pessoas[i].face = face;
            pessoas[i].num = num;
            strcpy(pessoas[i].compl, compl);
            pessoas[i].ehMorador = 1;
            break;
        }
    }
    
    // Atualiza no hashfile
    Pessoa *p = banco_getPessoa(cpf);
    if (p) {
        strcpy(p->cep, cep);
        p->face = face;
        p->num = num;
        strcpy(p->compl, compl);
        p->ehMorador = 1;
        hf_insert(hPessoa, cpf, p);
        free(p);
    }
}

/* ==================== FUNÇÕES DE ITERAÇÃO ==================== */

Pessoa* banco_getPessoaIndex(int i) {
    if (i >= 0 && i < pCount) return &pessoas[i];
    return NULL;
}

int banco_getPessoaCount(void) {
    return pCount;
}

void banco_getQuadraResidents(char *cep, int counts[5]) {
    // Inicializa contadores
    for (int i = 0; i < 5; i++) counts[i] = 0;
    
    // Percorre todas as pessoas
    for (int i = 0; i < pCount; i++) {
        if (pessoas[i].ehMorador && strcmp(pessoas[i].cep, cep) == 0) {
            counts[0]++;  // Total
            char face = pessoas[i].face;
            
            // Converte W (West) para O (Oeste) e E (East) para L (Leste)
            if (face == 'W') face = 'O';
            if (face == 'E') face = 'L';
            
            // Incrementa contador da face correspondente
            if (face == 'N') counts[1]++;      // Norte
            else if (face == 'S') counts[2]++; // Sul
            else if (face == 'L') counts[3]++; // Leste
            else if (face == 'O') counts[4]++; // Oeste
        }
    }
}
