#include "cli.h"

/**
 * make_dirs - Creates the parent folders of a path, as needed
 *
 * @path: Path whose parent folders must exist
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int make_dirs(char const *path)
{
	char tmp[PATH_MAX_LEN];
	size_t i;

	if (strlen(path) >= sizeof(tmp))
		return (-1);
	strcpy(tmp, path);

	for (i = 1; tmp[i]; i++)
	{
		if (tmp[i] != '/')
			continue;
		tmp[i] = '\0';
		if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
			return (-1);
		tmp[i] = '/';
	}

	return (0);
}

/**
 * print_address - Prints the public key of the wallet, used as an address
 *
 * @cli: State of the CLI
 */
void print_address(cli_t const *cli)
{
	uint8_t pub[EC_PUB_LEN];

	if (!ec_to_pub(cli->wallet, pub))
		return;

	printf("Wallet address: ");
	print_hex(pub, EC_PUB_LEN);
	printf("\n");
}

/**
 * cmd_wallet_load - Loads the wallet (EC key pair) from a folder
 *
 * @cli: State of the CLI
 * @args: Path to the folder to load the key pair from
 *
 * Return: 0 upon success, or -1 upon failure
 */
int cmd_wallet_load(cli_t *cli, char *args)
{
	EC_KEY *key;

	if (!*args)
		return (fail("Usage: wallet_load <path>"));

	key = ec_load(args);
	if (!key)
	{
		printf("Error: unable to load a wallet from '%s'\n", args);
		return (-1);
	}

	EC_KEY_free(cli->wallet);
	cli->wallet = key;
	printf("Wallet loaded from '%s'\n", args);
	print_address(cli);

	return (0);
}

/**
 * cmd_wallet_save - Saves the wallet (EC key pair) into a folder
 *
 * @cli: State of the CLI
 * @args: Path to the folder to save the key pair in
 *
 * Return: 0 upon success, or -1 upon failure
 */
int cmd_wallet_save(cli_t *cli, char *args)
{
	if (!*args)
		return (fail("Usage: wallet_save <path>"));

	if (make_dirs(args) != 0 || !ec_save(cli->wallet, args))
	{
		printf("Error: unable to save the wallet in '%s'\n", args);
		return (-1);
	}

	printf("Wallet saved in '%s'\n", args);
	return (0);
}
