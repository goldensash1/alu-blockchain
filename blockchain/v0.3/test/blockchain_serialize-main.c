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
	EC_KEY *miner;

	miner = ec_create();
	blockchain = blockchain_create();
	block = llist_get_head(blockchain->chain);

	block = block_create(block, (int8_t *)"Holberton", 9);
	llist_add_node(block->transactions,
		coinbase_create(miner, block->info.index), ADD_NODE_REAR);
	block_hash(block, block->hash);
	llist_add_node(blockchain->chain, block, ADD_NODE_REAR);
	blockchain->unspent = update_unspent(block->transactions, block->hash,
		blockchain->unspent);

	blockchain_serialize(blockchain, "save.hblk");

	blockchain_destroy(blockchain);
	EC_KEY_free(miner);

	return (EXIT_SUCCESS);
}
