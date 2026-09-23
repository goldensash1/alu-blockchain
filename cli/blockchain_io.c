#include "cli.h"

/**
 * cmd_load - Loads a Blockchain from a file, overriding the local one
 *
 * @cli: State of the CLI
 * @args: Path to the file to load the Blockchain from
 *
 * Return: 0 upon success, or -1 upon failure
 */
int cmd_load(cli_t *cli, char *args)
{
	blockchain_t *blockchain;

	if (!*args)
		return (fail("Usage: load <path>"));

	blockchain = blockchain_deserialize(args);
	if (!blockchain)
	{
		printf("Error: unable to load a Blockchain from '%s'\n", args);
		return (-1);
	}

	blockchain_destroy(cli->blockchain);
	cli->blockchain = blockchain;
	printf("Blockchain loaded from '%s' (%d Blocks)\n", args,
		llist_size(blockchain->chain));

	return (0);
}

/**
 * cmd_save - Saves the local Blockchain into a file
 *
 * @cli: State of the CLI
 * @args: Path to the file to save the Blockchain in
 *
 * Return: 0 upon success, or -1 upon failure
 */
int cmd_save(cli_t *cli, char *args)
{
	if (!*args)
		return (fail("Usage: save <path>"));

	if (blockchain_serialize(cli->blockchain, args) != 0)
	{
		printf("Error: unable to save the Blockchain in '%s'\n", args);
		return (-1);
	}

	printf("Blockchain saved in '%s'\n", args);
	return (0);
}
