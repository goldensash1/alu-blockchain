#include "blockchain.h"

/**
 * is_genesis - Checks that a Block matches the Genesis Block
 *
 * @block: Pointer to the Block to check
 *
 * Return: 1 if @block is the Genesis Block, or 0 otherwise
 */
static int is_genesis(block_t const *block)
{
	static uint8_t const zeros[SHA256_DIGEST_LENGTH];
	size_t hlen = SHA256_DIGEST_LENGTH, dlen = GENESIS_DATA_LEN;

	return (block->info.index == 0 && block->info.difficulty == 0 &&
		block->info.timestamp == GENESIS_TIMESTAMP &&
		block->info.nonce == 0 &&
		memcmp(block->info.prev_hash, zeros, hlen) == 0 &&
		block->data.len == GENESIS_DATA_LEN &&
		memcmp(block->data.buffer, GENESIS_DATA, dlen) == 0 &&
		memcmp(block->hash, GENESIS_HASH, hlen) == 0);
}

/**
 * hash_matches - Checks that the computed hash of a Block matches its hash
 *
 * @block: Pointer to the Block to check
 *
 * Return: 1 if the hashes match, or 0 otherwise
 */
static int hash_matches(block_t const *block)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];

	if (!block_hash(block, hash))
		return (0);
	return (memcmp(hash, block->hash, SHA256_DIGEST_LENGTH) == 0);
}

/**
 * transactions_are_valid - Checks the list of transactions of a Block
 *
 * @block: Pointer to the Block to check
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Return: 1 if the transactions are valid, or 0 otherwise
 */
static int transactions_are_valid(block_t const *block, llist_t *all_unspent)
{
	transaction_t *tx;
	int i, n;

	if (!block->transactions)
		return (0);

	n = llist_size(block->transactions);
	if (n < 1 || !coinbase_is_valid(llist_get_head(block->transactions),
		block->info.index))
		return (0);

	for (i = 1; i < n; i++)
	{
		tx = llist_get_node_at(block->transactions, i);
		if (!transaction_is_valid(tx, all_unspent))
			return (0);
	}

	return (1);
}

/**
 * block_is_valid - Verifies that a Block is valid
 *
 * @block: Pointer to the Block to check
 * @prev_block: Pointer to the previous Block in the Blockchain, or NULL if
 * @block is the first Block of the chain
 * @all_unspent: List of all unspent transaction outputs to date
 *
 * Return: 0 if the Block is valid, or -1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block,
	llist_t *all_unspent)
{
	if (!block || block->data.len > BLOCKCHAIN_DATA_MAX)
		return (-1);

	if (block->info.index == 0)
		return (is_genesis(block) ? 0 : -1);

	if (!prev_block || block->info.index != prev_block->info.index + 1 ||
		!hash_matches(prev_block) ||
		memcmp(block->info.prev_hash, prev_block->hash,
			SHA256_DIGEST_LENGTH) != 0 ||
		!hash_matches(block) ||
		!hash_matches_difficulty(block->hash, block->info.difficulty) ||
		!transactions_are_valid(block, all_unspent))
		return (-1);

	return (0);
}
