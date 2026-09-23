# Crypto

Cryptographic primitives used by the Blockchain project, built on top of
the OpenSSL `crypto` library: SHA256 hashing and secp256k1 EC key
generation, loading/saving, and ECDSA signing/verification.

## Files

| File | Description |
| --- | --- |
| `hblk_crypto.h` | Header file with all prototypes, macros (`EC_CURVE`, `EC_PUB_LEN`) and the `sig_t` type |
| `sha256.c` | `sha256` - hashes a sequence of bytes with SHA256 |
| `ec_create.c` | `ec_create` - creates a new EC key pair (secp256k1) |
| `ec_to_pub.c` | `ec_to_pub` - extracts the public key from an `EC_KEY` |
| `ec_from_pub.c` | `ec_from_pub` - builds an `EC_KEY` from a public key |
| `ec_save.c` | `ec_save` - saves an EC key pair to disk, in PEM format |
| `ec_load.c` | `ec_load` - loads an EC key pair from disk |
| `ec_sign.c` | `ec_sign` - signs a sequence of bytes with an EC private key |
| `ec_verify.c` | `ec_verify` - verifies an ECDSA signature with an EC public key |
| `Makefile` | Builds `libhblk_crypto.a`, the static library archiving all of the above |
| `provided/_print_hex_buffer.c` | Helper used by the test mains to print a buffer in hexadecimal |
| `test/` | One `-main.c` test driver per function |

## Building

```
make
```

produces `libhblk_crypto.a` in this directory.

## Testing a single function

Each source file also compiles independently, e.g.:

```
gcc -Wall -Werror -Wextra -pedantic -I. -o sha256-test \
    test/sha256-main.c provided/_print_hex_buffer.c sha256.c \
    -lssl -lcrypto
./sha256-test Holberton
```
