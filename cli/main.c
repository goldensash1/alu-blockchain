#include "cli.h"

/**
 * struct command_s - A command of the CLI
 *
 * @name: Name of the command
 * @func: Function implementing the command
 * @usage: Usage of the command
 */
typedef struct command_s
{
	char const *name;
	int (*func)(cli_t *, char *);
	char const *usage;
} command_t;

static command_t const commands[] = {
	{"wallet_load", cmd_wallet_load, "wallet_load <path>"},
	{"wallet_save", cmd_wallet_save, "wallet_save <path>"},
	{"send", cmd_send, "send <amount> <address>"},
	{"mine", cmd_mine, "mine"},
	{"info", cmd_info, "info"},
	{"load", cmd_load, "load <path>"},
	{"save", cmd_save, "save <path>"},
	{NULL, NULL, NULL}
};

/**
 * dispatch - Interprets a line and runs the matching command
 *
 * @cli: State of the CLI
 * @line: Line typed by the user, modified in place
 *
 * Return: 1 if the CLI must exit, or 0 otherwise
 */
static int dispatch(cli_t *cli, char *line)
{
	char *name = line + strspn(line, " \t\r\n"), *args, *end;
	int i;

	args = name + strcspn(name, " \t\r\n");
	if (*args)
		*args++ = '\0';
	args += strspn(args, " \t");
	end = args + strlen(args);
	while (end > args && isspace((unsigned char)end[-1]))
		*--end = '\0';

	if (!*name)
		return (0);
	if (!strcmp(name, "exit") || !strcmp(name, "quit"))
		return (1);
	if (!strcmp(name, "help"))
	{
		printf("Commands:\n");
		for (i = 0; commands[i].name; i++)
			printf("  %s\n", commands[i].usage);
		return (0);
	}

	for (i = 0; commands[i].name; i++)
		if (!strcmp(name, commands[i].name))
		{
			commands[i].func(cli, args);
			return (0);
		}

	printf("Unknown command: %s (type 'help')\n", name);
	return (0);
}

/**
 * cleanup - Frees everything owned by the CLI
 *
 * @cli: State of the CLI
 */
static void cleanup(cli_t *cli)
{
	transaction_t *tx;

	while (cli->pool && (tx = llist_pop(cli->pool)))
		transaction_destroy(tx);
	if (cli->pool)
		llist_destroy(cli->pool, 0, NULL);

	blockchain_destroy(cli->blockchain);
	EC_KEY_free(cli->wallet);
}

/**
 * main - Entry point, a simple command interpreter
 *
 * @ac: Arguments count
 * @av: Arguments vector, optionally the folder to load the wallet from
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int ac, char **av)
{
	cli_t cli;
	char *line = NULL;
	size_t cap = 0;

	memset(&cli, 0, sizeof(cli));
	if (ac > 1)
		cli.wallet = ec_load(av[1]);
	printf(cli.wallet ? "Wallet loaded\n" : "New wallet created\n");
	if (!cli.wallet)
		cli.wallet = ec_create();
	cli.blockchain = blockchain_create();
	cli.pool = llist_create(MT_SUPPORT_FALSE);
	if (!cli.wallet || !cli.blockchain || !cli.pool)
	{
		cleanup(&cli);
		return (EXIT_FAILURE);
	}

	print_address(&cli);
	for (;;)
	{
		printf(PROMPT);
		fflush(stdout);
		if (getline(&line, &cap, stdin) == -1 || dispatch(&cli, line))
			break;
	}

	printf("\n");
	free(line);
	cleanup(&cli);
	return (EXIT_SUCCESS);
}
