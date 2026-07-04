CC = gcc
CFLAGS = -Wall -Wextra -g
BISON = bison
FLEX = flex
BUILD_DIR = build
SRC_DIR = src

all: zelx

zelx: $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/codegen.o $(BUILD_DIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^ -lfl

$(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h: $(SRC_DIR)/parser.y
	$(BISON) -d -o $(BUILD_DIR)/parser.tab.c $<

$(BUILD_DIR)/lex.yy.c: $(SRC_DIR)/lex.l $(BUILD_DIR)/parser.tab.h
	$(FLEX) -o $@ $<

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -I$(SRC_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -I$(SRC_DIR) -c $< -o $@

clean:
	rm -f zelx $(BUILD_DIR)/*.c $(BUILD_DIR)/*.h $(BUILD_DIR)/*.o output.c test_bin

test: zelx
	@echo 'nama = "Budi"' > test.zx
	@echo 'umur = 25' >> test.zx
	@echo 'berat = 70.5' >> test.zx
	@echo 'active = true' >> test.zx
	@echo 'const VERSION = "1.0.0"' >> test.zx
	./zelx test.zx
	gcc output.c -o test_bin
	./test_bin

.PHONY: all clean test
