#include "transaction.h"

/**
 * coinbase_is_valid - Checks whether a coinbase transaction is valid
 *
 * @coinbase: Pointer to the coinbase transaction to verify
 * @block_index: Index of the Block the coinbase transaction belongs to
 *
 * Return: 1 if the coinbase transaction is valid, or 0 otherwise
 */
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index)
{
	static uint8_t const zeros[SHA256_DIGEST_LENGTH];
	static sig_t const no_sig;
	uint8_t hash[SHA256_DIGEST_LENGTH];
	tx_in_t *in;
	tx_out_t *out;

	if (!coinbase || !transaction_hash(coinbase, hash) ||
		memcmp(hash, coinbase->id, SHA256_DIGEST_LENGTH) != 0 ||
		llist_size(coinbase->inputs) != 1 ||
		llist_size(coinbase->outputs) != 1)
		return (0);

	in = llist_get_head(coinbase->inputs);
	out = llist_get_head(coinbase->outputs);
	if (!in || !out ||
		memcmp(in->tx_out_hash, &block_index, sizeof(uint32_t)) != 0 ||
		memcmp(in->block_hash, zeros, sizeof(zeros)) != 0 ||
		memcmp(in->tx_id, zeros, sizeof(zeros)) != 0 ||
		memcmp(&in->sig, &no_sig, sizeof(no_sig)) != 0)
		return (0);

	return (out->amount == COINBASE_AMOUNT);
}
