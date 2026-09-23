#include <stdio.h>
#include "blockchain.h"

void _print_hex_buffer(uint8_t const *buf, size_t len);

/**
 * _transaction_print_brief - Prints a transaction in a compact form
 *
 * @transaction: Pointer to the transaction to print
 * @idx: Unused
 * @indent: Indentation
 *
 * Return: 0
 */
int _transaction_print_brief(transaction_t const *transaction,
	unsigned int idx, char const *indent)
{
	tx_out_t *out = llist_get_head(transaction->outputs);

	printf("%sTransaction: {\n", indent);
	printf("%s\tamount: %u from %d inputs,\n", indent,
		out ? out->amount : 0, llist_size(transaction->inputs));
	printf("%s\treceiver: ", indent);
	if (out)
		_print_hex_buffer(out->pub, sizeof(out->pub));
	printf("\n%s\tid: ", indent);
	_print_hex_buffer(transaction->id, sizeof(transaction->id));
	printf("\n%s}\n", indent);

	(void)idx;
	return (0);
}
