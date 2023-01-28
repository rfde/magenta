#pragma once

#include <cstdio>
#include <cstdlib>
#include <cinttypes>

#include <array>

using state_t = std::array<uint8_t, 16>;
using half_state_t = std::array<uint8_t, 8>;

using mkey_t = std::array<uint8_t, 16>;

class MAGENTA {
private:
	std::array<uint8_t, 256> f {};

	// === helper functions =============================================
	template <typename arr_t> arr_t sxor(arr_t const& a, arr_t const& b);
	state_t concat(half_state_t const& a, half_state_t const& b);
	state_t swap_halves(state_t const& x);

	// === CIPHER CIPHER ======== CYBER CYBER ========= CYPHER CYPHER ===
	uint8_t A(uint8_t x, uint8_t y);
	uint16_t PE(uint8_t x, uint8_t y);
	state_t pi(state_t const& x);
	state_t T(state_t const& w);
	state_t S(state_t const& x);
	state_t C(size_t n, state_t const& w);
	half_state_t SK(mkey_t const& key, size_t n);
	half_state_t F(half_state_t const& X2, half_state_t const& SKn);
	state_t rnd(size_t n, state_t const& X, half_state_t const& SKn);

public:
	MAGENTA(void);
	state_t encrypt(state_t const& x, mkey_t const& key);
	state_t decrypt(state_t const& x, mkey_t const& key);
};