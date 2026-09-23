#include <stdio.h>
#include "blockchain.h"

void _print_hex_buffer(uint8_t const *buf, size_t len);
int _transaction_print_brief(transaction_t const *transaction,
	unsigned int idx, char const *indent);

/**
 * _blockchain_print_brief - Prints a Blockchain in a compact form
 *
 * @blockchain: Pointer to the Blockchain to print
 */
void _blockchain_print_brief(blockchain_t const *blockchain)
{
	int i, j, n = llist_size(blockchain->chain);
	block_t *b;

	printf("Blockchain: {\n\tchain [%d]: [\n", n);
	for (i = 0; i < n; i++)
	{
		b = llist_get_node_at(blockchain->chain, i);
		printf("\t\tBlock: {\n\t\t\tinfo: { %u, %u, %lu, %lu, ",
			b->info.index, b->info.difficulty,
			(unsigned long)b->info.timestamp, (unsigned long)b->info.nonce);
		_print_hex_buffer(b->info.prev_hash, SHA256_DIGEST_LENGTH);
		printf(" },\n\t\t\tdata: { \"%.*s\", %u },\n",
			(int)b->data.len, (char *)b->data.buffer, b->data.len);
		printf("\t\t\ttransactions [%d]: [\n", llist_size(b->transactions));
		for (j = 0; j < llist_size(b->transactions); j++)
			_transaction_print_brief(llist_get_node_at(b->transactions, j),
				j, "\t\t\t\t");
		printf("\t\t\t]\n\t\t\thash: ");
		_print_hex_buffer(b->hash, SHA256_DIGEST_LENGTH);
		printf("\n\t\t}\n");
	}
	printf("\t]\n}\n");
}
