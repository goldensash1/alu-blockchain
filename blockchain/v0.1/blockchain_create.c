#include "blockchain.h"

/**
 * create_genesis - Creates the Genesis Block
 *
 * Return: Pointer to the allocated Genesis Block, or NULL upon failure
 */
static block_t *create_genesis(void)
{
	block_t *block;

	block = calloc(1, sizeof(*block));
	if (!block)
		return (NULL);

	block->info.timestamp = GENESIS_TIMESTAMP;
	memcpy(block->data.buffer, GENESIS_DATA, GENESIS_DATA_LEN);
	block->data.len = GENESIS_DATA_LEN;
	memcpy(block->hash, GENESIS_HASH, SHA256_DIGEST_LENGTH);

	return (block);
}

/**
 * blockchain_create - Creates a Blockchain structure, and initializes it
 *
 * Return: Pointer to the allocated Blockchain, or NULL upon failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *blockchain;
	block_t *genesis;

	blockchain = malloc(sizeof(*blockchain));
	if (!blockchain)
		return (NULL);

	blockchain->chain = llist_create(MT_SUPPORT_FALSE);
	genesis = create_genesis();
	if (!blockchain->chain || !genesis ||
		llist_add_node(blockchain->chain, genesis, ADD_NODE_REAR) != 0)
	{
		free(genesis);
		if (blockchain->chain)
			llist_destroy(blockchain->chain, 0, NULL);
		free(blockchain);
		return (NULL);
	}

	return (blockchain);
}
