from magenta import MAGENTA

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

print(f"PT:  {MAGENTA.state_to_str(pt)}")
print(f"KEY: {MAGENTA.state_to_str(key)}")
print(f"CT:  {MAGENTA.state_to_str(ct)}")
print(f"PT2: {MAGENTA.state_to_str(pt2)}")