#include <openssl/ecdsa.h>
#include "hblk_crypto.h"

/**
 * ec_sign - Signs a given set of bytes, using a given EC_KEY private key
 *
 * @key: Pointer to the EC_KEY structure containing the private key
 * @msg: Pointer to the bytes to be signed
 * @msglen: Number of bytes to sign in @msg
 * @sig: Address at which to store the signature
 *
 * Return: Pointer to the signature buffer upon success, or NULL upon
 * failure
 */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen,
	sig_t *sig)
{
	unsigned int len;

	if (!key || !msg || !sig)
		return (NULL);

	if (ECDSA_sign(0, msg, msglen, sig->sig, &len, (EC_KEY *)key) != 1)
		return (NULL);

	sig->len = len;

	return (sig->sig);
}
