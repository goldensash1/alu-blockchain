#include "blockchain.h"

/**
 * block_mine - Mines a Block in order to insert it in the Blockchain
 *
 * @block: Pointer to the Block to be mined
 */
void block_mine(block_t *block)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	uint64_t nonce = 0;

	if (!block)
		return;

	for (;;)
	{
		if (!block_hash_nonce(block, nonce, hash))
			return;
		if (hash_matches_difficulty(hash, block->info.difficulty))
			break;
		nonce++;
	}

	block->info.nonce = nonce;
	memcpy(block->hash, hash, SHA256_DIGEST_LENGTH);
}
