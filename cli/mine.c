#include "cli.h"

/**
 * conflicts - Checks whether a transaction spends an output that a
 * transaction of a list already spends
 *
 * @txs: List of accepted transactions
 * @tx: Transaction to check
 *
 * Return: 1 if there is a conflict, or 0 otherwise
 */
static int conflicts(llist_t *txs, transaction_t const *tx)
{
	transaction_t *other;
	tx_in_t *in, *cur;
	size_t len = 3 * SHA256_DIGEST_LENGTH;
	int i, j, k;

	for (i = 0; i < llist_size(txs); i++)
	{
		other = llist_get_node_at(txs, i);
		for (j = 0; other && j < llist_size(other->inputs); j++)
		{
			in = llist_get_node_at(other->inputs, j);
			for (k = 0; k < llist_size(tx->inputs); k++)
			{
				cur = llist_get_node_at(tx->inputs, k);
				if (!memcmp(in, cur, len))
					return (1);
			}
		}
	}

	return (0);
}

/**
 * fill_block - Moves the valid transactions of the pool into a Block, and
 * deletes the invalid ones
 *
 * @cli: State of the CLI
 * @block: Block to fill
 */
static void fill_block(cli_t *cli, block_t *block)
{
	transaction_t *tx;
	llist_t *txs = block->transactions;

	while ((tx = llist_pop(cli->pool)))
	{
		if (transaction_is_valid(tx, cli->blockchain->unspent) &&
			!conflicts(txs, tx) &&
			llist_add_node(txs, tx, ADD_NODE_REAR) == 0)
			continue;
		transaction_destroy(tx);
	}
}

/**
 * commit_block - Verifies a mined Block, updates the list of unspent outputs
 * and adds the Block to the Blockchain
 *
 * @cli: State of the CLI
 * @block: Mined Block
 * @prev: Previous Block in the Blockchain
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int commit_block(cli_t *cli, block_t *block, block_t const *prev)
{
	blockchain_t *bc = cli->blockchain;
	llist_t *unspent;

	if (block_is_valid(block, prev, bc->unspent) != 0)
		return (-1);

	unspent = update_unspent(block->transactions, block->hash,
		bc->unspent);
	if (!unspent)
		return (-1);
	bc->unspent = unspent;

	return (llist_add_node(bc->chain, block, ADD_NODE_REAR));
}

/**
 * cmd_mine - Mines a Block and adds it to the Blockchain
 *
 * @cli: State of the CLI
 * @args: Unused
 *
 * Return: 0 upon success, or -1 upon failure
 */
int cmd_mine(cli_t *cli, char *args)
{
	block_t *block, *prev;
	transaction_t *coinbase;

	(void)args;
	prev = llist_get_node_at(cli->blockchain->chain,
		llist_size(cli->blockchain->chain) - 1);
	block = block_create(prev, (int8_t *)MINE_DATA, strlen(MINE_DATA));
	if (!block)
		return (fail("Error: unable to create a Block"));

	block->info.difficulty = blockchain_difficulty(cli->blockchain);
	coinbase = coinbase_create(cli->wallet, block->info.index);
	if (!coinbase ||
		llist_add_node(block->transactions, coinbase,
			ADD_NODE_FRONT) != 0)
	{
		transaction_destroy(coinbase);
		block_destroy(block);
		return (fail("Error: unable to create a coinbase"));
	}

	fill_block(cli, block);
	block_mine(block);
	if (commit_block(cli, block, prev) != 0)
	{
		block_destroy(block);
		return (fail("Error: the mined Block is not valid"));
	}

	printf("Block mined: index %u, difficulty %u, %d transaction(s), hash ",
		block->info.index, block->info.difficulty,
		llist_size(block->transactions));
	print_hex(block->hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
}
