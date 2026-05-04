PROJ_NAME = ted
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration -Wall -Wextra -Wno-format-truncation -D_GNU_SOURCE
LDFLAGS = -O0

# Diretórios
SRC_DIR = src
TEST_DIR = src/test
UNITY_DIR = unity
OUT_DIR = saida
DATA_DIR = t1

# Arquivos fonte principais
MAIN_SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/banco.c $(SRC_DIR)/hashfile.c \
            $(SRC_DIR)/file_reader.c $(SRC_DIR)/geo.c $(SRC_DIR)/pessoa.c \
            $(SRC_DIR)/qry.c $(SRC_DIR)/svg.c
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

# Testes unitários
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

# ==================== REGRAS PRINCIPAIS ====================

all: $(PROJ_NAME)

$(PROJ_NAME): $(MAIN_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Regra genérica para compilação
%.o: %.c
	$(CC) -c $(CFLAGS) -I$(SRC_DIR) -I$(UNITY_DIR) $< -o $@

# ==================== TESTES UNITÁRIOS ====================

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

# Executa todos os testes unitários
tstall: test_hashfile test_banco test_geo test_pessoa test_svg test_qry

# ==================== EXECUÇÃO COM DADOS ====================

# Apenas o mapa (c1.geo)
run-geo-only: $(PROJ_NAME)
	mkdir -p $(OUT_DIR)
	./$(PROJ_NAME) -f $(DATA_DIR)/c1.geo -o $(OUT_DIR)
	@echo "Resultado salvo em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# Executar para c1 (apenas quadras + pessoas)
run-c1: $(PROJ_NAME)
	mkdir -p $(OUT_DIR)
	./$(PROJ_NAME) -f $(DATA_DIR)/c1.geo -pm $(DATA_DIR)/c1.pm -o $(OUT_DIR)
	@echo "Resultado salvo em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# Executar para c1 com consulta mudanca-todos-moradores
run-c1-qry: $(PROJ_NAME)
	mkdir -p $(OUT_DIR)
	./$(PROJ_NAME) -f $(DATA_DIR)/c1.geo -pm $(DATA_DIR)/c1.pm -q "$(DATA_DIR)/c1/mudanca-todos-moradores.qry" -o $(OUT_DIR)
	@echo "Resultado salvo em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# Executar para c1 com consulta de verificação
run-c1-qry-verif: $(PROJ_NAME)
	mkdir -p $(OUT_DIR)
	./$(PROJ_NAME) -f $(DATA_DIR)/c1.geo -pm $(DATA_DIR)/c1.pm -q "$(DATA_DIR)/c1/mudanca-todos-moradores-com-verif.qry" -o $(OUT_DIR)
	@echo "Resultado salvo em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# Executar para c2 (apenas quadras + pessoas)
run-c2: $(PROJ_NAME)
	mkdir -p $(OUT_DIR)
	./$(PROJ_NAME) -f $(DATA_DIR)/c2.geo -pm $(DATA_DIR)/c2.pm -o $(OUT_DIR)
	@echo "Resultado salvo em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# Executar para c2 com consulta
run-c2-qry: $(PROJ_NAME)
	mkdir -p $(OUT_DIR)
	./$(PROJ_NAME) -f $(DATA_DIR)/c2.geo -pm $(DATA_DIR)/c2.pm -q "$(DATA_DIR)/c2/consultas.qry" -o $(OUT_DIR)
	@echo "Resultado salvo em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# Executar para c3 (apenas quadras + pessoas)
run-c3: $(PROJ_NAME)
	mkdir -p $(OUT_DIR)
	./$(PROJ_NAME) -f $(DATA_DIR)/c3.geo -pm $(DATA_DIR)/c3.pm -o $(OUT_DIR)
	@echo "Resultado salvo em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# Executar para c3 com consulta
run-c3-qry: $(PROJ_NAME)
	mkdir -p $(OUT_DIR)
	./$(PROJ_NAME) -f $(DATA_DIR)/c3.geo -pm $(DATA_DIR)/c3.pm -q "$(DATA_DIR)/c3/consultas.qry" -o $(OUT_DIR)
	@echo "Resultado salvo em: $(OUT_DIR)/"
	@ls -la $(OUT_DIR)/ 2>/dev/null || true

# Executar todas as variações do c1
run-c1-all: run-c1 run-c1-qry run-c1-qry-verif
	@echo "Todos os testes do c1 concluídos!"

# Executar todos os testes de dados (c1, c2, c3)
run-all-data: run-c1 run-c1-qry run-c1-qry-verif run-c2 run-c2-qry run-c3 run-c3-qry
	@echo "========================================="
	@echo "Todos os testes de dados concluídos!"
	@echo "Resultados salvos em $(OUT_DIR)/"
	@echo "========================================="

# ==================== LIMPEZA ====================

clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(UNITY_DIR)/*.o
	rm -f $(PROJ_NAME)
	rm -f $(TEST_HASHFILE_BIN) $(TEST_BANCO_BIN) $(TEST_GEO_BIN)
	rm -f $(TEST_PESSOA_BIN) $(TEST_SVG_BIN) $(TEST_QRY_BIN)
	rm -rf $(OUT_DIR)

clean-all: clean
	rm -f $(SRC_DIR)/*~ $(TEST_DIR)/*~ 2>/dev/null || true

# ==================== DEPENDÊNCIAS ====================

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

# ==================== HELP ====================

help:
	@echo "========================================="
	@echo "COMANDOS DISPONÍVEIS"
	@echo "========================================="
	@echo ""
	@echo "COMPILAÇÃO:"
	@echo "  make              - Compila o programa principal (ted)"
	@echo "  make clean        - Remove arquivos objeto e executáveis"
	@echo "  make clean-all    - Limpeza completa"
	@echo ""
	@echo "TESTES UNITÁRIOS:"
	@echo "  make test_hashfile - Executa testes do hashfile"
	@echo "  make test_banco    - Executa testes do banco"
	@echo "  make test_geo      - Executa testes do geo"
	@echo "  make test_pessoa   - Executa testes do pessoa"
	@echo "  make test_svg      - Executa testes do svg"
	@echo "  make test_qry      - Executa testes do qry"
	@echo "  make tstall        - Executa todos os testes unitários"
	@echo ""
	@echo "TESTES COM DADOS:"
	@echo "  make run-geo-only    - Apenas o mapa (c1.geo)"
	@echo "  make run-c1          - c1.geo + c1.pm"
	@echo "  make run-c1-qry      - c1 + consulta mudanca-todos-moradores"
	@echo "  make run-c1-qry-verif - c1 + consulta de verificação"
	@echo "  make run-c2          - c2.geo + c2.pm"
	@echo "  make run-c2-qry      - c2 + consultas.qry"
	@echo "  make run-c3          - c3.geo + c3.pm"
	@echo "  make run-c3-qry      - c3 + consultas.qry"
	@echo "  make run-c1-all      - Executa todas as variações do c1"
	@echo "  make run-all-data    - Executa todos os testes de dados"
	@echo ""
	@echo "EXEMPLOS:"
	@echo "  make run-c1-qry-verif  # Executa verificação do c1"
	@echo "  make run-all-data      # Executa tudo"
	@echo "  make tstall            # Executa todos os testes unitários"
	@echo ""

# ==================== PHONY ====================

.PHONY: all clean clean-all tstall
.PHONY: test_hashfile test_banco test_geo test_pessoa test_svg test_qry
.PHONY: run-geo-only run-c1 run-c1-qry run-c1-qry-verif 
.PHONY: run-c2 run-c2-qry run-c3 run-c3-qry
.PHONY: run-c1-all run-all-data help
