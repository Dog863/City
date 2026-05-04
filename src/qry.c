#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qry.h"
#include "banco.h"
#include "svg.h"

static FILE *txt = NULL;

double getX(Quadra *q, char face, int num) {
    double x = quadra_get_x(q);
    double w = quadra_get_w(q);
    
    if (face == 'W') face = 'O';
    if (face == 'E') face = 'L';
    
    if (face == 'N' || face == 'S') return x + num;
    if (face == 'L') return x + w;
    return x;
}

double getY(Quadra *q, char face, int num) {
    double y = quadra_get_y(q);
    double h = quadra_get_h(q);
    
    if (face == 'W') face = 'O';
    if (face == 'E') face = 'L';
    
    if (face == 'N') return y;
    if (face == 'S') return y + h;
    return y + num;
}

void qry_init(char *f) { 
    txt = fopen(f, "w"); 
    if (txt) {
        setvbuf(txt, NULL, _IONBF, 0);
    }
}

void qry_close(void) { 
    if (txt) {
        fclose(txt);
        txt = NULL;
    }
}

void qry_removeQuadra(char *cep) {
    Quadra *q = banco_getQuadra(cep);
    if (q) {
        for (int i = 0; i < banco_getPessoaCount(); i++) {
            Pessoa *p = banco_getPessoaIndex(i);
            if (p && pessoa_eh_morador(p)) {
                const char *p_cep = pessoa_get_cep(p);
                if (p_cep && strcmp(p_cep, cep) == 0) {
                    fprintf(txt, "%s %s\n", pessoa_get_cpf(p), pessoa_get_nome(p));
                }
            }
        }
        free(q);
    }
    
    banco_removeQuadra(cep);
}

void qry_infoPessoa(char *cpf) {
    Pessoa *p = banco_getPessoa(cpf);
    if (p) {
        fprintf(txt, "%s %s %s %c %s\n", 
                pessoa_get_cpf(p), pessoa_get_nome(p), 
                pessoa_get_sobrenome(p), pessoa_get_sexo(p), 
                pessoa_get_nasc(p));
        
        if (pessoa_eh_morador(p)) {
            fprintf(txt, "Mora em %s/%c/%d %s\n", 
                    pessoa_get_cep(p), pessoa_get_face(p), 
                    pessoa_get_num(p), pessoa_get_compl(p));
            
            Quadra *q = banco_getQuadra((char*)pessoa_get_cep(p));
            if (q) {
                svg_circle(getX(q, pessoa_get_face(p), pessoa_get_num(p)), 
                           getY(q, pessoa_get_face(p), pessoa_get_num(p)), 5, "blue");
                free(q);
            }
        } else {
            fprintf(txt, "Sem-teto\n");
        }
        
        free(p);
    }
}

void qry_mudarPessoa(char *cpf, char *cep, char *face, int num, char *compl) {
    char face_char = face[0];
    
    // Converte W (West) para O (Oeste) e E (East) para L (Leste)
    if (face_char == 'W') face_char = 'O';
    if (face_char == 'E') face_char = 'L';
    
    // Realiza a mudança de endereço no banco de dados
    banco_mudar(cpf, cep, face_char, num, compl);
    
    // Desenha a marcação no SVG
    Quadra *q = banco_getQuadra(cep);
    if (q) {
        double x = getX(q, face_char, num);
        double y = getY(q, face_char, num);
        
        // Quadrado vermelho (tamanho 10x10)
        svg_rect(x - 5, y - 5, 10, 10, "red");
        
        // CPF dentro do quadrado (fonte minúscula/menor)
        // Posiciona o texto centralizado
        svg_text(x - 4, y + 2, cpf);
        
        free(q);
    }
}

void qry_censo(void) {
    int total = banco_getPessoaCount();
    int moradores = 0, sem_teto = 0;
    int homens = 0, mulheres = 0;
    int moradores_homens = 0, moradores_mulheres = 0;
    int sem_teto_homens = 0, sem_teto_mulheres = 0;
    
    for (int i = 0; i < total; i++) {
        Pessoa *p = banco_getPessoaIndex(i);
        if (!p) continue;
        
        char sexo = pessoa_get_sexo(p);
        int eh_morador = pessoa_eh_morador(p);
        
        if (sexo == 'M') {
            homens++;
            if (eh_morador) moradores_homens++;
            else sem_teto_homens++;
        } else if (sexo == 'F') {
            mulheres++;
            if (eh_morador) moradores_mulheres++;
            else sem_teto_mulheres++;
        }
        
        if (eh_morador) {
            moradores++;
        } else {
            sem_teto++;
        }
    }
    
    fprintf(txt, "\n========== CENSO ==========\n");
    fprintf(txt, "Total habitantes: %d\n", total);
    fprintf(txt, "Total moradores: %d\n", moradores);
    fprintf(txt, "Total sem-teto: %d\n", sem_teto);
    fprintf(txt, "\n--- Por sexo (total) ---\n");
    fprintf(txt, "Homens: %d\n", homens);
    fprintf(txt, "Mulheres: %d\n", mulheres);
    fprintf(txt, "\n--- Moradores por sexo ---\n");
    fprintf(txt, "Moradores homens: %d\n", moradores_homens);
    fprintf(txt, "Moradores mulheres: %d\n", moradores_mulheres);
    fprintf(txt, "\n--- Sem-teto por sexo ---\n");
    fprintf(txt, "Sem-teto homens: %d\n", sem_teto_homens);
    fprintf(txt, "Sem-teto mulheres: %d\n", sem_teto_mulheres);
    fprintf(txt, "==========================\n");
    fflush(txt);
}

void qry_quadraCount(char *cep) {
    int counts[5];
    banco_getQuadraResidents(cep, counts);
    fprintf(txt, "\n Quadra %s:\n", cep);
    fprintf(txt, "  Norte: %d\n  Sul: %d\n  Leste: %d\n  Oeste: %d\n  Total: %d\n", 
            counts[1], counts[2], counts[3], counts[4], counts[0]);
}

void qry_nasce(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc) {
    banco_addPessoa(cpf, nome, sobrenome, sexo, nasc);
}

void qry_rip(char *cpf) {
    Pessoa *p = banco_getPessoa(cpf);
    
    if (p) {
        // Salvar dados ANTES de remover
        char cep[50];
        strcpy(cep, pessoa_get_cep(p));
        char face = pessoa_get_face(p);
        int num = pessoa_get_num(p);
        int eh_morador = pessoa_eh_morador(p);
        
        fprintf(txt, "Faleceu: %s %s\n", pessoa_get_cpf(p), pessoa_get_nome(p));
        
        if (eh_morador) {
            fprintf(txt, "Endereco: %s/%c/%d\n", cep, face, num);
            
            Quadra *q = banco_getQuadra(cep);
            if (q) {
                double x = getX(q, face, num);
                double y = getY(q, face, num);
                
                // ✝ CRUZ VERMELHA
                svg_line(x - 4, y, x + 4, y, "red");   // horizontal
                svg_line(x, y - 4, x, y + 4, "red");   // vertical
                
                free(q);
            }
        }
        
        free(p);
        banco_removePessoa(cpf);
    }
}

void qry_despejar(char *cpf) {
    Pessoa *p = banco_getPessoa(cpf);
    
    if (p && pessoa_eh_morador(p)) {
        // Salva dados ANTES de alterar
        char cep[50];
        strcpy(cep, pessoa_get_cep(p));
        char face = pessoa_get_face(p);
        int num = pessoa_get_num(p);
        
        fprintf(txt, "Despejado: %s %s\n", pessoa_get_cpf(p), pessoa_get_nome(p));
        fprintf(txt, "Endereco: %s/%c/%d\n", cep, face, num);
        
        Quadra *q = banco_getQuadra(cep);
        if (q) {
            double x = getX(q, face, num);
            double y = getY(q, face, num);
            
            // 🔴 MARCA DO DESPEJO - círculo preto
            svg_circle(x, y, 4, "black");
            free(q);
        }
        
        free(p);
        banco_despejar(cpf);
    }
}