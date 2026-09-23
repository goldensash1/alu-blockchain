#include <stdio.h>
#include "../blockchain.h"

/**
 * print_hex - Prints a buffer as hexadecimal
 *
 * @buf: Buffer to print
 * @len: Number of bytes to print
 */
static void print_hex(uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		printf("%02x", buf[i]);
}

/**
 * _blockchain_print - Prints a Blockchain in full
 *
 * @blockchain: Pointer to the Blockchain to print
 */
void _blockchain_print(blockchain_t const *blockchain)
{
	int i, n = llist_size(blockchain->chain);
	block_t *b;

	printf("Blockchain: {\n\tchain [%d]: [\n", n);
	for (i = 0; i < n; i++)
	{
		b = llist_get_node_at(blockchain->chain, i);
		printf("\t\tBlock: {\n\t\t\tinfo: {\n");
		printf("\t\t\t\tindex: %u,\n\t\t\t\tdifficulty: %u,\n",
			b->info.index, b->info.difficulty);
		printf("\t\t\t\ttimestamp: %lu,\n\t\t\t\tnonce: %lu,\n",
			(unsigned long)b->info.timestamp, (unsigned long)b->info.nonce);
		printf("\t\t\t\tprev_hash: ");
		print_hex(b->info.prev_hash, SHA256_DIGEST_LENGTH);
		printf("\n\t\t\t},\n\t\t\tdata: {\n\t\t\t\tbuffer: \"%.*s\",\n",
			(int)b->data.len, (char *)b->data.buffer);
		printf("\t\t\t\tlen: %u\n\t\t\t},\n\t\t\thash: ", b->data.len);
		print_hex(b->hash, SHA256_DIGEST_LENGTH);
		printf("\n\t\t}\n");
	}
	printf("\t]\n}\n");
}

/**
 * _blockchain_print_brief - Prints a Blockchain in a compact form
 *
 * @blockchain: Pointer to the Blockchain to print
 */
void _blockchain_print_brief(blockchain_t const *blockchain)
{
	int i, n = llist_size(blockchain->chain);
	block_t *b;

	printf("Blockchain: {\n\tchain [%d]: [\n", n);
	for (i = 0; i < n; i++)
	{
		b = llist_get_node_at(blockchain->chain, i);
		printf("\t\tBlock: {\n\t\t\tinfo: { %u, %u, %lu, %lu, ",
			b->info.index, b->info.difficulty,
			(unsigned long)b->info.timestamp, (unsigned long)b->info.nonce);
		print_hex(b->info.prev_hash, SHA256_DIGEST_LENGTH);
		printf(" },\n\t\t\tdata: { \"%.*s\", %u },\n\t\t\thash: ",
			(int)b->data.len, (char *)b->data.buffer, b->data.len);
		print_hex(b->hash, SHA256_DIGEST_LENGTH);
		printf("\n\t\t}\n");
	}
	printf("\t]\n}\n");
}
