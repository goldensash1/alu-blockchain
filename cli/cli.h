#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "blockchain.h"

#define PROMPT "cli> "
#define MINE_DATA "Holberton"
#define PATH_MAX_LEN 4096

/**
 * struct cli_s - State of the CLI
 *
 * @wallet: EC key pair of the user
 * @blockchain: Local Blockchain
 * @pool: Local transaction pool, a list of transaction_t
 */
typedef struct cli_s
{
	EC_KEY *wallet;
	blockchain_t *blockchain;
	llist_t *pool;
} cli_t;

int fail(char const *msg);
void print_hex(uint8_t const *buf, size_t len);
int parse_hex(char const *str, uint8_t *buf, size_t len);
int parse_amount(char const *str, uint32_t *amount);
void print_address(cli_t const *cli);
int cmd_wallet_load(cli_t *cli, char *args);
int cmd_wallet_save(cli_t *cli, char *args);
int cmd_send(cli_t *cli, char *args);
int cmd_mine(cli_t *cli, char *args);
int cmd_info(cli_t *cli, char *args);
int cmd_load(cli_t *cli, char *args);
int cmd_save(cli_t *cli, char *args);

#endif /* CLI_H */
