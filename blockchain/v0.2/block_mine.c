#include "blockchain.h"

/**
 * block_mine - Mines a Block in order to insert it in the Blockchain
 *
 * @block: Pointer to the Block to be mined
 */
void block_mine(block_t *block)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];

	if (!block)
		return;

	block->info.nonce = 0;
	for (;;)
	{
		if (!block_hash(block, hash))
			return;
		if (hash_matches_difficulty(hash, block->info.difficulty))
			break;
		block->info.nonce++;
	}

	memcpy(block->hash, hash, SHA256_DIGEST_LENGTH);
}
