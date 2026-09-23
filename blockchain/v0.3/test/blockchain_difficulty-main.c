#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "blockchain.h"

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	blockchain_t *blockchain;
	block_t *block;
	int i;

	blockchain = blockchain_create();
	block = llist_get_head(blockchain->chain);
	printf("Difficulty: %u\n", blockchain_difficulty(blockchain));

	for (i = 0; i < 5; i++)
	{
		block = block_create(block, (int8_t *)"Holberton", 9);
		block->info.difficulty = blockchain_difficulty(blockchain);
		llist_add_node(blockchain->chain, block, ADD_NODE_REAR);
	}
	printf("Difficulty: %u\n", blockchain_difficulty(blockchain));

	blockchain_destroy(blockchain);

	return (EXIT_SUCCESS);
}
