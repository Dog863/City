#include <stdio.h>
#include "svg.h"

/**
 * @file svg.c
 * @brief Implementação da geração de arquivos SVG
 */

static FILE *f = NULL;  // Ponteiro para o arquivo SVG (NULL se não aberto)

/**
 * Inicializa um novo arquivo SVG
 * Abre o arquivo e escreve a tag de abertura <svg>
 * 
 * @param file Nome do arquivo .svg a ser criado
 */
void svg_init(char *file) {
    // Se já existe um arquivo aberto, não abrir outro
    if (f != NULL) {
        return;
    }
    
    f = fopen(file, "w");
    if (f) {
        // Escreve o cabeçalho do SVG
        fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n");
    }
}

/**
 * Desenha um retângulo
 * 
 * @param x Coordenada X do canto superior esquerdo
 * @param y Coordenada Y do canto superior esquerdo
 * @param w Largura do retângulo
 * @param h Altura do retângulo
 * @param fill Cor de preenchimento
 */
void svg_rect(double x, double y, double w, double h, char *fill) {
    if (f) {
        fprintf(f, "<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" "
                   "style=\"fill:%s;stroke:black;stroke-width:1\"/>\n", 
                   x, y, w, h, fill);
    }
}

/**
 * Desenha texto na posição especificada
 * 
 * @param x Coordenada X do texto
 * @param y Coordenada Y do texto
 * @param txt Texto a ser desenhado
 */
void svg_text(double x, double y, char *txt) {
    if (f) {
        fprintf(f, "<text x=\"%lf\" y=\"%lf\" font-size=\"12\" font-family=\"Arial\">%s</text>\n", 
                x, y, txt);
    }
}

/**
 * Desenha uma linha entre dois pontos
 * 
 * @param x1 Coordenada X do ponto inicial
 * @param y1 Coordenada Y do ponto inicial
 * @param x2 Coordenada X do ponto final
 * @param y2 Coordenada Y do ponto final
 * @param cor Cor da linha
 */
void svg_line(double x1, double y1, double x2, double y2, char *cor) {
    if (f) {
        fprintf(f, "<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" "
                   "stroke=\"%s\" stroke-width=\"2\"/>\n", 
                   x1, y1, x2, y2, cor);
    }
}

/**
 * Desenha um círculo
 * 
 * @param x Coordenada X do centro
 * @param y Coordenada Y do centro
 * @param r Raio do círculo
 * @param cor Cor de preenchimento
 */
void svg_circle(double x, double y, double r, char *cor) {
    if (f) {
        fprintf(f, "<circle cx=\"%lf\" cy=\"%lf\" r=\"%lf\" fill=\"%s\" "
                   "stroke=\"black\" stroke-width=\"1\"/>\n", 
                   x, y, r, cor);
    }
}

/**
 * Finaliza o arquivo SVG
 * Escreve a tag de fechamento </svg> e fecha o arquivo
 */
void svg_close(void) {
    if (f) {
        fprintf(f, "</svg>\n");
        fclose(f);
        f = NULL;  // Marca como fechado para evitar double free
    }
}
