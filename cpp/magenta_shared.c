#include <inttypes.h>
#include <stdio.h>
#include "libmagenta.h"

int main () {
	uint8_t key[16] = {
		0x1f, 0x17, 0xf7, 0xe2,
		0x6b, 0x78, 0xd7, 0x85,
		0x24, 0xb6, 0xb6, 0xaf,
		0xde, 0x54, 0x07, 0xcf,
	};

	uint8_t plaintext[16] = {
		0x03, 0x13, 0x23, 0x33,
		0x43, 0x53, 0x63, 0x73,
		0x83, 0x93, 0xa3, 0xb3,
		0xc3, 0xd3, 0xe3, 0xf3,
	};

	uint8_t ct[16];

	MAGENTA_enc(plaintext, key, ct);

	for (size_t i = 0; i <16; i++) {
		printf("%.2x ", ct[i]);
	}
	puts("");
	uint8_t pt2[16];
	MAGENTA_dec(ct, key, pt2);

	for (size_t i = 0; i <16; i++) {
		printf("%.2x ", pt2[i]);
	}
	puts("");
}
