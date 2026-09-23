#include "transaction.h"

/**
 * is_spent - Checks whether an unspent output is consumed by a transaction
 * list
 *
 * @transactions: List of transactions
 * @unspent: Unspent output to look for
 *
 * Return: 1 if @unspent is referenced by an input, or 0 otherwise
 */
static int is_spent(llist_t *transactions, unspent_tx_out_t const *unspent)
{
	transaction_t *tx;
	tx_in_t *in;
	int i, j, n = llist_size(transactions);

	for (i = 0; i < n; i++)
	{
		tx = llist_get_node_at(transactions, i);
		for (j = 0; tx && j < llist_size(tx->inputs); j++)
		{
			in = llist_get_node_at(tx->inputs, j);
			if (in && !memcmp(in->tx_out_hash, unspent->out.hash,
				sizeof(in->tx_out_hash)))
				return (1);
		}
	}

	return (0);
}

/**
 * add_outputs - Adds the outputs of a transaction list as unspent outputs
 *
 * @list: List of unspent outputs to add to
 * @transactions: List of transactions
 * @block_hash: Hash of the Block containing @transactions
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int add_outputs(llist_t *list, llist_t *transactions,
	uint8_t block_hash[SHA256_DIGEST_LENGTH])
{
	transaction_t *tx;
	unspent_tx_out_t *unspent;
	int i, j, n = llist_size(transactions);

	for (i = 0; i < n; i++)
	{
		tx = llist_get_node_at(transactions, i);
		for (j = 0; tx && j < llist_size(tx->outputs); j++)
		{
			unspent = unspent_tx_out_create(block_hash, tx->id,
				llist_get_node_at(tx->outputs, j));
			if (!unspent || llist_add_node(list, unspent,
				ADD_NODE_REAR) != 0)
			{
				free(unspent);
				return (-1);
			}
		}
	}

	return (0);
}

/**
 * update_unspent - Updates the list of all unspent transaction outputs
 *
 * @transactions: List of validated transactions
 * @block_hash: Hash of the validated Block containing @transactions
 * @all_unspent: Current list of unspent transaction outputs, deleted upon
 * success
 *
 * Return: The new list of unspent transaction outputs, or NULL upon failure
 */
llist_t *update_unspent(llist_t *transactions,
	uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *all_unspent)
{
	llist_t *list = llist_create(MT_SUPPORT_FALSE);
	unspent_tx_out_t *unspent, *copy;
	int i, n = llist_size(all_unspent);

	for (i = 0; list && i < n; i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (!unspent || is_spent(transactions, unspent))
			continue;
		copy = malloc(sizeof(*copy));
		if (!copy || llist_add_node(list, memcpy(copy, unspent,
			sizeof(*copy)), ADD_NODE_REAR) != 0)
		{
			free(copy);
			llist_destroy(list, 1, free);
			return (NULL);
		}
	}

	if (!list || add_outputs(list, transactions, block_hash) != 0)
	{
		if (list)
			llist_destroy(list, 1, free);
		return (NULL);
	}

	llist_destroy(all_unspent, 1, free);
	return (list);
}
