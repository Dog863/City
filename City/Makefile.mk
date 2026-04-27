PROJ_NAME = ted
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration -Wall -Wextra
LDFLAGS = -O0

# Diretórios
SRC_DIR = src
TEST_DIR = src/test
UNITY_DIR = unity

# Arquivos fonte principais
MAIN_SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c \
            $(SRC_DIR)/file_reader.c $(SRC_DIR)/geo.c $(SRC_DIR)/pessoa.c \
            $(SRC_DIR)/qry.c $(SRC_DIR)/svg.c
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

# Testes
TEST_HASHFILE_SRCS = $(TEST_DIR)/test_hashfile.c $(SRC_DIR)/hashfile.c $(UNITY_DIR)/unity.c
TEST_HASHFILE_OBJS = $(TEST_HASHFILE_SRCS:.c=.o)
TEST_HASHFILE_BIN = test_hashfile

TEST_BANCO_SRCS = $(TEST_DIR)/test_banco.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c $(UNITY_DIR)/unity.c
TEST_BANCO_OBJS = $(TEST_BANCO_SRCS:.c=.o)
TEST_BANCO_BIN = test_banco

TEST_GEO_SRCS = $(TEST_DIR)/test_geo.c $(SRC_DIR)/geo.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c $(SRC_DIR)/svg.c $(UNITY_DIR)/unity.c
TEST_GEO_OBJS = $(TEST_GEO_SRCS:.c=.o)
TEST_GEO_BIN = test_geo

TEST_PESSOA_SRCS = $(TEST_DIR)/test_pessoa.c $(SRC_DIR)/pessoa.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c $(UNITY_DIR)/unity.c
TEST_PESSOA_OBJS = $(TEST_PESSOA_SRCS:.c=.o)
TEST_PESSOA_BIN = test_pessoa

TEST_SVG_SRCS = $(TEST_DIR)/test_svg.c $(SRC_DIR)/svg.c $(UNITY_DIR)/unity.c
TEST_SVG_OBJS = $(TEST_SVG_SRCS:.c=.o)
TEST_SVG_BIN = test_svg

TEST_QRY_SRCS = $(TEST_DIR)/test_qry.c $(SRC_DIR)/qry.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c $(SRC_DIR)/svg.c $(UNITY_DIR)/unity.c
TEST_QRY_OBJS = $(TEST_QRY_SRCS:.c=.o)
TEST_QRY_BIN = test_qry

# Regras principais
all: $(PROJ_NAME)

$(PROJ_NAME): $(MAIN_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Regras para os testes
test_hashfile: $(TEST_HASHFILE_OBJS)
	$(CC) -o $(TEST_HASHFILE_BIN) $^ $(LDFLAGS)
	./$(TEST_HASHFILE_BIN)

test_banco: $(TEST_BANCO_OBJS)
	$(CC) -o $(TEST_BANCO_BIN) $^ $(LDFLAGS)
	./$(TEST_BANCO_BIN)

test_geo: $(TEST_GEO_OBJS)
	$(CC) -o $(TEST_GEO_BIN) $^ $(LDFLAGS)
	./$(TEST_GEO_BIN)

test_pessoa: $(TEST_PESSOA_OBJS)
	$(CC) -o $(TEST_PESSOA_BIN) $^ $(LDFLAGS)
	./$(TEST_PESSOA_BIN)

test_svg: $(TEST_SVG_OBJS)
	$(CC) -o $(TEST_SVG_BIN) $^ $(LDFLAGS)
	./$(TEST_SVG_BIN)

test_qry: $(TEST_QRY_OBJS)
	$(CC) -o $(TEST_QRY_BIN) $^ $(LDFLAGS)
	./$(TEST_QRY_BIN)

# Executa todos os testes
tstall: test_hashfile test_banco test_geo test_pessoa test_svg test_qry

# Regra genérica para compilação
%.o: %.c
	$(CC) -c $(CFLAGS) -I$(SRC_DIR) -I$(UNITY_DIR) $< -o $@

# Limpeza
clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(UNITY_DIR)/*.o
	rm -f $(PROJ_NAME)
	rm -f $(TEST_HASHFILE_BIN) $(TEST_BANCO_BIN) $(TEST_GEO_BIN)
	rm -f $(TEST_PESSOA_BIN) $(TEST_SVG_BIN) $(TEST_QRY_BIN)
	rm -f *.hf *.hfd *.svg *.txt
	rm -rf saida

# Limpeza completa (inclui arquivos de teste)
clean-all: clean
	rm -f $(SRC_DIR)/*~ $(TEST_DIR)/*~ 2>/dev/null || true

# Dependências
$(SRC_DIR)/banco.o: $(SRC_DIR)/banco.h $(SRC_DIR)/hashfile.h
$(SRC_DIR)/hashfile.o: $(SRC_DIR)/hashfile.h
$(SRC_DIR)/file_reader.o: $(SRC_DIR)/file_reader.h $(SRC_DIR)/geo.h $(SRC_DIR)/qry.h $(SRC_DIR)/pessoa.h
$(SRC_DIR)/geo.o: $(SRC_DIR)/geo.h $(SRC_DIR)/banco.h $(SRC_DIR)/svg.h
$(SRC_DIR)/pessoa.o: $(SRC_DIR)/pessoa.h $(SRC_DIR)/banco.h
$(SRC_DIR)/qry.o: $(SRC_DIR)/qry.h $(SRC_DIR)/banco.h $(SRC_DIR)/svg.h
$(SRC_DIR)/svg.o: $(SRC_DIR)/svg.h
$(SRC_DIR)/main.o: $(SRC_DIR)/file_reader.h $(SRC_DIR)/banco.h $(SRC_DIR)/svg.h $(SRC_DIR)/qry.h

$(TEST_DIR)/test_hashfile.o: $(SRC_DIR)/hashfile.h $(UNITY_DIR)/unity.h
$(TEST_DIR)/test_banco.o: $(SRC_DIR)/banco.h $(UNITY_DIR)/unity.h
$(TEST_DIR)/test_geo.o: $(SRC_DIR)/geo.h $(SRC_DIR)/banco.h $(SRC_DIR)/svg.h $(UNITY_DIR)/unity.h
$(TEST_DIR)/test_pessoa.o: $(SRC_DIR)/pessoa.h $(SRC_DIR)/banco.h $(UNITY_DIR)/unity.h
$(TEST_DIR)/test_svg.o: $(SRC_DIR)/svg.h $(UNITY_DIR)/unity.h
$(TEST_DIR)/test_qry.o: $(SRC_DIR)/qry.h $(SRC_DIR)/banco.h $(SRC_DIR)/svg.h $(UNITY_DIR)/unity.h

# Help
help:
	@echo "Comandos disponíveis:"
	@echo "  make            - Compila o programa principal (ted)"
	@echo "  make test_hashfile - Executa testes do hashfile"
	@echo "  make test_banco    - Executa testes do banco"
	@echo "  make test_geo      - Executa testes do geo"
	@echo "  make test_pessoa   - Executa testes do pessoa"
	@echo "  make test_svg      - Executa testes do svg"
	@echo "  make test_qry      - Executa testes do qry"
	@echo "  make tstall        - Executa todos os testes"
	@echo "  make clean         - Remove arquivos objeto e executáveis"
	@echo "  make clean-all     - Limpeza completa"

.PHONY: all clean clean-all tstall test_hashfile test_banco test_geo test_pessoa test_svg test_qry help