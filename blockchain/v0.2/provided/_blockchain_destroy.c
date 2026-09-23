#include <stdlib.h>
#include "../blockchain.h"

/**
 * _blockchain_destroy - Frees a Blockchain and all its Blocks
 *
 * @blockchain: Pointer to the Blockchain to free
 */
void _blockchain_destroy(blockchain_t *blockchain)
{
	block_t *block;

	if (!blockchain)
		return;
	while ((block = llist_pop(blockchain->chain)))
		free(block);
	llist_destroy(blockchain->chain, 0, NULL);
	free(blockchain);
}
