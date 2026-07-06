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

/**
 * @brief Estrutura para armazenar os parâmetros do programa
 */
typedef struct {
    char entrada_dir[PATH_MAX];
    char geo_file[PATH_MAX];
    char pm_file[PATH_MAX];
    char qry_file[PATH_MAX];
    char saida_dir[PATH_MAX];
    int tem_geo;
    int tem_pm;
    int tem_qry;
} Parametros;

/**
 * @brief Obtém o nome base de um arquivo (sem extensão e sem caminho)
 */
void get_base_name(const char *filepath, char *basename, int size) {
    const char *name = strrchr(filepath, '/');
    if (!name) {
        name = strrchr(filepath, '\\');
    }
    if (!name) {
        name = filepath;
    } else {
        name++;
    }
    
    int i = 0;
    while (name[i] && name[i] != '.' && i < size - 1) {
        basename[i] = name[i];
        i++;
    }
    basename[i] = '\0';
}

/**
 * @brief Concatena caminhos de forma segura
 */
void concat_path(const char *dir, const char *file, char *output, int size) {
    if (!dir || !file || !output || size <= 0) {
        if (output && size > 0) output[0] = '\0';
        return;
    }
    
    int dir_len = strlen(dir);
    int file_len = strlen(file);
    
    if (dir_len + file_len + 2 > size) {
        snprintf(output, size, "%s", file);
        return;
    }
    
    const char *dir_clean = dir;
    char dir_temp[PATH_MAX];
    if (dir_len > 0 && (dir[dir_len - 1] == '/' || dir[dir_len - 1] == '\\')) {
        strncpy(dir_temp, dir, dir_len - 1);
        dir_temp[dir_len - 1] = '\0';
        dir_clean = dir_temp;
    }
    
    snprintf(output, size, "%s/%s", dir_clean, file);
}

/**
 * @brief Cria um diretório recursivamente
 */
void mkdir_recursive(const char *path) {
    if (!path || strlen(path) == 0) return;
    
    char cmd[PATH_MAX + 20];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", path);
    system(cmd);
}

/**
 * @brief Valida se um arquivo existe
 */
int file_exists(const char *path) {
    if (!path) return 0;
    FILE *f = fopen(path, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

/**
 * @brief Exibe ajuda sobre os parâmetros do programa
 */
void print_help(void) {
    printf("========================================\n");
    printf("TED - Tratamento de Estatísticas de Dados\n");
    printf("========================================\n");
    printf("\n");
    printf("USO: ./ted -f arquivo.geo -o diretorio [-pm pessoas.pm] [-q consultas.qry] [-e entrada]\n");
    printf("\n");
    printf("PARÂMETROS:\n");
    printf("  -f arquivo.geo   Arquivo com descrição das quadras (OBRIGATÓRIO)\n");
    printf("  -o diretorio     Diretório de saída (OBRIGATÓRIO)\n");
    printf("  -pm arquivo.pm   Arquivo com pessoas e moradores (OPCIONAL)\n");
    printf("  -q arquivo.qry   Arquivo com consultas (OPCIONAL)\n");
    printf("  -e diretorio     Diretório base de entrada (OPCIONAL)\n");
    printf("  -h               Exibe esta mensagem de ajuda\n");
    printf("\n");
    printf("EXEMPLOS:\n");
    printf("  ./ted -f dados/c1.geo -o saida\n");
    printf("  ./ted -f dados/c1.geo -pm dados/c1.pm -o saida\n");
    printf("  ./ted -f dados/c1.geo -pm dados/c1.pm -q dados/consultas.qry -o saida\n");
    printf("========================================\n");
}

/**
 * @brief Processa os argumentos da linha de comando
 */
int parse_arguments(int argc, char *argv[], Parametros *p) {
    memset(p, 0, sizeof(Parametros));
    strncpy(p->entrada_dir, ".", PATH_MAX - 1);
    strncpy(p->saida_dir, ".", PATH_MAX - 1);
    p->entrada_dir[PATH_MAX - 1] = '\0';
    p->saida_dir[PATH_MAX - 1] = '\0';
    p->tem_geo = 0;
    p->tem_pm = 0;
    p->tem_qry = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            strncpy(p->geo_file, argv[++i], PATH_MAX - 1);
            p->geo_file[PATH_MAX - 1] = '\0';
            p->tem_geo = 1;
        }
        else if (strcmp(argv[i], "-pm") == 0 && i + 1 < argc) {
            strncpy(p->pm_file, argv[++i], PATH_MAX - 1);
            p->pm_file[PATH_MAX - 1] = '\0';
            p->tem_pm = 1;
        }
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            strncpy(p->qry_file, argv[++i], PATH_MAX - 1);
            p->qry_file[PATH_MAX - 1] = '\0';
            p->tem_qry = 1;
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            strncpy(p->saida_dir, argv[++i], PATH_MAX - 1);
            p->saida_dir[PATH_MAX - 1] = '\0';
        }
        else if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            strncpy(p->entrada_dir, argv[++i], PATH_MAX - 1);
            p->entrada_dir[PATH_MAX - 1] = '\0';
        }
        else {
            printf("Erro: Parâmetro desconhecido '%s'\n", argv[i]);
            print_help();
            return 0;
        }
    }
    
    if (!p->tem_geo) {
        printf("Erro: Arquivo .geo é obrigatório (use -f)\n");
        print_help();
        return 0;
    }
    
    return 1;
}

/**
 * @brief Função principal do programa
 */
int main(int argc, char *argv[]) {
    Parametros params;
    char geo_path[PATH_MAX];
    char pm_path[PATH_MAX];
    char qry_path[PATH_MAX];
    char svg_path[PATH_MAX];
    char txt_path[PATH_MAX];
    char base_name[PATH_MAX];
    int erro = 0;
    
    if (!parse_arguments(argc, argv, &params)) {
        return 1;
    }
    
    // ============================================================
    // 1. PREPARAR DIRETÓRIOS
    // ============================================================
    
    mkdir_recursive(params.saida_dir);
    
    if (strlen(params.entrada_dir) > 0 && strlen(params.geo_file) > 0) {
        concat_path(params.entrada_dir, params.geo_file, geo_path, PATH_MAX);
    } else {
        snprintf(geo_path, PATH_MAX, "%s", params.geo_file);
    }
    
    if (params.tem_pm && strlen(params.pm_file) > 0) {
        if (strlen(params.entrada_dir) > 0) {
            concat_path(params.entrada_dir, params.pm_file, pm_path, PATH_MAX);
        } else {
            snprintf(pm_path, PATH_MAX, "%s", params.pm_file);
        }
    }
    
    if (params.tem_qry && strlen(params.qry_file) > 0) {
        if (strlen(params.entrada_dir) > 0) {
            concat_path(params.entrada_dir, params.qry_file, qry_path, PATH_MAX);
        } else {
            snprintf(qry_path, PATH_MAX, "%s", params.qry_file);
        }
    }
    
    get_base_name(params.geo_file, base_name, PATH_MAX);
    snprintf(svg_path, PATH_MAX, "%s/%s.svg", params.saida_dir, base_name);
    snprintf(txt_path, PATH_MAX, "%s/%s.txt", params.saida_dir, base_name);
    
    // ============================================================
    // 2. INFORMAR PARÂMETROS
    // ============================================================
    
    printf("========================================\n");
    printf("TED - Tratamento de Estatísticas de Dados\n");
    printf("========================================\n");
    printf("Arquivo .geo: %s\n", geo_path);
    printf("Arquivo .pm:  %s\n", params.tem_pm ? pm_path : "(nenhum)");
    printf("Arquivo .qry: %s\n", params.tem_qry ? qry_path : "(nenhum)");
    printf("Saida SVG:    %s\n", svg_path);
    printf("Saida TXT:    %s\n", params.tem_qry ? txt_path : "(nenhum)");
    printf("========================================\n");
    
    // ============================================================
    // 3. INICIALIZAR SISTEMA
    // ============================================================
    
    hf_set_output_dir(params.saida_dir);
    banco_set_output_dir(params.saida_dir);
    
    printf("Inicializando SVG...\n");
    svg_init(svg_path);
    
    printf("Inicializando banco de dados...\n");
    banco_init();
    
    if (params.tem_qry) {
        printf("Inicializando arquivo de consultas...\n");
        qry_init(txt_path);
    }
    
    // ============================================================
    // 4. LER ARQUIVOS DE ENTRADA
    // ============================================================
    
    if (file_exists(geo_path)) {
        printf("Lendo arquivo .geo...\n");
        readGeo(geo_path);
    } else {
        printf("Erro: Arquivo .geo não encontrado: %s\n", geo_path);
        erro = 1;
        goto cleanup;
    }
    
    if (params.tem_pm && file_exists(pm_path)) {
        printf("Lendo arquivo .pm...\n");
        readPm(pm_path);
    } else if (params.tem_pm) {
        printf("Aviso: Arquivo .pm não encontrado: %s\n", pm_path);
    }
    
    if (params.tem_qry && file_exists(qry_path)) {
        printf("Lendo arquivo .qry...\n");
        readQry(qry_path);
    } else if (params.tem_qry) {
        printf("Aviso: Arquivo .qry não encontrado: %s\n", qry_path);
    }
    
    // ============================================================
    // 5. DESENHAR QUADRAS NO SVG
    // ============================================================
    
    printf("Desenhando quadras no SVG...\n");
    banco_desenhar_todas_quadras();
    
    // ============================================================
    // 6. FINALIZAR
    // ============================================================
    
cleanup:
    printf("Finalizando SVG...\n");
    svg_close();
    
    if (params.tem_qry) {
        printf("Finalizando arquivo de consultas...\n");
        qry_close();
    }
    
    printf("Finalizando banco de dados...\n");
    banco_close();
    
    if (erro) {
        printf("========================================\n");
        printf("❌ Processamento falhou!\n");
        printf("========================================\n");
        return 1;
    }
    
    printf("========================================\n");
    printf("✅ Processamento concluído com sucesso!\n");
    printf("📁 Arquivos gerados em: %s\n", params.saida_dir);
    printf("   - %s\n", svg_path);
    if (params.tem_qry) {
        printf("   - %s\n", txt_path);
    }
    printf("========================================\n");
    
    return 0;
}