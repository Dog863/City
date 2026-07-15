#include "banco.h"
#include "hashfile.h"
#include "svg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// ============================================================
// ESTRUTURAS DE DADOS (opacas - definidas apenas no .c)
// ============================================================

struct quadra {
    char cep[20];
    double x, y;
    double w, h;
    char fill[20];
    char stroke[20];
    char stroke_width[20];
};

struct pessoa {
    char cpf[20];
    char nome[50];
    char sobrenome[50];
    char sexo;
    char nasc[20];
    char cep[20];
    char face;
    int num;
    char compl[100];
    int ehMorador;
};

// ============================================================
// VARIÁVEIS ESTÁTICAS
// ============================================================

static Hash hQuadra;
static Hash hPessoa;
static Pessoa *pessoas = NULL;
static int pCount = 0;
static int pCapacity = 0;

static Quadra *todas_quadras = NULL;
static int qCount = 0;
static int qCapacity = 0;

static char current_fill[20] = "lightblue";
static char current_stroke[20] = "black";
static char current_sw[20] = "1.0px";
static char output_dir[PATH_MAX] = ".";
static int estado_carregado = 0;

// ============================================================
// ACESSORES - QUADRA
// ============================================================

const char* quadra_get_cep(Quadra *q) { return q ? q->cep : NULL; }
double quadra_get_x(Quadra *q) { return q ? q->x : 0; }
double quadra_get_y(Quadra *q) { return q ? q->y : 0; }
double quadra_get_w(Quadra *q) { return q ? q->w : 0; }
double quadra_get_h(Quadra *q) { return q ? q->h : 0; }
const char* quadra_get_fill(Quadra *q) { return q ? q->fill : NULL; }
const char* quadra_get_stroke(Quadra *q) { return q ? q->stroke : NULL; }
const char* quadra_get_stroke_width(Quadra *q) { return q ? q->stroke_width : NULL; }

// ============================================================
// ACESSORES - PESSOA
// ============================================================

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

// ============================================================
// PERSISTÊNCIA
// ============================================================

void banco_salvar_estado(void) {
    char estado_path[PATH_MAX];
    snprintf(estado_path, sizeof(estado_path), "%s/estado.txt", output_dir);
    
    FILE *f = fopen(estado_path, "w");
    if (!f) {
        printf("Erro: Não foi possível salvar estado em %s\n", estado_path);
        return;
    }
    
    // Salvar quadras
    for (int i = 0; i < qCount; i++) {
        Quadra *q = &todas_quadras[i];
        fprintf(f, "Q %s %.2f %.2f %.2f %.2f %s %s %s\n",
                q->cep, q->x, q->y, q->w, q->h,
                q->fill, q->stroke, q->stroke_width);
    }
    
    // Salvar pessoas
    for (int i = 0; i < pCount; i++) {
        Pessoa *p = &pessoas[i];
        fprintf(f, "P %s %s %s %c %s %d %s %c %d %s\n",
                p->cpf, p->nome, p->sobrenome, p->sexo, p->nasc,
                p->ehMorador, p->cep, p->face, p->num, p->compl);
    }
    
    fclose(f);
    printf("Estado salvo em: %s\n", estado_path);
}

void banco_carregar_estado(void) {
    char estado_path[PATH_MAX];
    snprintf(estado_path, sizeof(estado_path), "%s/estado.txt", output_dir);
    
    FILE *f = fopen(estado_path, "r");
    if (!f) {
        printf("Nenhum estado anterior encontrado. Iniciando novo.\n");
        estado_carregado = 0;
        return;
    }
    
    char line[1024];
    int quadras_carregadas = 0;
    int pessoas_carregadas = 0;
    
    // Limpar dados existentes antes de carregar
    qCount = 0;
    pCount = 0;
    
    while (fgets(line, sizeof(line), f)) {
        char tipo;
        sscanf(line, "%c", &tipo);
        
        if (tipo == 'Q') {
            Quadra q;
            sscanf(line, "Q %s %lf %lf %lf %lf %s %s %s",
                   q.cep, &q.x, &q.y, &q.w, &q.h,
                   q.fill, q.stroke, q.stroke_width);
            banco_addQuadra(q.cep, q.x, q.y, q.w, q.h);
            quadras_carregadas++;
        } else if (tipo == 'P') {
            Pessoa p;
            char face_str[5];
            sscanf(line, "P %s %s %s %c %s %d %s %s %d %s",
                   p.cpf, p.nome, p.sobrenome, &p.sexo, p.nasc,
                   &p.ehMorador, p.cep, face_str, &p.num, p.compl);
            p.face = face_str[0];
            banco_addPessoa(p.cpf, p.nome, p.sobrenome, p.sexo, p.nasc);
            if (p.ehMorador) {
                banco_morar(p.cpf, p.cep, p.face, p.num, p.compl);
            }
            pessoas_carregadas++;
        }
    }
    
    fclose(f);
    printf("Estado carregado: %d quadras, %d pessoas\n", quadras_carregadas, pessoas_carregadas);
    estado_carregado = 1;
}

int banco_estado_foi_carregado(void) {
    return estado_carregado;
}

// ============================================================
// FUNÇÕES DO BANCO
// ============================================================

void banco_set_output_dir(char *dir) {
    if (dir && dir[0] != '\0') {
        strncpy(output_dir, dir, PATH_MAX - 1);
        output_dir[PATH_MAX - 1] = '\0';
    }
}

void banco_init(void) {
    // Os nomes dos arquivos serão definidos pelo hf_set_base_name
    hQuadra = hf_create("quadras", 101, sizeof(Quadra));
    hPessoa = hf_create("pessoas", 101, sizeof(Pessoa));
    
    pCapacity = 1000;
    pessoas = (Pessoa*)malloc(pCapacity * sizeof(Pessoa));
    pCount = 0;
    
    qCapacity = 1000;
    todas_quadras = (Quadra*)malloc(qCapacity * sizeof(Quadra));
    qCount = 0;
    
    // Carregar estado anterior
    banco_carregar_estado();
}

void banco_close(void) {
    // Salvar estado antes de fechar
    banco_salvar_estado();
    
    hf_dump(hQuadra, "quadras");
    hf_dump(hPessoa, "pessoas");
    
    hf_close(hQuadra);
    hf_close(hPessoa);
    
    free(pessoas);
    free(todas_quadras);
    pessoas = NULL;
    todas_quadras = NULL;
}

// ============================================================
// OPERAÇÕES COM QUADRAS
// ============================================================

void banco_addQuadra(char *cep, double x, double y, double w, double h) {
    // Verificar se já existe
    for (int i = 0; i < qCount; i++) {
        if (strcmp(todas_quadras[i].cep, cep) == 0) {
            return; // Já existe
        }
    }
    
    // Adicionar ao array para desenho posterior
    if (qCount >= qCapacity) {
        qCapacity = qCapacity == 0 ? 100 : qCapacity * 2;
        todas_quadras = (Quadra*)realloc(todas_quadras, qCapacity * sizeof(Quadra));
    }
    
    Quadra *q = &todas_quadras[qCount++];
    strcpy(q->cep, cep);
    q->x = x; q->y = y; q->w = w; q->h = h;
    strcpy(q->fill, current_fill);
    strcpy(q->stroke, current_stroke);
    strcpy(q->stroke_width, current_sw);
    
    // Também inserir no hashfile
    hf_insert(hQuadra, cep, q);
}

Quadra* banco_getQuadra(char *cep) {
    return (Quadra*)hf_search(hQuadra, cep);
}

void banco_removeQuadra(char *cep) {
    // Remover do array de quadras
    for (int i = 0; i < qCount; i++) {
        if (strcmp(todas_quadras[i].cep, cep) == 0) {
            for (int j = i; j < qCount - 1; j++) {
                todas_quadras[j] = todas_quadras[j + 1];
            }
            qCount--;
            break;
        }
    }
    
    // Remover do hashfile
    hf_remove(hQuadra, cep);
}

void banco_desenhar_todas_quadras(void) {
    for (int i = 0; i < qCount; i++) {
        Quadra *q = &todas_quadras[i];
        svg_rect(q->x, q->y, q->w, q->h, q->fill);
        svg_text(q->x + 5, q->y + 15, q->cep);
    }
}

void banco_setQuadraStyle(char *fill, char *stroke, char *sw) {
    strcpy(current_fill, fill);
    strcpy(current_stroke, stroke);
    strcpy(current_sw, sw);
}

// ============================================================
// OPERAÇÕES COM PESSOAS
// ============================================================

void banco_addPessoa(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc) {
    // Verificar se já existe
    for (int i = 0; i < pCount; i++) {
        if (strcmp(pessoas[i].cpf, cpf) == 0) {
            return; // Já existe
        }
    }
    
    if (pCount >= pCapacity) {
        pCapacity *= 2;
        pessoas = (Pessoa*)realloc(pessoas, pCapacity * sizeof(Pessoa));
    }
    
    Pessoa *p = &pessoas[pCount++];
    strcpy(p->cpf, cpf);
    strcpy(p->nome, nome);
    strcpy(p->sobrenome, sobrenome);
    p->sexo = sexo;
    strcpy(p->nasc, nasc);
    p->ehMorador = 0;
    p->cep[0] = '\0';
    p->compl[0] = '\0';
    
    hf_insert(hPessoa, cpf, p);
}

Pessoa* banco_getPessoa(char *cpf) {
    return (Pessoa*)hf_search(hPessoa, cpf);
}

void banco_removePessoa(char *cpf) {
    hf_remove(hPessoa, cpf);
    
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
    for (int i = 0; i < pCount; i++) {
        if (strcmp(pessoas[i].cpf, cpf) == 0) {
            pessoas[i].ehMorador = 0;
            pessoas[i].cep[0] = '\0';
            break;
        }
    }
    
    Pessoa *p = banco_getPessoa(cpf);
    if (p && p->ehMorador) {
        p->ehMorador = 0;
        p->cep[0] = '\0';
        hf_insert(hPessoa, cpf, p);
        free(p);
    }
}

void banco_mudar(char *cpf, char *cep, char face, int num, char *compl) {
    Quadra *q_destino = banco_getQuadra(cep);
    if (!q_destino) {
        return;
    }
    free(q_destino);
    
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

// ============================================================
// FUNÇÕES DE ITERAÇÃO
// ============================================================

Pessoa* banco_getPessoaIndex(int i) {
    if (i >= 0 && i < pCount) return &pessoas[i];
    return NULL;
}

int banco_getPessoaCount(void) {
    return pCount;
}

void banco_getQuadraResidents(char *cep, int counts[5]) {
    for (int i = 0; i < 5; i++) counts[i] = 0;
    
    for (int i = 0; i < pCount; i++) {
        if (pessoas[i].ehMorador && strcmp(pessoas[i].cep, cep) == 0) {
            counts[0]++;
            char face = pessoas[i].face;
            
            // Normaliza: W->O, E->L
            if (face == 'W') face = 'O';
            if (face == 'E') face = 'L';
            
            if (face == 'N') counts[1]++;
            else if (face == 'S') counts[2]++;
            else if (face == 'L') counts[3]++;
            else if (face == 'O') counts[4]++;
        }
    }
}
