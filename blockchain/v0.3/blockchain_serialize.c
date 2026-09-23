#include "blockchain.h"

/**
 * write_list - Writes the nodes of a list, each on a fixed number of bytes
 *
 * @file: Stream to write to
 * @list: List of nodes to write
 * @bytes: Number of bytes to write for each node
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int write_list(FILE *file, llist_t *list, size_t bytes)
{
	void *node;
	int i, n = llist_size(list);

	for (i = 0; i < n; i++)
	{
		node = llist_get_node_at(list, i);
		if (!node || fwrite(node, bytes, 1, file) != 1)
			return (-1);
	}

	return (0);
}

/**
 * write_tx - Writes a transaction
 *
 * @file: Stream to write to
 * @tx: Transaction to write
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int write_tx(FILE *file, transaction_t const *tx)
{
	uint32_t n_in = llist_size(tx->inputs), n_out = llist_size(tx->outputs);

	if (fwrite(tx->id, sizeof(tx->id), 1, file) != 1 ||
		fwrite(&n_in, sizeof(n_in), 1, file) != 1 ||
		fwrite(&n_out, sizeof(n_out), 1, file) != 1 ||
		write_list(file, tx->inputs, TX_IN_SIZE) != 0 ||
		write_list(file, tx->outputs, TX_OUT_SIZE) != 0)
		return (-1);

	return (0);
}

/**
 * write_block - Writes a Block, along with its transactions
 *
 * @file: Stream to write to
 * @block: Block to write
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int write_block(FILE *file, block_t const *block)
{
	transaction_t *tx;
	int32_t i, n = -1;
	uint32_t len = block->data.len;

	if (block->transactions)
		n = llist_size(block->transactions);

	if (len > BLOCKCHAIN_DATA_MAX ||
		fwrite(&block->info, sizeof(block->info), 1, file) != 1 ||
		fwrite(&len, sizeof(len), 1, file) != 1 ||
		(len && fwrite(block->data.buffer, len, 1, file) != 1) ||
		fwrite(block->hash, sizeof(block->hash), 1, file) != 1 ||
		fwrite(&n, sizeof(n), 1, file) != 1)
		return (-1);

	for (i = 0; i < n; i++)
	{
		tx = llist_get_node_at(block->transactions, i);
		if (!tx || write_tx(file, tx) != 0)
			return (-1);
	}

	return (0);
}

/**
 * write_header - Writes the file header
 *
 * @file: Stream to write to
 * @blockchain: Blockchain to serialize
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int write_header(FILE *file, blockchain_t const *blockchain)
{
	uint16_t probe = 1;
	uint8_t endian = HBLK_BIG_ENDIAN;
	int32_t n_blocks = llist_size(blockchain->chain);
	int32_t n_unspent = llist_size(blockchain->unspent);

	if (*(uint8_t *)&probe == 1)
		endian = HBLK_LITTLE_ENDIAN;
	if (n_unspent < 0)
		n_unspent = 0;

	if (n_blocks < 0 || fwrite(HBLK_MAGIC, HBLK_MAGIC_LEN, 1, file) != 1 ||
		fwrite(HBLK_VERSION, HBLK_VERSION_LEN, 1, file) != 1 ||
		fwrite(&endian, sizeof(endian), 1, file) != 1 ||
		fwrite(&n_blocks, sizeof(n_blocks), 1, file) != 1 ||
		fwrite(&n_unspent, sizeof(n_unspent), 1, file) != 1)
		return (-1);

	return (0);
}

/**
 * blockchain_serialize - Serializes a Blockchain into a file
 *
 * @blockchain: Pointer to the Blockchain to be serialized
 * @path: Path to the file to serialize the Blockchain into
 *
 * Return: 0 upon success, or -1 upon failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *file;
	block_t *block;
	int i, n, ret = 0;

	if (!blockchain || !blockchain->chain || !path)
		return (-1);

	file = fopen(path, "wb");
	if (!file)
		return (-1);

	n = llist_size(blockchain->chain);
	if (write_header(file, blockchain) != 0)
		ret = -1;
	for (i = 0; ret == 0 && i < n; i++)
	{
		block = llist_get_node_at(blockchain->chain, i);
		if (!block || write_block(file, block) != 0)
			ret = -1;
	}
	if (ret == 0 && write_list(file, blockchain->unspent, UNSPENT_SIZE))
		ret = -1;

	if (fclose(file) != 0)
		ret = -1;

	return (ret);
}
