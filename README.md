# alu-blockchain

Low-level implementation of the cryptographic primitives that underpin a
Blockchain: SHA256 hashing and secp256k1 elliptic-curve key
generation/loading/saving/signing/verification, built on top of the
OpenSSL `crypto` library.

## Project: 0x01. Crypto

This project covers:

- How a Blockchain is considered "unbreakable"
- What a hash algorithm is, what SHA stands for, and how hashing applies
  to Blockchains
- What asymmetric cryptography is and how it applies to cryptocurrencies
- What ECC and ECDSA stand for
- What a digital signature is and how it applies to cryptocurrencies

### `crypto/`

| File | Description |
| --- | --- |
| `hblk_crypto.h` | Header file with all prototypes, macros and the `sig_t` type |
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

### Compilation

Each source file compiles independently with:

```
gcc -Wall -Werror -Wextra -pedantic -I. -o <test> test/<x>-main.c provided/_print_hex_buffer.c <x>.c [...] -lssl -lcrypto
```

The whole library is built with:

```
cd crypto && make
```

which produces `libhblk_crypto.a`.

## Author

Golden Munyankindi

## CLI

`cli/` contains a small command interpreter on top of the Blockchain library.

```
cd cli && make && ./cli [wallet_folder]
```

A wallet (EC key pair) is created at startup, or loaded from `wallet_folder`
when given. Commands:

| Command | Description |
| --- | --- |
| `wallet_load <path>` | Load the wallet from a folder |
| `wallet_save <path>` | Save the wallet (PEM) into a folder, creating it if needed |
| `send <amount> <address>` | Create a transaction and add it to the pool |
| `mine` | Mine a Block with a coinbase and the valid pooled transactions |
| `info` | Show Blocks, unspent outputs, pending transactions and balance |
| `load <path>` | Load a Blockchain from a file, replacing the local one |
| `save <path>` | Save the local Blockchain into a file |
| `help`, `exit` | Show the commands, or quit |

`<address>` is a public key in hexadecimal (130 characters), as printed by the
CLI at startup and by `info`.
