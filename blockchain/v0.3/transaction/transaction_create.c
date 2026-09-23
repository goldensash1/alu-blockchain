#include "transaction.h"

/**
 * select_inputs - Creates inputs from the unspent outputs of a public key
 *
 * @tx: Transaction to add the inputs to
 * @all_unspent: List of all unspent transaction outputs
 * @pub: Public key of the owner of the outputs to select
 * @amount: Amount to reach
 * @total: Address at which to store the total of the selected outputs
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int select_inputs(transaction_t *tx, llist_t *all_unspent,
	uint8_t const *pub, uint32_t amount, uint32_t *total)
{
	unspent_tx_out_t *unspent;
	tx_in_t *in;
	int i, n = llist_size(all_unspent);

	*total = 0;
	for (i = 0; i < n && *total < amount; i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (!unspent || memcmp(unspent->out.pub, pub, EC_PUB_LEN) != 0)
			continue;
		in = tx_in_create(unspent);
		if (!in || llist_add_node(tx->inputs, in, ADD_NODE_REAR) != 0)
		{
			free(in);
			return (-1);
		}
		*total += unspent->out.amount;
	}

	return (0);
}

/**
 * add_output - Creates a transaction output and adds it to a transaction
 *
 * @tx: Transaction to add the output to
 * @amount: Amount of the output
 * @pub: Public key of the receiver
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int add_output(transaction_t *tx, uint32_t amount, uint8_t const *pub)
{
	tx_out_t *out = tx_out_create(amount, pub);

	if (!out || llist_add_node(tx->outputs, out, ADD_NODE_REAR) != 0)
	{
		free(out);
		return (-1);
	}

	return (0);
}

/**
 * sign_inputs - Signs all the inputs of a transaction
 *
 * @tx: Transaction whose inputs to sign
 * @sender: Private key of the sender
 * @all_unspent: List of all unspent transaction outputs
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int sign_inputs(transaction_t *tx, EC_KEY const *sender,
	llist_t *all_unspent)
{
	tx_in_t *in;
	int i, n = llist_size(tx->inputs);

	for (i = 0; i < n; i++)
	{
		in = llist_get_node_at(tx->inputs, i);
		if (!tx_in_sign(in, tx->id, sender, all_unspent))
			return (-1);
	}

	return (0);
}

/**
 * discard - Frees a transaction being built, along with its inputs and outputs
 *
 * @tx: Transaction to free
 */
static void discard(transaction_t *tx)
{
	if (tx->inputs)
		llist_destroy(tx->inputs, 1, free);
	if (tx->outputs)
		llist_destroy(tx->outputs, 1, free);
	free(tx);
}

/**
 * transaction_create - Creates a transaction
 *
 * @sender: Private key of the transaction sender
 * @receiver: Public key of the transaction receiver
 * @amount: Amount to send
 * @all_unspent: List of all the unspent outputs to date
 *
 * Return: Pointer to the created transaction, or NULL upon failure
 */
transaction_t *transaction_create(EC_KEY const *sender,
	EC_KEY const *receiver, uint32_t amount, llist_t *all_unspent)
{
	transaction_t *tx;
	uint8_t spub[EC_PUB_LEN], rpub[EC_PUB_LEN];
	uint32_t total;

	if (!sender || !receiver || !amount || !all_unspent ||
		!ec_to_pub(sender, spub) || !ec_to_pub(receiver, rpub))
		return (NULL);

	tx = calloc(1, sizeof(*tx));
	if (!tx)
		return (NULL);
	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);

	if (!tx->inputs || !tx->outputs ||
		select_inputs(tx, all_unspent, spub, amount, &total) != 0 ||
		total < amount || add_output(tx, amount, rpub) != 0 ||
		(total > amount && add_output(tx, total - amount, spub) != 0) ||
		!transaction_hash(tx, tx->id) ||
		sign_inputs(tx, sender, all_unspent) != 0)
	{
		discard(tx);
		return (NULL);
	}

	return (tx);
}
