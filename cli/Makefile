CC = gcc
CLI_FLAGS = -Wall -Werror -Wextra -pedantic -I. -I../crypto \
	-I../blockchain/v0.3 -I../blockchain/v0.3/transaction
CLI_LIBS = -L../blockchain/v0.3 -L../crypto -lhblk_blockchain -lhblk_crypto \
	-lllist -lssl -lcrypto -pthread

NAME = cli
SRC = main.c utils.c wallet.c send.c mine.c info.c blockchain_io.c
OBJ = $(SRC:.c=.o)

.PHONY: all libs clean fclean re

all: $(NAME)

libs:
	$(MAKE) -C ../crypto libhblk_crypto.a
	$(MAKE) -C ../blockchain/v0.3 libhblk_blockchain.a

$(NAME): libs $(OBJ)
	$(CC) -o $@ $(OBJ) $(CLI_LIBS)

%.o: %.c cli.h
	$(CC) $(CLI_FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
