#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_reader.h"
#include "banco.h"
#include "hashfile.h"
#include "svg.h"
#include "qry.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

void parse_arguments(int argc, char *argv[], char *geo_file, char *pm_file, char *qry_file, char *out_dir, int *skip_geo) {
    geo_file[0] = '\0';
    pm_file[0] = '\0';
    qry_file[0] = '\0';
    strcpy(out_dir, ".");
    *skip_geo = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            strcpy(geo_file, argv[++i]);
        }
        else if (strcmp(argv[i], "-pm") == 0 && i + 1 < argc) {
            strcpy(pm_file, argv[++i]);
        }
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            strcpy(qry_file, argv[++i]);
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            strcpy(out_dir, argv[++i]);
        }
        else if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            i++;
        }
        else if (strcmp(argv[i], "-skip-geo") == 0) {
            *skip_geo = 1;
        }
    }
}

void get_base_name(const char *filepath, char *basename, int size) {
    char temp[PATH_MAX];
    snprintf(temp, sizeof(temp), "%s", filepath);
    
    char *last_slash = strrchr(temp, '/');
    char *name = (last_slash) ? last_slash + 1 : temp;
    
    char *dot = strrchr(name, '.');
    if (dot) *dot = '\0';
    
    snprintf(basename, size, "%s", name);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: ./ted -f arquivo.geo -o saida [-pm pessoas.pm] [-q consultas.qry] [-skip-geo]\n");
        return 1;
    }

    char geo_file[PATH_MAX];
    char pm_file[PATH_MAX];
    char qry_file[PATH_MAX];
    char out_dir[PATH_MAX];
    int skip_geo = 0;
    
    parse_arguments(argc, argv, geo_file, pm_file, qry_file, out_dir, &skip_geo);
    
    if (strlen(geo_file) == 0 && !skip_geo) {
        printf("Erro: Arquivo .geo não fornecido (use -f)\n");
        return 1;
    }
    
    // Criar diretório de saída
    char mkdir_cmd[PATH_MAX + 20];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", out_dir);
    system(mkdir_cmd);
    
    // Determinar nome base para os arquivos de saída
    char base_name[PATH_MAX];
    char geo_base[PATH_MAX];
    
    if (strlen(geo_file) > 0) {
        get_base_name(geo_file, geo_base, sizeof(geo_base));
        strcpy(base_name, geo_base);
    } else {
        strcpy(base_name, "estado");
    }
    
    if (strlen(qry_file) > 0) {
        char qry_base[PATH_MAX];
        get_base_name(qry_file, qry_base, sizeof(qry_base));
        strncat(base_name, "-", sizeof(base_name) - strlen(base_name) - 1);
        strncat(base_name, qry_base, sizeof(base_name) - strlen(base_name) - 1);
    }
    
    char svg_path[PATH_MAX];
    char txt_path[PATH_MAX];
    
    snprintf(svg_path, sizeof(svg_path), "%s/%s.svg", out_dir, base_name);
    snprintf(txt_path, sizeof(txt_path), "%s/%s.txt", out_dir, base_name);
    
    printf("========================================\n");
    printf("TED - Tratamento de Estatísticas de Dados\n");
    printf("========================================\n");
    printf("Arquivo geo: %s\n", geo_file);
    printf("Arquivo pm: %s\n", strlen(pm_file) > 0 ? pm_file : "(nenhum)");
    printf("Arquivo qry: %s\n", strlen(qry_file) > 0 ? qry_file : "(nenhum)");
    printf("Saida SVG: %s\n", svg_path);
    printf("Saida TXT: %s\n", txt_path);
    printf("Skip geo: %s\n", skip_geo ? "SIM" : "NAO");
    printf("========================================\n");
    
    // Configurar diretórios para os módulos
    hf_set_output_dir(out_dir);
    banco_set_output_dir(out_dir);
    
    // INICIALIZAR SVG PRIMEIRO (antes de processar consultas)
    printf("Inicializando SVG...\n");
    svg_init(svg_path);
    
    // Inicializar banco de dados
    printf("Inicializando banco de dados...\n");
    banco_init();
    
    // Inicializar arquivo de texto
    printf("Inicializando arquivo de texto...\n");
    qry_init(txt_path);
    
    // Ler arquivo .geo
    if (!skip_geo && strlen(geo_file) > 0) {
        printf("Lendo arquivo geo...\n");
        readGeo(geo_file);
    } else if (skip_geo) {
        printf("Pulando leitura do arquivo geo (usando estado salvo)...\n");
    }
    
    // Ler arquivo .pm
    if (strlen(pm_file) > 0) {
        printf("Lendo arquivo pm...\n");
        readPm(pm_file);
    }
    
    // Processar consultas .qry (aqui serão desenhados círculos, cruzes, etc.)
    if (strlen(qry_file) > 0) {
        printf("Lendo arquivo qry...\n");
        readQry(qry_file);
    }
    
    // Desenhar todas as quadras (após as consultas)
    printf("Desenhando quadras...\n");
    banco_desenhar_todas_quadras();
    
    // Finalizar SVG
    printf("Finalizando SVG...\n");
    svg_close();
    
    // Finalizar outros módulos
    qry_close();
    banco_close();
    
    printf("========================================\n");
    printf("Processamento concluído com sucesso!\n");
    printf("Arquivos gerados em: %s\n", out_dir);
    printf("========================================\n");
    
    return 0;
}