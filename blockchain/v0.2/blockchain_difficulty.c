#include "blockchain.h"

/**
 * blockchain_difficulty - Computes the difficulty to assign to the next Block
 *
 * @blockchain: Pointer to the Blockchain to analyze
 *
 * Return: Difficulty to be assigned to the next Block
 */
uint32_t blockchain_difficulty(blockchain_t const *blockchain)
{
	block_t *last, *adjust;
	int64_t expected, taken;
	int size;

	if (!blockchain || !blockchain->chain)
		return (0);

	size = llist_size(blockchain->chain);
	last = size > 0 ? llist_get_node_at(blockchain->chain, size - 1) : NULL;
	if (!last)
		return (0);
	if (last->info.index == 0 ||
		last->info.index % DIFFICULTY_ADJUSTMENT_INTERVAL != 0 ||
		size < DIFFICULTY_ADJUSTMENT_INTERVAL)
		return (last->info.difficulty);

	adjust = llist_get_node_at(blockchain->chain,
		size - DIFFICULTY_ADJUSTMENT_INTERVAL);
	if (!adjust)
		return (last->info.difficulty);

	expected = BLOCK_GENERATION_INTERVAL * DIFFICULTY_ADJUSTMENT_INTERVAL;
	taken = (int64_t)last->info.timestamp - (int64_t)adjust->info.timestamp;
	if (2 * taken < expected)
		return (last->info.difficulty + 1);
	if (taken > 2 * expected && last->info.difficulty > 0)
		return (last->info.difficulty - 1);

	return (last->info.difficulty);
}
