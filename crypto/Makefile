CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -I.
LDFLAGS = -lssl -lcrypto
AR = ar
ARFLAGS = rcs

LIB = libhblk_crypto.a
SRC = sha256.c \
      ec_create.c \
      ec_to_pub.c \
      ec_from_pub.c \
      ec_save.c \
      ec_load.c \
      ec_sign.c \
      ec_verify.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean oclean fclean re

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

%.o: %.c hblk_crypto.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

oclean: clean

fclean: clean
	$(RM) $(LIB)

re: fclean all
