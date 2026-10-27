CC = gcc

NAME = minishell

RM = rm -rf

ARFLAGS = rcs

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = ${BUILD_DIR}/obj
BIN_DIR = ${BUILD_DIR}/bin

CPPFLAGS = -I${SRC_DIR}
CFLAGS = -Wall

# Fedora ne cherche pas criterion.pc dans /usr/local/lib/pkgconfig
PKG_CONFIG_PATH := /usr/local/lib/pkgconfig:$(PKG_CONFIG_PATH)
export PKG_CONFIG_PATH

CRITERION_CFLAGS = $(shell pkg-config --cflags criterion 2>/dev/null)
CRITERION_LIBS = $(shell pkg-config --libs criterion 2>/dev/null)

SRCS = $(shell find ${SRC_DIR} -name '*.c' ! -name 'main.c' ! -path '*/tests/*')
OBJS = $(patsubst ${SRC_DIR}/%.c, ${OBJ_DIR}/%.o, ${SRCS})
MAIN_OBJ = ${OBJ_DIR}/main.o
LIB = ${BUILD_DIR}/libminishell.a

UNIT_SRCS = $(shell find ${TEST_DIR}/unit -name '*.c' 2>/dev/null)
UNIT_OBJS = $(patsubst ${TEST_DIR}/unit/%.c, ${OBJ_DIR}/tests/unit/%.o, ${UNIT_SRCS})
UNIT_BIN = ${BIN_DIR}/unit_tests
UNIT_CPPFLAGS = ${CPPFLAGS} -I${TEST_DIR}/unit ${CRITERION_CFLAGS}

DEPS = ${OBJS:.o=.d} ${MAIN_OBJ:.o=.d} ${UNIT_OBJS:.o=.d}

.PHONY: all check clean test run test-unit fclean re

all: ${NAME}

${NAME}: ${MAIN_OBJ} ${LIB}
	${CC} $^ -o $@

${LIB}: ${OBJS}
	@mkdir -p $(dir $@)
	${RM} $@
	${AR} ${ARFLAGS} $@ $^

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
	@mkdir -p $(dir $@)
	${CC} ${CPPFLAGS} ${CFLAGS} -MMD -MP -c $< -o $@

${OBJ_DIR}/tests/unit/%.o: ${TEST_DIR}/unit/%.c
	@mkdir -p $(dir $@)
	${CC} ${UNIT_CPPFLAGS} ${CFLAGS} -MMD -MP -c $< -o $@

${UNIT_BIN}: ${UNIT_OBJS} ${LIB}
	@mkdir -p $(dir $@)
	@test -n "$(CRITERION_LIBS)" || { \
		echo "Criterion introuvable: pkg-config --libs criterion est vide"; \
		echo "Installe Criterion ou verifie PKG_CONFIG_PATH"; exit 1; }
	${CC} $^ ${CRITERION_LIBS} -o $@

run: ${NAME}
	./${NAME} ${ARGS}

check: test

test: test-unit

test-unit: ${UNIT_BIN}
	./${UNIT_BIN} ${UNIT_ARGS}

clean:
	${RM} ${BUILD_DIR}

fclean: clean
	${RM} ${NAME}

re:
	${MAKE} fclean
	${MAKE} all

help:
	@echo "usage: make [target]"
	@echo "  all        build $(NAME) (default)"
	@echo "  run        lance le programme $(NAME) (ARGS=\"...\")"
	@echo "  test       lance les tests unitaires"
	@echo "  test-unit  lance les tests unitaires Criterion (UNIT_ARGS=\"...\")"
	@echo "  check      alias pour test"
	@echo "  clean      supprime $(BUILD_DIR)/"
	@echo "  fclean     clean + supprime $(NAME)"
	@echo "  re         fclean + all"

# @echo "  test-func  run functional tests"

-include ${DEPS}
