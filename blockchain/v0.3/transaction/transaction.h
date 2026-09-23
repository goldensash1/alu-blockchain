#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <llist.h>

#include "hblk_crypto.h"

#define COINBASE_AMOUNT 50

#define TX_IN_SIZE 169
#define TX_OUT_SIZE 101
#define UNSPENT_SIZE 165

/**
 * struct tx_out_s - Transaction output
 *
 * @amount: Amount of the transaction
 * @pub: Public key of the receiver
 * @hash: Hash of @amount and @pub, used as an identifier
 */
typedef struct tx_out_s
{
	uint32_t amount;
	uint8_t pub[EC_PUB_LEN];
	uint8_t hash[SHA256_DIGEST_LENGTH];
} tx_out_t;

/**
 * struct unspent_tx_out_s - Unspent transaction output
 *
 * @block_hash: Hash of the Block containing the transaction @tx_id
 * @tx_id: ID of the transaction containing @out
 * @out: Copy of the referenced transaction output
 */
typedef struct unspent_tx_out_s
{
	uint8_t block_hash[SHA256_DIGEST_LENGTH];
	uint8_t tx_id[SHA256_DIGEST_LENGTH];
	tx_out_t out;
} unspent_tx_out_t;

/**
 * struct tx_in_s - Transaction input
 *
 * @block_hash: Hash of the Block containing the referenced output
 * @tx_id: Hash of the transaction containing the referenced output
 * @tx_out_hash: Hash of the referenced transaction output
 * @sig: Signature, proving the owner of the referenced output
 */
typedef struct tx_in_s
{
	uint8_t block_hash[SHA256_DIGEST_LENGTH];
	uint8_t tx_id[SHA256_DIGEST_LENGTH];
	uint8_t tx_out_hash[SHA256_DIGEST_LENGTH];
	sig_t sig;
} tx_in_t;

/**
 * struct transaction_s - Transaction structure
 *
 * @id: Transaction ID, a hash of the inputs and outputs
 * @inputs: List of tx_in_t
 * @outputs: List of tx_out_t
 */
typedef struct transaction_s
{
	uint8_t id[SHA256_DIGEST_LENGTH];
	llist_t *inputs;
	llist_t *outputs;
} transaction_t;

tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN]);
unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	uint8_t tx_id[SHA256_DIGEST_LENGTH], tx_out_t const *out);
tx_in_t *tx_in_create(unspent_tx_out_t const *unspent);
uint8_t *transaction_hash(transaction_t const *transaction,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH]);
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH],
	EC_KEY const *sender, llist_t *all_unspent);
transaction_t *transaction_create(EC_KEY const *sender,
	EC_KEY const *receiver, uint32_t amount, llist_t *all_unspent);
int transaction_is_valid(transaction_t const *transaction,
	llist_t *all_unspent);
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index);
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index);
void transaction_destroy(transaction_t *transaction);
llist_t *update_unspent(llist_t *transactions,
	uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *all_unspent);

#endif /* TRANSACTION_H */
