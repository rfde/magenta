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
		out[2*i] = tmp[i] >> 8;
		out[2*i+1] = tmp[i] & 0xff;
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
	half_state_t out;
	if (n == 1 || n == 2 || n == 5 || n == 6) {
		std::copy(key.begin(), key.begin() + 8, out.begin());
	} else {
		std::copy(key.begin() + 8, key.end(), out.begin());
	}
	return out;
}

half_state_t F(half_state_t const& X2, half_state_t const& SKn) {
	state_t in;
	std::copy(X2.begin(), X2.end(), in.begin());
	std::copy(SKn.begin(), SKn.end(), in.begin() + 8);
	state_t out = S(C(3, in));
	return {
		out[0], out[1], out[2], out[3],
		out[4], out[5], out[6], out[7],
	};
}

state_t mround(size_t n, state_t const& X, half_state_t const& SKn) {
	half_state_t X1, X2;
	std::copy(X.begin(), X.begin() + 8, X1.begin());
	std::copy(X.begin() + 8, X.end(), X2.begin());

	half_state_t tmp = sxor(X1, F(X2, SKn));
	
	state_t out;
	std::copy(X2.begin(), X2.end(), out.begin());
	std::copy(tmp.begin(), tmp.end(), out.begin() + 8);
	return out;
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