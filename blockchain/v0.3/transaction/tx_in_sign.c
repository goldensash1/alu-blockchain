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
 * tx_in_sign - Signs a transaction input, given the ID of its transaction
 *
 * @in: Pointer to the transaction input to sign
 * @tx_id: ID of the transaction the input is stored in
 * @sender: Private key of the owner of the referenced output
 * @all_unspent: List of all unspent transaction outputs
 *
 * Return: Pointer to the resulting signature, or NULL upon failure
 */
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH],
	EC_KEY const *sender, llist_t *all_unspent)
{
	unspent_tx_out_t *unspent;
	uint8_t pub[EC_PUB_LEN];

	if (!in || !tx_id || !sender || !all_unspent)
		return (NULL);

	unspent = find_unspent(in, all_unspent);
	if (!unspent || !ec_to_pub(sender, pub) ||
		memcmp(pub, unspent->out.pub, EC_PUB_LEN) != 0)
		return (NULL);

	if (!ec_sign(sender, tx_id, SHA256_DIGEST_LENGTH, &in->sig))
		return (NULL);

	return (&in->sig);
}
