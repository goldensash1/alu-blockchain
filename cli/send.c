#include "cli.h"

/**
 * is_pending - Checks whether an unspent output is already consumed by a
 * transaction of the pool
 *
 * @pool: Transaction pool
 * @u: Unspent output to look for
 *
 * Return: 1 if @u is consumed by a pending transaction, or 0 otherwise
 */
static int is_pending(llist_t *pool, unspent_tx_out_t const *u)
{
	transaction_t *tx;
	tx_in_t *in;
	size_t len = SHA256_DIGEST_LENGTH;
	int i, j, n = llist_size(pool);

	for (i = 0; i < n; i++)
	{
		tx = llist_get_node_at(pool, i);
		for (j = 0; tx && j < llist_size(tx->inputs); j++)
		{
			in = llist_get_node_at(tx->inputs, j);
			if (in && !memcmp(in, u, 2 * len) &&
				!memcmp(in->tx_out_hash, u->out.hash, len))
				return (1);
		}
	}

	return (0);
}

/**
 * create_tx - Creates a transaction, only using coins that are not already
 * committed to a pending transaction
 *
 * @cli: State of the CLI
 * @amount: Amount to send
 * @pub: Public key of the receiver
 *
 * Return: Pointer to the created transaction, or NULL upon failure
 */
static transaction_t *create_tx(cli_t *cli, uint32_t amount,
	uint8_t const *pub)
{
	llist_t *available = llist_create(MT_SUPPORT_FALSE);
	unspent_tx_out_t *u;
	transaction_t *tx = NULL;
	EC_KEY *receiver = ec_from_pub(pub);
	int i, n = llist_size(cli->blockchain->unspent);

	for (i = 0; available && receiver && i < n; i++)
	{
		u = llist_get_node_at(cli->blockchain->unspent, i);
		if (u && !is_pending(cli->pool, u))
			llist_add_node(available, u, ADD_NODE_REAR);
	}

	if (available && receiver)
		tx = transaction_create(cli->wallet, receiver, amount,
			available);

	if (available)
		llist_destroy(available, 0, NULL);
	EC_KEY_free(receiver);

	return (tx);
}

/**
 * cmd_send - Creates a transaction and adds it to the transaction pool
 *
 * @cli: State of the CLI
 * @args: Amount to send, then the public key of the receiver
 *
 * Return: 0 upon success, or -1 upon failure
 */
int cmd_send(cli_t *cli, char *args)
{
	char *amount_s = strtok(args, " \t"), *address = strtok(NULL, " \t");
	uint8_t pub[EC_PUB_LEN];
	uint32_t amount = 0;
	transaction_t *tx;

	if (!amount_s || !address || strtok(NULL, " \t"))
		return (fail("Usage: send <amount> <address>"));
	if (parse_amount(amount_s, &amount) != 0)
		return (fail("Error: invalid amount"));
	if (parse_hex(address, pub, EC_PUB_LEN) != 0)
		return (fail("Error: invalid address"));

	tx = create_tx(cli, amount, pub);
	if (!tx)
		return (fail("Error: unable to create the transaction"));
	if (!transaction_is_valid(tx, cli->blockchain->unspent) ||
		llist_add_node(cli->pool, tx, ADD_NODE_REAR) != 0)
	{
		transaction_destroy(tx);
		return (fail("Error: the transaction is not valid"));
	}

	printf("Transaction added to the pool: ");
	print_hex(tx->id, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
}
