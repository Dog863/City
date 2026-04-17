#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include "file_reader.h"
#include "banco.h"
#include "svg.h"
#include "qry.h"

#define PATH_MAX 1024  // Aumentado para 1024

void parse_arguments(int argc, char *argv[], char *geo_file, char *pm_file, char *qry_file, char *out_dir) {
    // Valores padrão
    geo_file[0] = '\0';
    pm_file[0] = '\0';
    qry_file[0] = '\0';
    strcpy(out_dir, ".");
    
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
    }
}

// Função para extrair o nome base do arquivo (sem extensão e sem caminho)
void get_base_name(const char *filepath, char *basename, int size) {
    // Fazer uma cópia pois strrchr não modifica
    char temp[PATH_MAX];
    snprintf(temp, sizeof(temp), "%s", filepath);
    
    // Encontrar o último '/'
    char *last_slash = strrchr(temp, '/');
    char *name;
    
    if (last_slash) {
        name = last_slash + 1;
    } else {
        name = temp;
    }
    
    // Remover extensão
    char *dot = strrchr(name, '.');
    if (dot) {
        *dot = '\0';
    }
    
    snprintf(basename, size, "%s", name);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: ./ted -f arquivo.geo -o saida [-pm pessoas.pm] [-q consultas.qry]\n");
        printf("Exemplo: ./ted -f c1/c1.geo -o saida -pm c1/c1.pm -q c1/mudanca-todos-moradores.qry\n");
        return 1;
    }

    char geo_file[PATH_MAX];
    char pm_file[PATH_MAX];
    char qry_file[PATH_MAX];
    char out_dir[PATH_MAX];
    
    parse_arguments(argc, argv, geo_file, pm_file, qry_file, out_dir);
    
    // Verificar se o arquivo geo foi fornecido
    if (strlen(geo_file) == 0) {
        printf("Erro: Arquivo .geo não fornecido (use -f)\n");
        return 1;
    }
    
    // Criar diretório de saída se não existir
    char mkdir_cmd[PATH_MAX + 20];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", out_dir);
    system(mkdir_cmd);
    
    // Determinar nome base para os arquivos de saída
    char base_name[PATH_MAX];
    char geo_base[PATH_MAX];
    get_base_name(geo_file, geo_base, sizeof(geo_base));
    strcpy(base_name, geo_base);
    
    // Se houver arquivo .qry, adicionar o nome dele ao base_name
    if (strlen(qry_file) > 0) {
        char qry_base[PATH_MAX];
        get_base_name(qry_file, qry_base, sizeof(qry_base));
        // Usar strncat para evitar overflow
        strncat(base_name, "-", sizeof(base_name) - strlen(base_name) - 1);
        strncat(base_name, qry_base, sizeof(base_name) - strlen(base_name) - 1);
    }
    
    char svg_path[PATH_MAX];
    char txt_path[PATH_MAX];
    
    // Usar snprintf com verificação
    int svg_len = snprintf(svg_path, sizeof(svg_path), "%s/%s.svg", out_dir, base_name);
    int txt_len = snprintf(txt_path, sizeof(txt_path), "%s/%s.txt", out_dir, base_name);
    
    if (svg_len >= (int)sizeof(svg_path) || txt_len >= (int)sizeof(txt_path)) {
        printf("Erro: Caminho muito longo!\n");
        return 1;
    }
    
    printf("========================================\n");
    printf("Arquivo geo: %s\n", geo_file);
    printf("Arquivo pm: %s\n", strlen(pm_file) > 0 ? pm_file : "(nenhum)");
    printf("Arquivo qry: %s\n", strlen(qry_file) > 0 ? qry_file : "(nenhum)");
    printf("Saida SVG: %s\n", svg_path);
    printf("Saida TXT: %s\n", txt_path);
    printf("========================================\n");
    
    // Inicializar módulos
    banco_init();
    svg_init(svg_path);
    qry_init(txt_path);
    
    // Ler arquivos
    printf("Lendo arquivo geo...\n");
    readGeo(geo_file);
    
    if (strlen(pm_file) > 0) {
        printf("Lendo arquivo pm...\n");
        readPm(pm_file);
    }
    
    if (strlen(qry_file) > 0) {
        printf("Lendo arquivo qry...\n");
        readQry(qry_file);
    }
    
    // Fechar módulos
    svg_close();
    qry_close();
    banco_close();
    
    printf("Processamento concluído!\n");
    printf("Arquivos gerados em: %s\n", out_dir);
    
    return 0;
}