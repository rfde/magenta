#include "magenta.hh"

std::array<uint8_t, 256> f {};

void MAGENTA_init(void) {
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

uint8_t A(uint8_t x, uint8_t y) {
	return f[x ^ f[y]];
}

uint16_t PE(uint8_t x, uint8_t y) {
	return (A(x, y) << 8) | A(y, x);
}

state_t pi(state_t const& x) {
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

state_t T(state_t const& w) {
	return pi(pi(pi(pi(w))));
}

state_t S(state_t const& x) {
	return {
		x[ 0], x[ 2], x[ 4], x[ 6],
		x[ 8], x[10], x[12], x[14],
		x[ 1], x[ 3], x[ 5], x[ 7],
		x[ 9], x[11], x[13], x[15],
	};
}

state_t C(size_t n, state_t const& w) {
	if (n == 1) {
		return T(w);
	}
	return T(sxor(w, S(C(n-1, w))));
}

half_state_t SK(mkey_t const& key, size_t n) {
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

half_state_t F(half_state_t const& X2, half_state_t const& SKn) {
	state_t in {
		X2[0], X2[1], X2[2], X2[3],
		X2[4], X2[5], X2[6], X2[7],
		SKn[0], SKn[1], SKn[2], SKn[3],
		SKn[4], SKn[5], SKn[6], SKn[7],
	};
	state_t out = S(C(3, in));
	return {
		out[0], out[1], out[2], out[3],
		out[4], out[5], out[6], out[7],
	};
}

state_t mround(size_t n, state_t const& X, half_state_t const& SKn) {
	half_state_t X1 {
		X[0], X[1], X[2], X[3],
		X[4], X[5], X[6], X[7],
	};
	half_state_t X2 {
		X[ 8], X[ 9], X[10], X[11],
		X[12], X[13], X[14], X[15],
	};

	half_state_t tmp = sxor(X1, F(X2, SKn));
	
	return {
		X2[0], X2[1], X2[2], X2[3],
		X2[4], X2[5], X2[6], X2[7],
		tmp[0], tmp[1], tmp[2], tmp[3],
		tmp[4], tmp[5], tmp[6], tmp[7],
	};
}

state_t swap_halves(state_t const& x) {
	return {
		x[ 8], x[ 9], x[10], x[11],
		x[12], x[13], x[14], x[15],
		x[ 0], x[ 1], x[ 2], x[ 3],
		x[ 4], x[ 5], x[ 6], x[ 7],
	};
}

void check_init(void) {
	if(f[0] == 0) {
		printf("ERROR: Run MAGENTA_init first.\n");
		exit(-1);
	}
}

state_t MAGENTA_encrypt(state_t const& x, mkey_t const& key) {
	check_init();
	state_t state = x;
	for (size_t i = 1; i <= 6; i++) {
		state = mround(i, state, SK(key, i));
	}
	return state;
}

state_t MAGENTA_decrypt(state_t const& x, mkey_t const& key) {
	check_init();
	state_t state = swap_halves(x);
	for (size_t i = 1; i <= 6; i++) {
		state = mround(i, state, SK(key, i));
	}
	return swap_halves(state);
}