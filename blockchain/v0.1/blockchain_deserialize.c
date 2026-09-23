#include "blockchain.h"

/**
 * get_endianness - Gets the endianness of the host
 *
 * Return: HBLK_LITTLE_ENDIAN or HBLK_BIG_ENDIAN
 */
static uint8_t get_endianness(void)
{
	uint16_t x = 1;

	if (*(uint8_t *)&x == 1)
		return (HBLK_LITTLE_ENDIAN);
	return (HBLK_BIG_ENDIAN);
}

/**
 * swap - Reverses the bytes of a value
 *
 * @ptr: Pointer to the value
 * @size: Size of the value, in bytes
 */
static void swap(void *ptr, size_t size)
{
	uint8_t *p = ptr, tmp;
	size_t i;

	for (i = 0; i < size / 2; i++)
	{
		tmp = p[i];
		p[i] = p[size - 1 - i];
		p[size - 1 - i] = tmp;
	}
}

/**
 * read_block - Reads a Block
 *
 * @file: Stream to read from
 * @sw: Non-zero if multi-byte fields must be swapped
 *
 * Return: Pointer to the allocated Block, or NULL upon failure
 */
static block_t *read_block(FILE *file, int sw)
{
	block_t *block = calloc(1, sizeof(*block));
	uint32_t len;

	if (!block)
		return (NULL);

	if (fread(&block->info, sizeof(block->info), 1, file) != 1 ||
		fread(&len, sizeof(len), 1, file) != 1)
	{
		free(block);
		return (NULL);
	}
	if (sw)
	{
		swap(&block->info.index, sizeof(block->info.index));
		swap(&block->info.difficulty, sizeof(block->info.difficulty));
		swap(&block->info.timestamp, sizeof(block->info.timestamp));
		swap(&block->info.nonce, sizeof(block->info.nonce));
		swap(&len, sizeof(len));
	}
	block->data.len = len;
	if (len > BLOCKCHAIN_DATA_MAX ||
		(len && fread(block->data.buffer, len, 1, file) != 1) ||
		fread(block->hash, SHA256_DIGEST_LENGTH, 1, file) != 1)
	{
		free(block);
		return (NULL);
	}

	return (block);
}

/**
 * read_header - Reads and checks the file header
 *
 * @file: Stream to read from
 * @count: Address at which to store the number of Blocks
 * @sw: Address at which to store whether fields must be swapped
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int read_header(FILE *file, uint32_t *count, int *sw)
{
	char magic[HBLK_MAGIC_LEN], version[HBLK_VERSION_LEN];
	uint8_t endian;

	if (fread(magic, HBLK_MAGIC_LEN, 1, file) != 1 ||
		fread(version, HBLK_VERSION_LEN, 1, file) != 1 ||
		fread(&endian, sizeof(endian), 1, file) != 1 ||
		fread(count, sizeof(*count), 1, file) != 1)
		return (-1);

	if (memcmp(magic, HBLK_MAGIC, HBLK_MAGIC_LEN) != 0 ||
		memcmp(version, HBLK_VERSION, HBLK_VERSION_LEN) != 0 ||
		(endian != HBLK_LITTLE_ENDIAN && endian != HBLK_BIG_ENDIAN))
		return (-1);

	*sw = endian != get_endianness();
	if (*sw)
		swap(count, sizeof(*count));

	return (0);
}

/**
 * read_chain - Reads all the Blocks of a file into a Blockchain
 *
 * @file: Stream to read from
 * @blockchain: Blockchain to fill
 * @count: Number of Blocks to read
 * @sw: Non-zero if multi-byte fields must be swapped
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int read_chain(FILE *file, blockchain_t *blockchain, uint32_t count,
	int sw)
{
	block_t *block;
	uint32_t i;

	for (i = 0; i < count; i++)
	{
		block = read_block(file, sw);
		if (!block)
			return (-1);
		if (llist_add_node(blockchain->chain, block, ADD_NODE_REAR) != 0)
		{
			free(block);
			return (-1);
		}
	}
	return (0);
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
	FILE *file;
	blockchain_t *blockchain;
	uint32_t count;
	int sw;

	if (!path)
		return (NULL);
	file = fopen(path, "rb");
	if (!file)
		return (NULL);

	blockchain = malloc(sizeof(*blockchain));
	if (!blockchain)
	{
		fclose(file);
		return (NULL);
	}
	blockchain->chain = llist_create(MT_SUPPORT_FALSE);
	if (!blockchain->chain || read_header(file, &count, &sw) != 0 ||
		read_chain(file, blockchain, count, sw) != 0)
	{
		fclose(file);
		blockchain_destroy(blockchain);
		return (NULL);
	}

	fclose(file);
	return (blockchain);
}
