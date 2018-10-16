# RKHC (Randomized Key Hill Cipher)
A secure variant of Hill CIpher with sa self nonsingular matrices generator.

#Setup
Use gcc or cc to compile /Source/cipherMain32.c
#gcc /Source/cipherMain32.c -o out

#Run (command line arguments):
-enc -K secret_key -iv inicialization_vector -nonce one_nonce -in plaintext.in -out ciphertext.out
-dec -K secret_key -iv inicialization_vector -nonce one_nonce -in ciphertext.in -outplaintext.out

#Authors:
Moisés Ferreira^1
Bernardo Sequeiros^1
Mário Freire^1
Aleksandra Mileva^2
Pedro Inácio^1

