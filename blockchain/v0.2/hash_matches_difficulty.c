#include "blockchain.h"

/**
 * hash_matches_difficulty - Checks whether a hash has enough leading zero
 * bits to match a given difficulty
 *
 * @hash: Hash to check
 * @difficulty: Minimum number of leading zero bits the hash must have
 *
 * Return: 1 if the difficulty is respected, or 0 otherwise
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
	uint32_t difficulty)
{
	uint32_t i;

	if (!hash || difficulty > 8 * SHA256_DIGEST_LENGTH)
		return (0);

	for (i = 0; i < difficulty; i++)
		if (hash[i / 8] & (0x80 >> (i % 8)))
			return (0);

	return (1);
}
