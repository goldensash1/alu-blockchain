#include "blockchain.h"

/**
 * block_destroy - Deletes an existing Block, along with its transactions
 *
 * @block: Pointer to the Block to delete
 */
void block_destroy(block_t *block)
{
	transaction_t *tx;

	if (!block)
		return;

	if (block->transactions)
	{
		while ((tx = llist_pop(block->transactions)))
			transaction_destroy(tx);
		llist_destroy(block->transactions, 0, NULL);
	}

	free(block);
}
