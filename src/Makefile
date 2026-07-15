PROJ_NAME = ted
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Wall -Wextra -Wno-format-truncation -D_GNU_SOURCE
LDFLAGS = -O0 -lm

# Diretórios
SRC_DIR = .
TEST_DIR = ./tests
UNITY_DIR = ../unity
OUT_DIR = ../saida
DATA_DIR = ../dados

# Caminho completo do executável
EXEC = $(SRC_DIR)/$(PROJ_NAME)

# ==================== ARQUIVOS FONTE ====================
MAIN_SRCS = $(SRC_DIR)/main.c \
            $(SRC_DIR)/banco.c \
            $(SRC_DIR)/hashfile.c \
            $(SRC_DIR)/file_reader.c \
            $(SRC_DIR)/geo.c \
            $(SRC_DIR)/pessoa.c \
            $(SRC_DIR)/qry.c \
            $(SRC_DIR)/svg.c

MAIN_OBJS = $(MAIN_SRCS:.c=.o)

# ==================== UNITY ====================
UNITY_SRC = $(UNITY_DIR)/unity.c
UNITY_OBJ = $(UNITY_DIR)/unity.o

# ==================== TESTES ====================
TEST_HASHFILE_SRCS = $(TEST_DIR)/test_hashfile.c $(SRC_DIR)/hashfile.c $(UNITY_SRC)
TEST_HASHFILE_OBJS = $(TEST_HASHFILE_SRCS:.c=.o)
TEST_HASHFILE_BIN = $(TEST_DIR)/test_hashfile

TEST_BANCO_SRCS = $(TEST_DIR)/test_banco.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c $(SRC_DIR)/svg.c $(UNITY_SRC)
TEST_BANCO_OBJS = $(TEST_BANCO_SRCS:.c=.o)
TEST_BANCO_BIN = $(TEST_DIR)/test_banco

TEST_GEO_SRCS = $(TEST_DIR)/test_geo.c $(SRC_DIR)/geo.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c $(SRC_DIR)/svg.c $(UNITY_SRC)
TEST_GEO_OBJS = $(TEST_GEO_SRCS:.c=.o)
TEST_GEO_BIN = $(TEST_DIR)/test_geo

TEST_PESSOA_SRCS = $(TEST_DIR)/test_pessoa.c $(SRC_DIR)/pessoa.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c $(SRC_DIR)/svg.c $(UNITY_SRC)
TEST_PESSOA_OBJS = $(TEST_PESSOA_SRCS:.c=.o)
TEST_PESSOA_BIN = $(TEST_DIR)/test_pessoa

TEST_QRY_SRCS = $(TEST_DIR)/test_qry.c $(SRC_DIR)/qry.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c $(SRC_DIR)/svg.c $(UNITY_SRC)
TEST_QRY_OBJS = $(TEST_QRY_SRCS:.c=.o)
TEST_QRY_BIN = $(TEST_DIR)/test_qry

TEST_SVG_SRCS = $(TEST_DIR)/test_svg.c $(SRC_DIR)/svg.c $(UNITY_SRC)
TEST_SVG_OBJS = $(TEST_SVG_SRCS:.c=.o)
TEST_SVG_BIN = $(TEST_DIR)/test_svg

# ==================== REGRAS PRINCIPAIS ====================

all: $(EXEC)

$(EXEC): $(MAIN_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo ""
	@echo "========================================"
	@echo "✅ Executável criado em: $(EXEC)"
	@echo "========================================"
	@echo ""

%.o: %.c
	$(CC) -c $(CFLAGS) -I$(SRC_DIR) -I$(UNITY_DIR) $< -o $@

# ==================== TESTES ====================

test_hashfile: $(TEST_HASHFILE_OBJS)
	$(CC) -o $(TEST_HASHFILE_BIN) $^ $(LDFLAGS)
	@echo ""
	@echo "========================================"
	@echo "🧪 TESTANDO HASHFILE"
	@echo "========================================"
	@cd $(TEST_DIR) && ./test_hashfile
	@echo ""

test_banco: $(TEST_BANCO_OBJS)
	$(CC) -o $(TEST_BANCO_BIN) $^ $(LDFLAGS)
	@echo ""
	@echo "========================================"
	@echo "🧪 TESTANDO BANCO"
	@echo "========================================"
	@cd $(TEST_DIR) && ./test_banco
	@echo ""

test_geo: $(TEST_GEO_OBJS)
	$(CC) -o $(TEST_GEO_BIN) $^ $(LDFLAGS)
	@echo ""
	@echo "========================================"
	@echo "🧪 TESTANDO GEO"
	@echo "========================================"
	@cd $(TEST_DIR) && ./test_geo
	@echo ""

test_pessoa: $(TEST_PESSOA_OBJS)
	$(CC) -o $(TEST_PESSOA_BIN) $^ $(LDFLAGS)
	@echo ""
	@echo "========================================"
	@echo "🧪 TESTANDO PESSOA"
	@echo "========================================"
	@cd $(TEST_DIR) && ./test_pessoa
	@echo ""

test_qry: $(TEST_QRY_OBJS)
	$(CC) -o $(TEST_QRY_BIN) $^ $(LDFLAGS)
	@echo ""
	@echo "========================================"
	@echo "🧪 TESTANDO QRY"
	@echo "========================================"
	@cd $(TEST_DIR) && ./test_qry
	@echo ""

test_svg: $(TEST_SVG_OBJS)
	$(CC) -o $(TEST_SVG_BIN) $^ $(LDFLAGS)
	@echo ""
	@echo "========================================"
	@echo "🧪 TESTANDO SVG"
	@echo "========================================"
	@cd $(TEST_DIR) && ./test_svg
	@echo ""

tstall: test_hashfile test_banco test_geo test_pessoa test_qry test_svg
	@echo ""
	@echo "========================================"
	@echo "✅ TODOS OS TESTES CONCLUÍDOS!"
	@echo "========================================"
	@echo ""

# ==================== EXECUÇÃO ====================

run: $(EXEC)
	@echo ""
	@echo "========================================"
	@echo "🚀 EXECUTANDO TED"
	@echo "========================================"
	@cd .. && ./src/$(PROJ_NAME) -f $(DATA_DIR)/c1/c1.geo -pm $(DATA_DIR)/c1/c1.pm -q $(DATA_DIR)/c1/censo.qry -o $(OUT_DIR)
	@echo ""
	@echo "========================================"
	@echo "📁 Resultados salvos em: $(OUT_DIR)/"
	@echo "========================================"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true
	@echo ""

run-geo: $(EXEC)
	@echo ""
	@echo "🚀 Executando (apenas .geo)..."
	@cd .. && ./src/$(PROJ_NAME) -f $(DATA_DIR)/c1/c1.geo -o $(OUT_DIR)
	@echo ""
	@echo "📁 Resultados em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

run-pm: $(EXEC)
	@echo ""
	@echo "🚀 Executando (.geo + .pm)..."
	@cd .. && ./src/$(PROJ_NAME) -f $(DATA_DIR)/c1/c1.geo -pm $(DATA_DIR)/c1/c1.pm -o $(OUT_DIR)
	@echo ""
	@echo "📁 Resultados em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# ==================== LIMPEZA ====================

clean:
	@echo "🧹 Removendo arquivos objeto..."
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(UNITY_DIR)/*.o
	@echo "🧹 Removendo executáveis..."
	rm -f $(EXEC)
	rm -f $(TEST_DIR)/test_hashfile $(TEST_DIR)/test_banco
	rm -f $(TEST_DIR)/test_geo $(TEST_DIR)/test_pessoa
	rm -f $(TEST_DIR)/test_qry $(TEST_DIR)/test_svg
	@echo "🧹 Removendo arquivos gerados..."
	rm -f $(TEST_DIR)/*.hf $(TEST_DIR)/*.hfd $(TEST_DIR)/*.hfc
	rm -f $(TEST_DIR)/*.txt $(TEST_DIR)/*.svg
	rm -f $(TEST_DIR)/estado.txt
	rm -f $(SRC_DIR)/*.hf $(SRC_DIR)/*.hfd $(SRC_DIR)/*.hfc 2>/dev/null || true
	@echo ""
	@echo "✅ Limpeza concluída!"
	@echo ""

clean-out:
	rm -f $(OUT_DIR)/*.hf $(OUT_DIR)/*.hfd $(OUT_DIR)/*.hfc 2>/dev/null || true
	rm -f $(OUT_DIR)/*.txt $(OUT_DIR)/*.svg 2>/dev/null || true
	rm -f $(OUT_DIR)/estado.txt 2>/dev/null || true
	@echo "🧹 Arquivos de saída removidos!"

# ==================== DEPENDÊNCIAS ====================

$(SRC_DIR)/banco.o: $(SRC_DIR)/banco.h $(SRC_DIR)/hashfile.h $(SRC_DIR)/svg.h
$(SRC_DIR)/hashfile.o: $(SRC_DIR)/hashfile.h
$(SRC_DIR)/file_reader.o: $(SRC_DIR)/file_reader.h $(SRC_DIR)/geo.h $(SRC_DIR)/qry.h $(SRC_DIR)/pessoa.h
$(SRC_DIR)/geo.o: $(SRC_DIR)/geo.h $(SRC_DIR)/banco.h
$(SRC_DIR)/pessoa.o: $(SRC_DIR)/pessoa.h $(SRC_DIR)/banco.h
$(SRC_DIR)/qry.o: $(SRC_DIR)/qry.h $(SRC_DIR)/banco.h $(SRC_DIR)/svg.h
$(SRC_DIR)/svg.o: $(SRC_DIR)/svg.h
$(SRC_DIR)/main.o: $(SRC_DIR)/file_reader.h $(SRC_DIR)/banco.h $(SRC_DIR)/svg.h $(SRC_DIR)/qry.h $(SRC_DIR)/hashfile.h

$(UNITY_OBJ): $(UNITY_SRC)
	$(CC) -c $(CFLAGS) -I$(UNITY_DIR) $< -o $@

# ==================== HELP ====================

help:
	@echo "========================================="
	@echo "COMANDOS DISPONÍVEIS"
	@echo "========================================="
	@echo ""
	@echo "📦 COMPILAÇÃO:"
	@echo "  make              - Compila o programa principal"
	@echo "  make clean        - Remove objetos e executáveis"
	@echo "  make clean-out    - Remove arquivos de saída"
	@echo ""
	@echo "🧪 TESTES:"
	@echo "  make test_hashfile - Testes do hashfile"
	@echo "  make test_banco    - Testes do banco"
	@echo "  make test_geo      - Testes do geo"
	@echo "  make test_pessoa   - Testes do pessoa"
	@echo "  make test_qry      - Testes do qry"
	@echo "  make test_svg      - Testes do svg"
	@echo "  make tstall        - Executa todos os testes"
	@echo ""
	@echo "🚀 EXECUÇÃO:"
	@echo "  make run          - Executa com todos os dados"
	@echo "  make run-geo      - Executa apenas com .geo"
	@echo "  make run-pm       - Executa com .geo + .pm"
	@echo ""

.PHONY: all clean clean-out tstall
.PHONY: test_hashfile test_banco test_geo test_pessoa test_qry test_svg
.PHONY: run run-geo run-pm help
