#include "transaction.h"

/**
 * fill_coinbase - Adds the input and output of a coinbase transaction
 *
 * @tx: Coinbase transaction
 * @in: Input to add, owned by @tx upon success
 * @out: Output to add, owned by @tx upon success
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int fill_coinbase(transaction_t *tx, tx_in_t *in, tx_out_t *out)
{
	if (llist_add_node(tx->inputs, in, ADD_NODE_REAR) != 0)
	{
		free(in);
		free(out);
		return (-1);
	}
	if (llist_add_node(tx->outputs, out, ADD_NODE_REAR) != 0)
	{
		free(out);
		return (-1);
	}

	return (0);
}

/**
 * coinbase_create - Creates a coinbase transaction
 *
 * @receiver: Public key of the miner, who receives the coinbase coins
 * @block_index: Index of the Block the coinbase transaction belongs to
 *
 * Return: Pointer to the created transaction, or NULL upon failure
 */
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index)
{
	transaction_t *tx;
	tx_in_t *in;
	tx_out_t *out;
	uint8_t pub[EC_PUB_LEN];

	if (!receiver || !ec_to_pub(receiver, pub))
		return (NULL);

	tx = calloc(1, sizeof(*tx));
	if (!tx)
		return (NULL);
	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	in = calloc(1, sizeof(*in));
	out = tx_out_create(COINBASE_AMOUNT, pub);
	if (!tx->inputs || !tx->outputs || !in || !out)
	{
		free(in);
		free(out);
		transaction_destroy(tx);
		return (NULL);
	}

	memcpy(in->tx_out_hash, &block_index, sizeof(block_index));
	if (fill_coinbase(tx, in, out) != 0 || !transaction_hash(tx, tx->id))
	{
		transaction_destroy(tx);
		return (NULL);
	}

	return (tx);
}
