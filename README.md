# RKHC (Randomized Key Hill Cipher)
The Hill Cipher is a classical example of a cryptosystem with interesting properties, namely that it implements the diffusion and confusion concepts coined by Shannon as essential properties for ciphers; nonetheless, its basic form is vulnerable to KPAs. This dissertation presents an efficient method to generate nonsingular key matrices, based on the Gauss-Jordan elimination procedure, which provides means to generate a new matrix per each block submitted to encryption. RKHC, described along this dissertation, uses that method and adds a step to both the encryption and decryption algorithms to deal with messages containing patterns (e.g., a sequence of zeros), in order to increase their strength against KPAs, CPAs and CCAs. A performance evaluation of a non-optimized implementation in the C programming language of RKHC is also included, compared with those of optimized implementations of AES and Salsa20, along with a discussion of its security and limitations under the well-known cryptanalysis attacks. The claim that the proposed method embeds randomness with high entropy into the generated matrices and ciphertext is corroborated by results of the TestU01 library for stringent randomness statistical tests.

## Setup
Compile using gcc or cc
- `gcc /Source/cipherMain32.c -o out`

### Run (command line arguments)
Run the generated output with parameters:
- `./out -enc -K mykey -iv myiv -in plaintex.ext -out ciphertext.rkhc`
- `./out -dec -K mykey -iv myiv -in ciphertext.rhill -out recovered.ext`

### Authors
- [Moisés Ferreira](http://di.ubi.pt)
- [Bernardo Sequeiros](http://di.ubi.pt/pessoas.php)
- [Mário Freire](https://www.di.ubi.pt/~mario/)
- [Aleksandra Mileva](https://scholar.google.com/citations?user=d2lHwg0AAAAJ&hl=en)
- [Pedro Inácio](https://www.di.ubi.pt/~inacio/)

