#include <stdio.h>
#include <stdlib.h>
#include "geo.h"
#include "banco.h"
#include "svg.h"

/**
 * @file geo.c
 * @brief Implementação do processamento do arquivo .geo
 */

/**
 * Insere uma nova quadra no sistema
 * 
 * @param cep CEP da quadra
 * @param x Coordenada X
 * @param y Coordenada Y
 * @param w Largura
 * @param h Altura
 */
void geo_insertQuadra(char *cep, double x, double y, double w, double h){       
    // 1. Adiciona a quadra ao banco de dados
    banco_addQuadra(cep, x, y, w, h);
       
    // 2. Busca a quadra recém-inserida para obter as cores
    Quadra *q = banco_getQuadra(cep);
    if (q) {
        // 3. Desenha o retângulo no SVG com as cores definidas
        svg_rect(x, y, w, h, (char*)quadra_get_fill(q));        
        // 4. Escreve o CEP dentro da quadra (deslocado 5px para direita e 15px para baixo)
        svg_text(x + 5, y + 15, cep);
        // 5. Libera a memória da quadra (cópia retornada pelo banco)
        free(q);
    }
}

/**
 * Define o estilo padrão para as quadras
 * 
 * @param fill Cor de preenchimento
 * @param stroke Cor da borda
 * @param sw Espessura da borda
 */

void geo_setQuadraStyle(char *fill, char *stroke, char *sw){
        // Repassa a configuração para o banco de dados
    banco_setQuadraStyle(fill, stroke, sw);
}