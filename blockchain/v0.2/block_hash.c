#include "blockchain.h"

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
	SHA256_CTX ctx;
	uint32_t len;

	if (!block || !hash_buf)
		return (NULL);

	len = block->data.len;
	if (len > BLOCKCHAIN_DATA_MAX)
		len = BLOCKCHAIN_DATA_MAX;

	if (!SHA256_Init(&ctx) ||
		!SHA256_Update(&ctx, &block->info, sizeof(block->info)) ||
		!SHA256_Update(&ctx, block->data.buffer, len) ||
		!SHA256_Final(hash_buf, &ctx))
		return (NULL);

	return (hash_buf);
}
