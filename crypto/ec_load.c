#include <stdio.h>
#include <openssl/pem.h>
#include "hblk_crypto.h"

#define PATH_MAX_LEN 4096

/**
 * ec_load - Loads an EC key pair from the disk
 *
 * @folder: Path to the folder from which to load the keys
 *
 * Return: Pointer to the created EC key pair upon success,
 * or NULL upon failure
 */
EC_KEY *ec_load(char const *folder)
{
	char path[PATH_MAX_LEN];
	FILE *file;
	EC_KEY *key = NULL, *pub = NULL;

	if (!folder)
		return (NULL);

	snprintf(path, sizeof(path), "%s/" PRI_FILENAME, folder);
	file = fopen(path, "r");
	if (!file)
		return (NULL);
	key = PEM_read_ECPrivateKey(file, NULL, NULL, NULL);
	fclose(file);
	if (!key)
		return (NULL);

	snprintf(path, sizeof(path), "%s/" PUB_FILENAME, folder);
	file = fopen(path, "r");
	if (!file)
	{
		EC_KEY_free(key);
		return (NULL);
	}
	pub = PEM_read_EC_PUBKEY(file, NULL, NULL, NULL);
	fclose(file);
	if (!pub || !EC_KEY_set_public_key(key, EC_KEY_get0_public_key(pub)))
	{
		EC_KEY_free(pub);
		EC_KEY_free(key);
		return (NULL);
	}
	EC_KEY_free(pub);

	return (key);
}
