#include "transaction.h"

/**
 * find_unspent - Finds the unspent output referenced by a transaction input
 *
 * @in: Pointer to the transaction input
 * @all_unspent: List of all unspent transaction outputs
 *
 * Return: Pointer to the unspent output, or NULL if not found
 */
static unspent_tx_out_t *find_unspent(tx_in_t const *in, llist_t *all_unspent)
{
	unspent_tx_out_t *u;
	size_t len = SHA256_DIGEST_LENGTH;
	int i, n = llist_size(all_unspent);

	for (i = 0; i < n; i++)
	{
		u = llist_get_node_at(all_unspent, i);
		if (u && !memcmp(u->block_hash, in->block_hash, len) &&
			!memcmp(u->tx_id, in->tx_id, len) &&
			!memcmp(u->out.hash, in->tx_out_hash, len))
			return (u);
	}

	return (NULL);
}

/**
 * check_input - Verifies the signature of a transaction input
 *
 * @in: Pointer to the transaction input to verify
 * @id: ID of the transaction the input is stored in
 * @all_unspent: List of all unspent transaction outputs
 * @total: Address at which to add the amount of the referenced output
 *
 * Return: 1 if the input is valid, or 0 otherwise
 */
static int check_input(tx_in_t const *in, uint8_t const *id,
	llist_t *all_unspent, uint32_t *total)
{
	unspent_tx_out_t *unspent = find_unspent(in, all_unspent);
	EC_KEY *key;
	int valid;

	if (!unspent)
		return (0);

	key = ec_from_pub(unspent->out.pub);
	if (!key)
		return (0);

	valid = ec_verify(key, id, SHA256_DIGEST_LENGTH, &in->sig);
	EC_KEY_free(key);
	if (!valid)
		return (0);

	*total += unspent->out.amount;
	return (1);
}

/**
 * transaction_is_valid - Checks whether a transaction is valid
 *
 * @transaction: Pointer to the transaction to verify
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Return: 1 if the transaction is valid, or 0 otherwise
 */
int transaction_is_valid(transaction_t const *transaction,
	llist_t *all_unspent)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	uint32_t in_total = 0, out_total = 0;
	tx_out_t *out;
	int i, n;

	if (!transaction || !all_unspent ||
		!transaction_hash(transaction, hash) ||
		memcmp(hash, transaction->id, SHA256_DIGEST_LENGTH) != 0)
		return (0);

	n = llist_size(transaction->inputs);
	for (i = 0; i < n; i++)
		if (!check_input(llist_get_node_at(transaction->inputs, i),
			transaction->id, all_unspent, &in_total))
			return (0);

	n = llist_size(transaction->outputs);
	for (i = 0; i < n; i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		if (!out)
			return (0);
		out_total += out->amount;
	}

	return (in_total == out_total);
}
