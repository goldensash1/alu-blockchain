#include "cli.h"

/**
 * balance - Computes the amount of coins owned by the wallet
 *
 * @cli: State of the CLI
 *
 * Return: Total of the unspent outputs sent to the wallet
 */
static uint32_t balance(cli_t const *cli)
{
	unspent_tx_out_t *u;
	uint8_t pub[EC_PUB_LEN];
	uint32_t total = 0;
	int i, n = llist_size(cli->blockchain->unspent);

	if (!ec_to_pub(cli->wallet, pub))
		return (0);

	for (i = 0; i < n; i++)
	{
		u = llist_get_node_at(cli->blockchain->unspent, i);
		if (u && !memcmp(u->out.pub, pub, EC_PUB_LEN))
			total += u->out.amount;
	}

	return (total);
}

/**
 * cmd_info - Displays information about the Blockchain
 *
 * @cli: State of the CLI
 * @args: Unused
 *
 * Return: 0
 */
int cmd_info(cli_t *cli, char *args)
{
	(void)args;

	printf("Blocks in the Blockchain: %d\n",
		llist_size(cli->blockchain->chain));
	printf("Unspent transaction outputs: %d\n",
		llist_size(cli->blockchain->unspent));
	printf("Pending transactions in the pool: %d\n",
		llist_size(cli->pool));
	printf("Wallet balance: %u\n", balance(cli));
	print_address(cli);

	return (0);
}
