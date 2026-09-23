#include "blockchain.h"

/**
 * blockchain_destroy - Deletes an existing Blockchain, along with all the
 * Blocks it contains
 *
 * @blockchain: Pointer to the Blockchain to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	block_t *block;

	if (!blockchain)
		return;

	if (blockchain->chain)
	{
		while ((block = llist_pop(blockchain->chain)))
			block_destroy(block);
		llist_destroy(blockchain->chain, 0, NULL);
	}

	free(blockchain);
}
