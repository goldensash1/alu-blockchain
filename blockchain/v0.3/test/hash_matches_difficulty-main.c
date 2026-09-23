#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "blockchain.h"

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	uint32_t difficulty;

	memset(hash, 0, sizeof(hash));
	hash[1] = 0x10;

	for (difficulty = 8; difficulty <= 13; difficulty++)
		printf("difficulty %u: %d\n", difficulty,
			hash_matches_difficulty(hash, difficulty));

	return (EXIT_SUCCESS);
}
