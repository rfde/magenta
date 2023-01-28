class MAGENTA:
	def __init__(self):
		self.f = [0 for i in range(256)]
		self.f[0] = 1
		current = self.f[0]
		for i in range(1, 255):
			current <<= 1
			if current & 0x100 != 0:
				current ^= 101
			self.f[i] = current & 0xff

	# === helper functions =============================================

	def _concat(self, half_a, half_b):
		return [
			half_a[0], half_a[1], half_a[2], half_a[3],
			half_a[4], half_a[5], half_a[6], half_a[7],
			half_b[0], half_b[1], half_b[2], half_b[3],
			half_b[4], half_b[5], half_b[6], half_b[7],
		]

	def _swap_halves(self, x):
		return [
			x[ 8], x[ 9], x[10], x[11],
			x[12], x[13], x[14], x[15],
			x[ 0], x[ 1], x[ 2], x[ 3],
			x[ 4], x[ 5], x[ 6], x[ 7],
		]

	def _sxor(self, a, b):
		assert (len(a) == len(b))
		return [a[i] ^ b[i] for i in range(len(a))]

	# === CIPHER CIPHER ======== CYBER CYBER ========= CYPHER CYPHER ===

	def _A(self, x, y):
		return self.f[x ^ self.f[y]]

	def _PE(self, x, y):
		return (self._A(x, y) << 8) | self._A(y, x)

	def _pi(self, x):
		tmp = [self._PE(x[i], x[8 + i]) for i in range(8)]
		out = [0 for i in range(16)]
		for i in range(8):
			out[2 * i] = tmp[i] >> 8
			out[2 * i + 1] = tmp[i] & 0xff
		return out

	def _T(self, w):
		return self._pi(self._pi(self._pi(self._pi(w))))

	def _S(self, x):
		return [
			x[ 0], x[ 2], x[ 4], x[ 6],
			x[ 8], x[10], x[12], x[14],
			x[ 1], x[ 3], x[ 5], x[ 7],
			x[ 9], x[11], x[13], x[15],
		]

	def _C(self, n, w):
		if (n == 1):
			return self._T(w)
		return self._T(self._sxor(w, self._S(self._C(n - 1, w))))

	def _SK(self, key, n):
		if n == 3 or n == 4:
			return [
				key[ 8], key[ 9], key[10], key[11],
				key[12], key[13], key[14], key[15],
			]
		else:
			return [
				key[0], key[1], key[2], key[3],
				key[4], key[5], key[6], key[7],
			]

	def _F(self, X2, SKn):
		inp = self._concat(X2, SKn)
		out = self._S(self._C(3, inp))
		return [
			out[0], out[1], out[2], out[3],
			out[4], out[5], out[6], out[7],
		]

	def _rnd(self, n, X, SKn):
		X1 = [
			X[0], X[1], X[2], X[3],
			X[4], X[5], X[6], X[7],
		]
		X2 = [
			X[ 8], X[ 9], X[10], X[11],
			X[12], X[13], X[14], X[15],
		]

		tmp = self._sxor(X1, self._F(X2, SKn))

		return self._concat(X2, tmp)

	def encrypt(self, x, key):
		state = x[:]
		for i in range(1, 7):
			state = self._rnd(i, state, self._SK(key, i))
		return state

	def decrypt(self, x, key):
		state = self._swap_halves(x)
		for i in range(1, 7):
			state = self._rnd(i, state, self._SK(key, i))
		return self._swap_halves(state)


def state_to_str(x):
	out = ""
	for e in x:
		out += f"{e:02x} "
	return out

magenta = MAGENTA()
key = [
	0x1f, 0x17, 0xf7, 0xe2,
	0x6b, 0x78, 0xd7, 0x85,
	0x24, 0xb6, 0xb6, 0xaf,
	0xde, 0x54, 0x07, 0xcf,
]

pt = [
	0x03, 0x13, 0x23, 0x33,
	0x43, 0x53, 0x63, 0x73,
	0x83, 0x93, 0xa3, 0xb3,
	0xc3, 0xd3, 0xe3, 0xf3,
]

ct = magenta.encrypt(pt, key)
pt2 = magenta.decrypt(ct, key)

print(f"PT:  {state_to_str(pt)}")
print(f"KEY: {state_to_str(key)}")
print(f"CT:  {state_to_str(ct)}")
print(f"PT2: {state_to_str(pt2)}")
