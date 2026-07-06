#ifndef QRY_H
#define QRY_H

void qry_init(char *f);
void qry_close(void);

void qry_removeQuadra(char *cep);
void qry_infoPessoa(char *cpf);
void qry_mudarPessoa(char *cpf, char *cep, char *face, int num, char *compl);
void qry_censo(void);
void qry_quadraCount(char *cep);
void qry_nasce(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc);
void qry_rip(char *cpf);
void qry_despejar(char *cpf);

#endif