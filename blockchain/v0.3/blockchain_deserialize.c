#include "blockchain.h"

/**
 * rd - Reads a value, reversing its bytes if needed
 *
 * @file: Stream to read from
 * @p: Address at which to store the value
 * @n: Size of the value, in bytes
 * @sw: Non-zero if the bytes of the value must be reversed
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int rd(FILE *file, void *p, size_t n, int sw)
{
	uint8_t *b = p, tmp;
	size_t i;

	if (n && fread(p, n, 1, file) != 1)
		return (-1);

	for (i = 0; sw && i < n / 2; i++)
	{
		tmp = b[i];
		b[i] = b[n - 1 - i];
		b[n - 1 - i] = tmp;
	}

	return (0);
}

/**
 * read_list - Reads nodes of a fixed size into a list
 *
 * @file: Stream to read from
 * @list: List to fill
 * @n: Number of nodes to read
 * @size: In-memory size of a node
 * @bytes: Number of bytes a node is serialized on
 * @off: Offset of the 4-byte amount of a node, or -1 if it has none
 * @sw: Non-zero if the multi-byte fields must be reversed
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int read_list(FILE *file, llist_t *list, uint32_t n,
	size_t size, size_t bytes, long off, int sw)
{
	uint8_t *node;
	uint32_t i;
	int err;

	for (i = 0; i < n; i++)
	{
		node = calloc(1, size);
		if (!node)
			return (-1);
		if (off < 0)
			err = rd(file, node, bytes, 0);
		else
			err = rd(file, node, off, 0) ||
				rd(file, node + off, 4, sw) ||
				rd(file, node + off + 4, bytes - off - 4, 0);
		if (err || llist_add_node(list, node, ADD_NODE_REAR) != 0)
		{
			free(node);
			return (-1);
		}
	}

	return (0);
}

/**
 * read_tx - Reads a transaction
 *
 * @file: Stream to read from
 * @sw: Non-zero if the multi-byte fields must be reversed
 *
 * Return: Pointer to the allocated transaction, or NULL upon failure
 */
static transaction_t *read_tx(FILE *file, int sw)
{
	transaction_t *tx = calloc(1, sizeof(*tx));
	uint32_t n_in = 0, n_out = 0;

	if (!tx)
		return (NULL);

	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs || !tx->outputs ||
		rd(file, tx->id, sizeof(tx->id), 0) ||
		rd(file, &n_in, 4, sw) || rd(file, &n_out, 4, sw) ||
		read_list(file, tx->inputs, n_in, sizeof(tx_in_t),
			TX_IN_SIZE, -1, sw) ||
		read_list(file, tx->outputs, n_out, sizeof(tx_out_t),
			TX_OUT_SIZE, 0, sw))
	{
		transaction_destroy(tx);
		return (NULL);
	}

	return (tx);
}

/**
 * read_block - Reads a Block, along with its transactions
 *
 * @file: Stream to read from
 * @sw: Non-zero if the multi-byte fields must be reversed
 *
 * Return: Pointer to the allocated Block, or NULL upon failure
 */
static block_t *read_block(FILE *file, int sw)
{
	block_t *b = calloc(1, sizeof(*b));
	transaction_t *tx;
	int32_t n = 0, i;

	if (!b)
		return (NULL);

	if (rd(file, &b->info.index, 4, sw) ||
		rd(file, &b->info.difficulty, 4, sw) ||
		rd(file, &b->info.timestamp, 8, sw) ||
		rd(file, &b->info.nonce, 8, sw) ||
		rd(file, b->info.prev_hash, SHA256_DIGEST_LENGTH, 0) ||
		rd(file, &b->data.len, 4, sw) ||
		b->data.len > BLOCKCHAIN_DATA_MAX ||
		rd(file, b->data.buffer, b->data.len, 0) ||
		rd(file, b->hash, SHA256_DIGEST_LENGTH, 0) ||
		rd(file, &n, 4, sw) || n < -1)
	{
		block_destroy(b);
		return (NULL);
	}

	if (n >= 0)
		b->transactions = llist_create(MT_SUPPORT_FALSE);
	for (i = 0; i < n || (n >= 0 && !b->transactions); i++)
	{
		tx = read_tx(file, sw);
		if (!tx || !b->transactions ||
			llist_add_node(b->transactions, tx, ADD_NODE_REAR) != 0)
		{
			transaction_destroy(tx);
			block_destroy(b);
			return (NULL);
		}
	}

	return (b);
}

/**
 * blockchain_deserialize - Deserializes a Blockchain from a file
 *
 * @path: Path to the file to load the Blockchain from
 *
 * Return: Pointer to the deserialized Blockchain, or NULL upon failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *file = path ? fopen(path, "rb") : NULL;
	blockchain_t *bc = calloc(1, sizeof(*bc));
	block_t *block;
	uint32_t nb = 0, nu = 0, i;
	uint8_t hdr[8];
	uint16_t probe = 1;
	int sw = -1;

	if (bc)
	{
		bc->chain = llist_create(MT_SUPPORT_FALSE);
		bc->unspent = llist_create(MT_SUPPORT_FALSE);
	}
	if (file && bc && bc->chain && bc->unspent && !rd(file, hdr, 8, 0) &&
		!memcmp(hdr, HBLK_MAGIC HBLK_VERSION, 7) &&
		(hdr[7] == HBLK_LITTLE_ENDIAN || hdr[7] == HBLK_BIG_ENDIAN))
		sw = hdr[7] != 2 - (*(uint8_t *)&probe == 1);
	if (sw >= 0 && (rd(file, &nb, 4, sw) || rd(file, &nu, 4, sw)))
		sw = -1;
	for (i = 0; sw >= 0 && i < nb; i++)
	{
		block = read_block(file, sw);
		if (!block ||
			llist_add_node(bc->chain, block, ADD_NODE_REAR) != 0)
		{
			block_destroy(block);
			sw = -1;
		}
	}
	if (sw < 0 || read_list(file, bc->unspent, nu, sizeof(unspent_tx_out_t),
		UNSPENT_SIZE, 64, sw))
	{
		blockchain_destroy(bc);
		bc = NULL;
	}

	if (file)
		fclose(file);
	return (bc);
}
