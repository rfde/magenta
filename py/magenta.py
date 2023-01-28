class MAGENTA:
	def __init__(self):
		self.__f = [0 for i in range(256)]
		self.__f[0] = 1
		current = self.__f[0]
		for i in range(1, 255):
			current <<= 1
			if current & 0x100 != 0:
				current ^= 101
			self.__f[i] = current & 0xff

	# === helper functions =============================================

	@staticmethod
	def state_to_str(x):
		out = ""
		for e in x:
			out += f"{e:02x} "
		return out

	def __swap_halves(self, x):
		return x[8:16] + x[0:8]

	def __sxor(self, a, b):
		assert (len(a) == len(b))
		return [a[i] ^ b[i] for i in range(len(a))]

	# === CIPHER CIPHER ======== CYBER CYBER ========= CYPHER CYPHER ===

	def __A(self, x, y):
		return self.__f[x ^ self.__f[y]]

	def __PE(self, x, y):
		return (self.__A(x, y) << 8) | self.__A(y, x)

	def __pi(self, x):
		tmp = [self.__PE(x[i], x[8 + i]) for i in range(8)]
		out = [0 for i in range(16)]
		for i in range(8):
			out[2 * i] = tmp[i] >> 8
			out[2 * i + 1] = tmp[i] & 0xff
		return out

	def __T(self, w):
		return self.__pi(self.__pi(self.__pi(self.__pi(w))))

	def __S(self, x):
		return [
			x[ 0], x[ 2], x[ 4], x[ 6],
			x[ 8], x[10], x[12], x[14],
			x[ 1], x[ 3], x[ 5], x[ 7],
			x[ 9], x[11], x[13], x[15],
		]

	def __C(self, n, w):
		if (n == 1):
			return self.__T(w)
		return self.__T(self.__sxor(w, self.__S(self.__C(n - 1, w))))

	def __SK(self, key, n):
		if n == 3 or n == 4:
			return key[8:16]
		else:
			return key[0:8]

	def __F(self, X2, SKn):
		inp = X2 + SKn
		return self.__S(self.__C(3, inp))[0:8]

	def __rnd(self, n, X, SKn):
		X1 = X[0:8]
		X2 = X[8:16]
		return X2 + self.__sxor(X1, self.__F(X2, SKn))

	def encrypt(self, x, key):
		state = x[:]
		for i in range(1, 7):
			state = self.__rnd(i, state, self.__SK(key, i))
		return state

	def decrypt(self, x, key):
		state = self.__swap_halves(x)
		for i in range(1, 7):
			state = self.__rnd(i, state, self.__SK(key, i))
		return self.__swap_halves(state)
