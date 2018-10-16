# RKHC (Randomized Key Hill Cipher)
A secure variant of Hill Cipher with self nonsingular matrices generator.

## Setup
Compile using gcc or cc
- `gcc /Source/cipherMain32.c -o out`

### Run (command line arguments)
Run the generated output with parameters:
- `./out -enc -K secret_key -iv inicialization_vector -nonce one_nonce -in plaintext.in -out ciphertext.out`
- `./out -dec -K secret_key -iv inicialization_vector -nonce one_nonce -in ciphertext.in -out plaintext.out`

### Authors
- [Moisés Ferreira]http://di.ubi.pt
- [Bernardo Sequeiros]http://di.ubi.pt
- [Mário Freire]
- [Aleksandra Mileva]
- [Pedro Inácio]

