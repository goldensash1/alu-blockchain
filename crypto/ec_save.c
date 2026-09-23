#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <openssl/pem.h>
#include "hblk_crypto.h"

#define PATH_MAX_LEN 4096

/**
 * ec_save - Saves an existing EC key pair on the disk
 *
 * @key: Pointer to the EC key pair to be saved on disk
 * @folder: Path to the folder in which to save the keys
 *
 * Return: 1 upon success, or 0 upon failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	char path[PATH_MAX_LEN];
	FILE *file;

	if (!key || !folder)
		return (0);

	if (mkdir(folder, 0755) != 0 && errno != EEXIST)
		return (0);

	snprintf(path, sizeof(path), "%s/" PRI_FILENAME, folder);
	file = fopen(path, "w");
	if (!file)
		return (0);
	if (!PEM_write_ECPrivateKey(file, key, NULL, NULL, 0, NULL, NULL))
	{
		fclose(file);
		return (0);
	}
	fclose(file);

	snprintf(path, sizeof(path), "%s/" PUB_FILENAME, folder);
	file = fopen(path, "w");
	if (!file)
		return (0);
	if (!PEM_write_EC_PUBKEY(file, key))
	{
		fclose(file);
		return (0);
	}
	fclose(file);

	return (1);
}
