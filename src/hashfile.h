#ifndef HASHFILE_H
#define HASHFILE_H

typedef void* Hash;

void hf_set_output_dir(const char *dir);
void hf_set_base_name(const char *name);
Hash hf_create(char *filename, int size, int dataSize);
int hf_insert(Hash h, char *key, void *data);
void* hf_search(Hash h, char *key);
int hf_remove(Hash h, char *key);
void hf_dump(Hash h, const char *filename);
void hf_close(Hash h);

#endif
