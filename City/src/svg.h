#ifndef SVG_H
#define SVG_H

/**
 * @file svg.h
 * @brief Geração de arquivos SVG (Scalable Vector Graphics)
 * 
 * Permite desenhar formas geométricas e texto em arquivos SVG
 * O formato SVG é um padrão para gráficos vetoriais na web
 */

/**
 * Inicializa um novo arquivo SVG
 * Abre o arquivo e escreve a tag de abertura <svg>
 * 
 * @param file Nome do arquivo .svg a ser criado
 */
void svg_init(char* file);

/**
 * Desenha um retângulo
 * 
 * @param x Coordenada X do canto superior esquerdo
 * @param y Coordenada Y do canto superior esquerdo
 * @param w Largura do retângulo
 * @param h Altura do retângulo
 * @param fill Cor de preenchimento (ex: "red", "blue", "#FF0000")
 */
void svg_rect(double x, double y, double w, double h, char* fill);

/**
 * Desenha texto na posição especificada
 * 
 * @param x Coordenada X do texto
 * @param y Coordenada Y do texto (linha base)
 * @param txt Texto a ser desenhado
 */
void svg_text(double x, double y, char* txt);

/**
 * Desenha uma linha entre dois pontos
 * 
 * @param x1 Coordenada X do ponto inicial
 * @param y1 Coordenada Y do ponto inicial
 * @param x2 Coordenada X do ponto final
 * @param y2 Coordenada Y do ponto final
 * @param cor Cor da linha (ex: "black", "red")
 */
void svg_line(double x1, double y1, double x2, double y2, char* cor);

/**
 * Desenha um círculo
 * 
 * @param x Coordenada X do centro do círculo
 * @param y Coordenada Y do centro do círculo
 * @param r Raio do círculo
 * @param cor Cor de preenchimento
 */
void svg_circle(double x, double y, double r, char* cor);

/**
 * Finaliza o arquivo SVG
 * Escreve a tag de fechamento </svg> e fecha o arquivo
 */
void svg_close();

#endif
