#include <stdio.h>
#include "blockchain.h"

void _print_hex_buffer(uint8_t const *buf, size_t len);

/**
 * _transaction_print - Prints a transaction in full
 *
 * @transaction: Pointer to the transaction to print
 */
void _transaction_print(transaction_t const *transaction)
{
	int i, n;
	tx_in_t *in;
	tx_out_t *out;

	if (!transaction)
		return;
	n = llist_size(transaction->inputs);
	printf("Transaction: {\n\tinputs [%d]: [\n", n);
	for (i = 0; i < n; i++)
	{
		in = llist_get_node_at(transaction->inputs, i);
		printf("\t\t{\n\t\t\tblock_hash: ");
		_print_hex_buffer(in->block_hash, sizeof(in->block_hash));
		printf(",\n\t\t\ttx_id: ");
		_print_hex_buffer(in->tx_id, sizeof(in->tx_id));
		printf(",\n\t\t\ttx_out_hash: ");
		_print_hex_buffer(in->tx_out_hash, sizeof(in->tx_out_hash));
		printf(",\n\t\t\tsig: ");
		if (in->sig.len)
			_print_hex_buffer(in->sig.sig, in->sig.len);
		else
			printf("null");
		printf("\n\t\t}\n");
	}
	n = llist_size(transaction->outputs);
	printf("\t],\n\toutputs [%d]: [\n", n);
	for (i = 0; i < n; i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		printf("\t\t{\n\t\t\tamount: %u,\n\t\t\tpub: ", out->amount);
		_print_hex_buffer(out->pub, sizeof(out->pub));
		printf(",\n\t\t\thash: ");
		_print_hex_buffer(out->hash, sizeof(out->hash));
		printf("\n\t\t}\n");
	}
	printf("\t],\n\tid: ");
	_print_hex_buffer(transaction->id, sizeof(transaction->id));
	printf("\n}\n");
}
