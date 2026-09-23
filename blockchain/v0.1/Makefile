CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -I.
AR = ar
ARFLAGS = rcs

LIB = libhblk_blockchain_v0.1.a
SRC = blockchain_create.c \
      block_create.c \
      block_destroy.c \
      blockchain_destroy.c \
      block_hash.c \
      blockchain_serialize.c \
      blockchain_deserialize.c \
      block_is_valid.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean fclean re

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

%.o: %.c blockchain.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(LIB)

re: fclean all
