#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qry.h"
#include "banco.h"
#include "svg.h"

/**
 * @file qry.c
 * @brief Processamento de consultas do arquivo .qry
 * 
 * Este módulo processa os comandos do arquivo de consultas:
 * - rq: remove quadra
 * - h?: informações de pessoa
 * - mud: muda pessoa de endereço
 * - censo: estatísticas da cidade
 * - pq: conta moradores por face
 * - nasce: nova pessoa
 * - rip: pessoa falece
 * - dspj: despeja pessoa
 */

static FILE *txt = NULL;  // Arquivo de saída de texto

/* ==================== FUNÇÕES AUXILIARES ==================== */

/**
 * Calcula a coordenada X para desenhar um endereço no SVG
 * @param q Quadra onde o endereço está
 * @param face Face da quadra ('N','S','L','O')
 * @param num Número do endereço (distância da âncora)
 * @return Coordenada X em pixels
 */
double getX(Quadra *q, char face, int num) {
    double x = quadra_get_x(q);
    double w = quadra_get_w(q);
    
    // Converte W (West) para O (Oeste) e E (East) para L (Leste)
    if (face == 'W') face = 'O';
    if (face == 'E') face = 'L';
    
    // Norte e Sul: X = x + número (distância do canto esquerdo)
    if (face == 'N' || face == 'S') return x + num;
    // Leste: X = x + largura (canto direito)
    if (face == 'L') return x + w;
    // Oeste: X = x (canto esquerdo)
    return x;
}

/**
 * Calcula a coordenada Y para desenhar um endereço no SVG
 * @param q Quadra onde o endereço está
 * @param face Face da quadra ('N','S','L','O')
 * @param num Número do endereço (distância da âncora)
 * @return Coordenada Y em pixels
 */
double getY(Quadra *q, char face, int num) {
    double y = quadra_get_y(q);
    double h = quadra_get_h(q);
    
    // Converte W (West) para O (Oeste) e E (East) para L (Leste)
    if (face == 'W') face = 'O';
    if (face == 'E') face = 'L';
    
    // Norte: Y = y (canto superior)
    if (face == 'N') return y;
    // Sul: Y = y + altura (canto inferior)
    if (face == 'S') return y + h;
    // Leste e Oeste: Y = y + número (distância do canto superior)
    return y + num;
}

/* ==================== INICIALIZAÇÃO/FINALIZAÇÃO ==================== */

/**
 * Inicializa o arquivo de saída de texto
 * @param f Nome do arquivo .txt a ser criado
 */
void qry_init(char *f) { 
    txt = fopen(f, "w"); 
    if (txt) {
        // Desativa buffer para escrita imediata (útil para debug)
        setvbuf(txt, NULL, _IONBF, 0);
    }
}

/**
 * Finaliza o arquivo de saída de texto
 */
void qry_close(void) { 
    if (txt) {
        fclose(txt);
        txt = NULL;
    }
}

/* ==================== COMANDOS DO ARQUIVO .QRY ==================== */

/**
 * rq - Remove uma quadra
 * - Desenha um X vermelho no local da quadra
 * - Lista os CPFs e nomes dos moradores no arquivo .txt
 * - Remove a quadra do banco (moradores se tornam sem-teto)
 */
void qry_removeQuadra(char *cep) {
    Quadra *q = banco_getQuadra(cep);
    if (q) {
        // Lista moradores no arquivo de texto
        int total = banco_getPessoaCount();
        for (int i = 0; i < total; i++) {
            Pessoa *p = banco_getPessoaIndex(i);
            if (p && pessoa_eh_morador(p)) {
                const char *p_cep = pessoa_get_cep(p);
                if (p_cep && strcmp(p_cep, cep) == 0) {
                    fprintf(txt, "%s %s\n", pessoa_get_cpf(p), pessoa_get_nome(p));
                }
            }
        }
        
        // Desenha X vermelho (duas linhas diagonais)
        svg_line(quadra_get_x(q), quadra_get_y(q), 
                 quadra_get_x(q) + quadra_get_w(q), 
                 quadra_get_y(q) + quadra_get_h(q), "red");
        svg_line(quadra_get_x(q) + quadra_get_w(q), quadra_get_y(q), 
                 quadra_get_x(q), quadra_get_y(q) + quadra_get_h(q), "red");
        
        free(q);
    }
    
    banco_removeQuadra(cep);
}

/**
 * h? - Exibe informações de uma pessoa
 * - Mostra todos os dados cadastrais
 * - Se for morador, mostra o endereço e desenha círculo azul no SVG
 * - Se for sem-teto, indica essa condição
 */
void qry_infoPessoa(char *cpf) {
    Pessoa *p = banco_getPessoa(cpf);
    if (p) {
        // Dados básicos
        fprintf(txt, "%s %s %s %c %s\n", 
                pessoa_get_cpf(p), pessoa_get_nome(p), 
                pessoa_get_sobrenome(p), pessoa_get_sexo(p), 
                pessoa_get_nasc(p));
        
        if (pessoa_eh_morador(p)) {
            // Endereço
            fprintf(txt, "Mora em %s/%c/%d %s\n", 
                    pessoa_get_cep(p), pessoa_get_face(p), 
                    pessoa_get_num(p), pessoa_get_compl(p));
            
            // Desenha círculo azul no local
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

/**
 * mud - Muda uma pessoa de endereço
 * - Atualiza o endereço no banco
 * - Desenha um quadrado vermelho com o CPF no novo local
 */
void qry_mudarPessoa(char *cpf, char *cep, char *face, int num, char *compl) {
    char face_char = face[0];
    
    // Converte W (West) para O (Oeste) e E (East) para L (Leste)
    if (face_char == 'W') face_char = 'O';
    if (face_char == 'E') face_char = 'L';
    
    banco_mudar(cpf, cep, face_char, num, compl);
    
    // Desenha marcação no novo endereço
    Quadra *q = banco_getQuadra(cep);
    if (q) {
        double x = getX(q, face_char, num);
        double y = getY(q, face_char, num);
        svg_rect(x - 5, y - 5, 10, 10, "red");  // Quadrado vermelho
        svg_text(x - 4, y + 2, cpf);             // CPF dentro do quadrado
        free(q);
    }
}

/**
 * censo - Estatísticas da cidade
 * - Total de habitantes
 * - Total de moradores
 * - Número de homens e mulheres
 * (versão simplificada para o projeto)
 */
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
        
        // Contagem total por sexo
        if (sexo == 'M') {
            homens++;
            if (eh_morador) moradores_homens++;
            else sem_teto_homens++;
        } else if (sexo == 'F') {
            mulheres++;
            if (eh_morador) moradores_mulheres++;
            else sem_teto_mulheres++;
        }
        
        // Contagem de moradores e sem-teto
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
/**
 * pq - Conta moradores de uma quadra por face
 * - Mostra no arquivo .txt: Norte, Sul, Leste, Oeste e Total
 */
void qry_quadraCount(char *cep) {
    int counts[5]; // 0:Total, 1:N, 2:S, 3:L, 4:O
    banco_getQuadraResidents(cep, counts);
    fprintf(txt, "\n Quadra %s:\n", cep);
    fprintf(txt, "  Norte: %d\n  Sul: %d\n  Leste: %d\n  Oeste: %d\n  Total: %d\n", 
            counts[1], counts[2], counts[3], counts[4], counts[0]);
}

/**
 * nasce - Nova pessoa nasce
 * - Adiciona uma nova pessoa ao banco
 * - Registra o nascimento no arquivo .txt
 */
void qry_nasce(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc) {
    banco_addPessoa(cpf, nome, sobrenome, sexo, nasc);
    fprintf(txt, "Nasceu: %s %s\n", cpf, nome);
}

/**
 * rip - Pessoa falece
 * - Remove a pessoa do banco
 * - Se era morador, mostra o endereço onde morava
 * - e dadiciona uma ✝ na sua casa
 */
void qry_rip(char *cpf) {
    Pessoa *p = banco_getPessoa(cpf);

    if (p) {

        // 📌 SALVAR DADOS ANTES
        char cep[50];
        strcpy(cep, pessoa_get_cep(p));
        char face = pessoa_get_face(p);
        int num = pessoa_get_num(p);
        int eh_morador = pessoa_eh_morador(p);

        fprintf(txt, "Faleceu: %s %s\n",
                pessoa_get_cpf(p), pessoa_get_nome(p));

        if (eh_morador) {
            fprintf(txt, "Endereco: %s/%c/%d\n", cep, face, num);

            Quadra *q = banco_getQuadra(cep);
            if (q) {
                double x = getX(q, face, num);
                double y = getY(q, face, num);

                // ✝ CRUZ BRANCA
                // cruz branca desenhada
                    svg_line(x-4, y, x+4, y, "white");   // horizontal
                    svg_line(x, y-4, x, y+4, "white");   // vertical

                //svg_text(x - 3, y - 3, "✝");
            }
        }

        banco_removePessoa(cpf);
    }
}


/**
 * dspj - Despeja uma pessoa
 * - Torna a pessoa sem-teto
 * - Registra o despejo no arquivo .txt
 * - Gera um .vsg com um X marcando o locau do despejo
 */
void qry_despejar(char *cpf) {
    Pessoa *p = banco_getPessoa(cpf);

            if (p && pessoa_eh_morador(p)) {
        
                // Salva dados ANTES de alterar
                char cep[50];
                strcpy(cep, pessoa_get_cep(p));
                char face = pessoa_get_face(p);
                int num = pessoa_get_num(p);
        
                fprintf(txt, "Despejado: %s %s\n",
                        pessoa_get_cpf(p), pessoa_get_nome(p));
        
                fprintf(txt, "Endereco: %s/%c/%d\n", cep, face, num);
        
                Quadra *q = banco_getQuadra(cep);
                if (q) {
                            double x = getX(q, face, num);
                            double y = getY(q, face, num);
                
                            // X em cima
                            svg_line(x-4, y-4, x+4, y+4, "red");
                            svg_line(x+4, y-4, x-4, y+4, "red");
                }
        
                banco_despejar(cpf);
    }
}
