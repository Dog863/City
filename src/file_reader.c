#include <stdio.h>
#include <string.h>
#include "file_reader.h"
#include "geo.h"
#include "qry.h"
#include "pessoa.h"

#define MAX 256

void readGeo(char *file){
    FILE *f = fopen(file, "r");
    if (!f) {
        printf("Erro: Não foi possível abrir o arquivo %s\n", file);
        return;
    }
    
    char line[MAX];
    while(fgets(line, MAX, f)){
        char cmd[10];
        sscanf(line, "%s", cmd);

        if(strcmp(cmd, "q") == 0){
            char cep[20];
             double x, y, w, h;
            if(sscanf(line, "q %s %lf %lf %lf %lf", cep, &x, &y, &w, &h) == 5)
            geo_insertQuadra(cep, x, y, w, h);
        }

        else if(strcmp(cmd, "cq") == 0){
            char csw[20]; char cfill[20], cstrk[20];
            if(sscanf(line, "cq %s %s %s", csw, cfill, cstrk) == 3)
                geo_setQuadraStyle(cfill, cstrk, csw);
        }
    }
    fclose(f);
}

void readPm(char *file){
    FILE *f = fopen(file, "r");
    if (!f) {
        printf("Erro: Não foi possível abrir o arquivo %s\n", file);
        return;
    }
    
    char line[MAX];
    while(fgets(line, MAX, f)){
        char cmd[10];
        sscanf(line, "%s", cmd);

        if(strcmp(cmd, "p") == 0){
            char cpf[20], nome[50], sob[50], nasc[20];
            char sexo;
            sscanf(line, "p %s %s %s %c %s", cpf, nome, sob, &sexo, nasc);
            pessoa_insert(cpf, nome, sob, sexo, nasc);
        }
        else if(strcmp(cmd, "m") == 0){
            char cpf[20], cep[20], face[5], compl[50];
            int num;
            sscanf(line, "m %s %s %s %d %s", cpf, cep, face, &num, compl);
            pessoa_morar(cpf, cep, face, num, compl);
        }
    }
    fclose(f);
}

void readQry(char *file){
    FILE *f = fopen(file, "r");
    if (!f) {
        printf("Erro: Não foi possível abrir o arquivo %s\n", file);
        return;
    }
    
    char line[MAX];
    while(fgets(line, MAX, f)){
        char cmd[10];
        sscanf(line, "%s", cmd);

        if(strcmp(cmd, "rq") == 0){
            char cep[20];
            sscanf(line, "rq %s", cep);
            qry_removeQuadra(cep);
        }
        else if(strcmp(cmd, "h?") == 0){
            char cpf[20];
            sscanf(line, "h? %s", cpf);
            qry_infoPessoa(cpf);
        }
        else if(strcmp(cmd, "mud") == 0){
            char cpf[20], cep[20], face[5], compl[50];
            int num;
            sscanf(line, "mud %s %s %s %d %s", cpf, cep, face, &num, compl);
            qry_mudarPessoa(cpf, cep, face, num, compl);
        }
        else if(strcmp(cmd, "censo") == 0){
            qry_censo();
        }
        else if(strcmp(cmd, "pq") == 0){
            char cep[20];
            sscanf(line, "pq %s", cep);
            qry_quadraCount(cep);
        }
        else if(strcmp(cmd, "nasc") == 0){
            char cpf[20], nome[50], sob[50], nasc[20], sexo;
            sscanf(line, "nasc %s %s %s %c %s", cpf, nome, sob, &sexo, nasc);
            qry_nasce(cpf, nome, sob, sexo, nasc);
        }
        else if(strcmp(cmd, "rip") == 0){
            char cpf[20];
            sscanf(line, "rip %s", cpf);
            qry_rip(cpf);
        }
        else if(strcmp(cmd, "dspj") == 0){
            char cpf[20];
            sscanf(line, "dspj %s", cpf);
            qry_despejar(cpf);
        }
    }
    fclose(f);
}