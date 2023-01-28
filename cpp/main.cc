#include "magenta.hh"

template <typename arr_t>
void print_state(arr_t const& s) {
	for (size_t i = 0; i < s.size(); i++) {
		printf("%.2x ", s[i]);
	}
	puts("");
}

int main() {
	MAGENTA128 magenta;

	mkey_t key = {
		0x1f, 0x17, 0xf7, 0xe2,
		0x6b, 0x78, 0xd7, 0x85,
		0x24, 0xb6, 0xb6, 0xaf,
		0xde, 0x54, 0x07, 0xcf,
	};

	state_t plaintext = {
		0x03, 0x13, 0x23, 0x33,
		0x43, 0x53, 0x63, 0x73,
		0x83, 0x93, 0xa3, 0xb3,
		0xc3, 0xd3, 0xe3, 0xf3,
	};

	printf("PT:  ");
	print_state(plaintext);
	printf("KEY: ");
	print_state(key);

	state_t ciphertext = magenta.encrypt(plaintext, key);
	printf("CT:  ");
	print_state(ciphertext);

	state_t pt2 = magenta.decrypt(ciphertext, key);
	printf("PT2: ");
	print_state(pt2);
}
