#pragma once

#include <cstdio>
#include <cstdlib>
#include <cinttypes>

#include <array>

using state_t = std::array<uint8_t, 16>;
using half_state_t = std::array<uint8_t, 8>;

using mkey_t = std::array<uint8_t, 16>;

template <typename arr_t>
void print_state(arr_t const& s) {
	for (size_t i = 0; i < s.size(); i++) {
		printf("%.2x ", s[i]);
	}
	puts("");
}

template <typename arr_t>
arr_t sxor(arr_t const& a, arr_t const& b) {
	arr_t out;
	for (size_t i = 0; i < a.size(); i++) {
		out[i] = a[i] ^ b[i];
	}
	return out;
}

void MAGENTA_init(void);
state_t MAGENTA_encrypt(state_t const& x, mkey_t const& key);
state_t MAGENTA_decrypt(state_t const& x, mkey_t const& key);