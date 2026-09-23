#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "blockchain.h"

void _print_hex_buffer(uint8_t const *buf, size_t len);

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	blockchain_t *blockchain;
	block_t *block;

	blockchain = blockchain_create();
	block = block_create(llist_get_head(blockchain->chain),
		(int8_t *)"Holberton", 9);
	block->info.difficulty = 16;

	block_mine(block);
	printf("Nonce: %lu\nHash: ", (unsigned long)block->info.nonce);
	_print_hex_buffer(block->hash, SHA256_DIGEST_LENGTH);
	printf("\n");

	block_destroy(block);
	blockchain_destroy(blockchain);

	return (EXIT_SUCCESS);
}
