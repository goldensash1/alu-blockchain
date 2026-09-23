#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <llist.h>

#include "blockchain.h"

void _blockchain_print_brief(blockchain_t const *blockchain);

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	blockchain_t *blockchain;
	blockchain_t *blockchain2;

	blockchain = blockchain_create();

	_blockchain_print_brief(blockchain);

	blockchain2 = blockchain_create();
	if (memcmp(llist_get_head(blockchain->chain),
		llist_get_head(blockchain2->chain), sizeof(block_t)) != 0)
	{
		fprintf(stderr, "Genesis Block should always be the same\n");
		blockchain_destroy(blockchain);
		blockchain_destroy(blockchain2);
		return (EXIT_FAILURE);
	}

	blockchain_destroy(blockchain);
	blockchain_destroy(blockchain2);

	return (EXIT_SUCCESS);
}
