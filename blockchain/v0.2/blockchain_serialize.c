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
 * write_header - Writes the file header
 *
 * @file: Stream to write to
 * @count: Number of Blocks in the Blockchain
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int write_header(FILE *file, uint32_t count)
{
	uint8_t endian = get_endianness();

	if (fwrite(HBLK_MAGIC, HBLK_MAGIC_LEN, 1, file) != 1 ||
		fwrite(HBLK_VERSION, HBLK_VERSION_LEN, 1, file) != 1 ||
		fwrite(&endian, sizeof(endian), 1, file) != 1 ||
		fwrite(&count, sizeof(count), 1, file) != 1)
		return (-1);
	return (0);
}

/**
 * write_block - Writes a Block
 *
 * @file: Stream to write to
 * @block: Block to write
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int write_block(FILE *file, block_t const *block)
{
	uint32_t len = block->data.len;

	if (len > BLOCKCHAIN_DATA_MAX)
		return (-1);

	if (fwrite(&block->info, sizeof(block->info), 1, file) != 1 ||
		fwrite(&len, sizeof(len), 1, file) != 1 ||
		(len && fwrite(block->data.buffer, len, 1, file) != 1) ||
		fwrite(block->hash, SHA256_DIGEST_LENGTH, 1, file) != 1)
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
	int count, i, ret = 0;
	block_t *block;

	if (!blockchain || !blockchain->chain || !path)
		return (-1);

	file = fopen(path, "wb");
	if (!file)
		return (-1);

	count = llist_size(blockchain->chain);
	if (count < 0 || write_header(file, (uint32_t)count) != 0)
		ret = -1;

	for (i = 0; ret == 0 && i < count; i++)
	{
		block = llist_get_node_at(blockchain->chain, i);
		if (!block || write_block(file, block) != 0)
			ret = -1;
	}

	if (fclose(file) != 0)
		ret = -1;

	return (ret);
}
