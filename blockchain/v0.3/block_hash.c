#include "blockchain.h"

/**
 * block_hash_nonce - Computes the hash of a Block, as if it had a given nonce
 *
 * @block: Pointer to the Block to be hashed
 * @nonce: Nonce to use in place of the Block's nonce
 * @hash_buf: Buffer in which to store the resulting hash
 *
 * Return: Pointer to @hash_buf, or NULL upon failure
 */
uint8_t *block_hash_nonce(block_t const *block, uint64_t nonce,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX ctx;
	block_info_t info;
	transaction_t *tx;
	uint32_t len;
	int i, n = 0;

	if (!block || !hash_buf)
		return (NULL);

	info = block->info;
	info.nonce = nonce;
	len = block->data.len;
	if (len > BLOCKCHAIN_DATA_MAX)
		len = BLOCKCHAIN_DATA_MAX;
	if (block->transactions)
		n = llist_size(block->transactions);

	if (!SHA256_Init(&ctx) || !SHA256_Update(&ctx, &info, sizeof(info)) ||
		!SHA256_Update(&ctx, block->data.buffer, len))
		return (NULL);

	for (i = 0; i < n; i++)
	{
		tx = llist_get_node_at(block->transactions, i);
		if (!tx || !SHA256_Update(&ctx, tx->id, SHA256_DIGEST_LENGTH))
			return (NULL);
	}

	if (!SHA256_Final(hash_buf, &ctx))
		return (NULL);

	return (hash_buf);
}

/**
 * block_hash - Computes the hash of a Block
 *
 * @block: Pointer to the Block to be hashed
 * @hash_buf: Buffer in which to store the resulting hash
 *
 * Return: Pointer to @hash_buf, or NULL upon failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	if (!block)
		return (NULL);

	return (block_hash_nonce(block, block->info.nonce, hash_buf));
}
