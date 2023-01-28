#include "magenta.hh"

// === helper functions =============================================

template <typename arr_t>
arr_t MAGENTA128::sxor(arr_t const& a, arr_t const& b) {
	arr_t out;
	for (size_t i = 0; i < a.size(); i++) {
		out[i] = a[i] ^ b[i];
	}
	return out;
}

state_t MAGENTA128::concat(half_state_t const& a, half_state_t const& b) {
	return {
		a[0], a[1], a[2], a[3],
		a[4], a[5], a[6], a[7],
		b[0], b[1], b[2], b[3],
		b[4], b[5], b[6], b[7],
	};
}

state_t MAGENTA128::swap_halves(state_t const& x) {
	return {
		x[ 8], x[ 9], x[10], x[11],
		x[12], x[13], x[14], x[15],
		x[ 0], x[ 1], x[ 2], x[ 3],
		x[ 4], x[ 5], x[ 6], x[ 7],
	};
}

// === CIPHER CIPHER ======== CYBER CYBER ========= CYPHER CYPHER ===

MAGENTA128::MAGENTA128(void) {
	f[0] = 1;
	uint16_t current = f[0];
	for (size_t i = 1; i < 255; i++) {
		current <<= 1;
		if (current & 0x100) {
			current ^= 101;
		}
		f[i] = current;
	}
	f[255] = 0;
}

uint8_t MAGENTA128::A(uint8_t x, uint8_t y) {
	return f[x ^ f[y]];
}

uint16_t MAGENTA128::PE(uint8_t x, uint8_t y) {
	return (A(x, y) << 8) | A(y, x);
}

state_t MAGENTA128::pi(state_t const& x) {
	std::array<uint16_t, 8> tmp;
	for (size_t i = 0; i < 8; i++) {
		tmp[i] = PE(x[i], x[8+i]);
	}
	state_t out;
	for (size_t i = 0; i < 8; i += 1) {
		out[2 * i] = tmp[i] >> 8;
		out[2 * i + 1] = tmp[i] & 0xff;
	}
	return out;
}

state_t MAGENTA128::T(state_t const& w) {
	return pi(pi(pi(pi(w))));
}

state_t MAGENTA128::S(state_t const& x) {
	return {
		x[ 0], x[ 2], x[ 4], x[ 6],
		x[ 8], x[10], x[12], x[14],
		x[ 1], x[ 3], x[ 5], x[ 7],
		x[ 9], x[11], x[13], x[15],
	};
}

state_t MAGENTA128::C(size_t n, state_t const& w) {
	if (n == 1) {
		return T(w);
	}
	return T(sxor(w, S(C(n-1, w))));
}

half_state_t MAGENTA128::SK(mkey_t const& key, size_t n) {
	if (n == 3 || n == 4) {
		return {
			key[ 8], key[ 9], key[10], key[11],
			key[12], key[13], key[14], key[15],
		};
	}
	return {
		key[0], key[1], key[2], key[3],
		key[4], key[5], key[6], key[7],
	};
}

half_state_t MAGENTA128::F(half_state_t const& X2, half_state_t const& SKn) {
	state_t in = concat(X2, SKn);
	state_t out = S(C(3, in));
	return {
		out[0], out[1], out[2], out[3],
		out[4], out[5], out[6], out[7],
	};
}

state_t MAGENTA128::rnd(size_t n, state_t const& X, half_state_t const& SKn) {
	half_state_t X1 {
		X[0], X[1], X[2], X[3],
		X[4], X[5], X[6], X[7],
	};
	half_state_t X2 {
		X[ 8], X[ 9], X[10], X[11],
		X[12], X[13], X[14], X[15],
	};

	half_state_t tmp = sxor(X1, F(X2, SKn));
	
	return concat(X2, tmp);
}

state_t MAGENTA128::encrypt(state_t const& x, mkey_t const& key) {
	state_t state = x;
	for (size_t i = 1; i <= 6; i++) {
		state = rnd(i, state, SK(key, i));
	}
	return state;
}

state_t MAGENTA128::decrypt(state_t const& x, mkey_t const& key) {
	state_t state = swap_halves(x);
	for (size_t i = 1; i <= 6; i++) {
		state = rnd(i, state, SK(key, i));
	}
	return swap_halves(state);
}