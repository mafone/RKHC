# RKHC (Randomized Key Hill Cipher)
This repository contains an efficient method to generate nonsingular key matrices, based on the Gauss-Jordan elimina-tion procedure, which provides means to generate a new matrix per each block submitted toencryption. RKHC uses that method and adds an additional step to both the encryption and decryption algorithms to deal with messages containingpatterns (e.g., a sequence of zeros), in order to increase their strength against KPA, CPA and CCA.
The complete work related to this code may be read [here](https://drive.google.com/open?id=1qWo9lThliQI-RIe_64A_HQthqYN_wYwQ).

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

