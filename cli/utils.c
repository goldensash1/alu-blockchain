#include "cli.h"

/**
 * fail - Prints an error message
 *
 * @msg: Message to print
 *
 * Return: -1, so that callers can return it directly
 */
int fail(char const *msg)
{
	printf("%s\n", msg);
	return (-1);
}

/**
 * print_hex - Prints a buffer in its hexadecimal form
 *
 * @buf: Buffer to print
 * @len: Number of bytes to print
 */
void print_hex(uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		printf("%02x", buf[i]);
}

/**
 * parse_hex - Converts a hexadecimal string to bytes
 *
 * @str: String to convert, exactly 2 * @len characters long
 * @buf: Buffer in which to store the bytes
 * @len: Number of bytes expected
 *
 * Return: 0 upon success, or -1 if @str is not valid
 */
int parse_hex(char const *str, uint8_t *buf, size_t len)
{
	char pair[3] = {0, 0, 0};
	size_t i;

	if (strlen(str) != len * 2)
		return (-1);

	for (i = 0; i < len; i++)
	{
		if (!isxdigit((unsigned char)str[2 * i]) ||
			!isxdigit((unsigned char)str[2 * i + 1]))
			return (-1);
		pair[0] = str[2 * i];
		pair[1] = str[2 * i + 1];
		buf[i] = (uint8_t)strtoul(pair, NULL, 16);
	}

	return (0);
}

/**
 * parse_amount - Converts a string to a strictly positive 32-bit integer
 *
 * @str: String to convert
 * @amount: Address at which to store the result
 *
 * Return: 0 upon success, or -1 if @str is not a valid amount
 */
int parse_amount(char const *str, uint32_t *amount)
{
	char *end;
	unsigned long value;

	if (!isdigit((unsigned char)*str))
		return (-1);

	errno = 0;
	value = strtoul(str, &end, 10);
	if (errno || *end || value == 0 || value > UINT32_MAX)
		return (-1);

	*amount = (uint32_t)value;
	return (0);
}
