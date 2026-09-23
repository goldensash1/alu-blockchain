#include "transaction.h"

/**
 * transaction_hash - Computes the ID (hash) of a transaction
 *
 * @transaction: Pointer to the transaction to hash
 * @hash_buf: Buffer in which to store the computed hash
 *
 * Return: Pointer to @hash_buf, or NULL upon failure
 */
uint8_t *transaction_hash(transaction_t const *transaction,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX ctx;
	tx_in_t *in;
	tx_out_t *out;
	int i, n;

	if (!transaction || !hash_buf || !SHA256_Init(&ctx))
		return (NULL);

	n = llist_size(transaction->inputs);
	for (i = 0; i < n; i++)
	{
		in = llist_get_node_at(transaction->inputs, i);
		if (!in || !SHA256_Update(&ctx, in, 3 * SHA256_DIGEST_LENGTH))
			return (NULL);
	}

	n = llist_size(transaction->outputs);
	for (i = 0; i < n; i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		if (!out || !SHA256_Update(&ctx, out->hash, sizeof(out->hash)))
			return (NULL);
	}

	if (!SHA256_Final(hash_buf, &ctx))
		return (NULL);

	return (hash_buf);
}
