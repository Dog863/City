#ifndef GEO_H
#define GEO_H

/**
 * @file geo.h
 * @brief Processamento do arquivo .geo (descrição da cidade)
 * 
 * Este módulo processa os comandos do arquivo de entrada .geo
 * e desenha as quadras no SVG
 */

/**
 * Insere uma nova quadra no sistema
 * - Adiciona ao banco de dados
 * - Desenha o retângulo no SVG
 * - Escreve o CEP dentro da quadra
 * 
 * @param cep CEP da quadra (identificador único)
 * @param x Coordenada X do canto superior esquerdo
 * @param y Coordenada Y do canto superior esquerdo
 * @param w Largura da quadra
 * @param h Altura da quadra
 */
void geo_insertQuadra(char *cep, double x, double y, double w, double h);

/**
 * Define o estilo padrão para as quadras
 * Este estilo será aplicado a todas as quadras inseridas após este comando
 * 
 * @param fill Cor de preenchimento (ex: "lightblue", "yellow")
 * @param stroke Cor da borda (ex: "black", "blue")
 * @param sw Espessura da borda em pixels
 */
void geo_setQuadraStyle(char *fill, char *stroke, char*sw);

#endif
