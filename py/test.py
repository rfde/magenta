import os
import ctypes
from functools import partial
from PIL import Image, ImageDraw, ImageFont

MAGENTA_lib_file = "../cpp/libmagenta.so"
MAGENTA_lib = ctypes.CDLL(MAGENTA_lib_file)
MAGENTA_state =  ctypes.c_ubyte * 16

if __name__ == '__main__':
	key = bytearray([
		0x1f, 0x17, 0xf7, 0xe2,
		0x6b, 0x78, 0xd7, 0x85,
		0x24, 0xb6, 0xb6, 0xaf,
		0xde, 0x54, 0x07, 0xcf,
	])

	FONTPATH = "./font/Inconsolata_Expanded-Regular.ttf"
	IMGPATH = "./foo.bmp"
	CRYPTPATH = f"{IMGPATH}.crypt"
	BLOCK_SIZE_BITS = 128
	BLOCK_SIZE_BYTES = int(BLOCK_SIZE_BITS / 8)
	print(FONTPATH)

	font = ImageFont.truetype(FONTPATH, size=240)

	img = Image.new(mode="L", size=(8400,300))
	imgdraw = ImageDraw.Draw(img)
	imgdraw.text(
		(0,0),
		"flag{0123456789abcdef0123456789abcdef0123456789abcdef}",
		font=font,
		fill=255,
	)
	img.save(IMGPATH)

	with open(IMGPATH, "rb") as imgfile:
		ciphertext = bytearray(16)
		with open(CRYPTPATH, "wb") as cryptfile:
			imgdata = bytearray(imgfile.read())
			last_block_overflow_bytes = len(imgdata) % BLOCK_SIZE_BYTES
			if last_block_overflow_bytes != 0:
				imgdata = imgdata + b'\xff' * (BLOCK_SIZE_BYTES - last_block_overflow_bytes)
			for i in range(int(len(imgdata) / BLOCK_SIZE_BYTES)):
				MAGENTA_lib.MAGENTA_enc(
					MAGENTA_state.from_buffer(imgdata, i * BLOCK_SIZE_BYTES),
					MAGENTA_state.from_buffer(key),
					MAGENTA_state.from_buffer(ciphertext)
				)
				cryptfile.write(ciphertext)

	for break_pos in range(525, 10000, 525):
		print(break_pos)
		no_blocks = int(os.path.getsize(CRYPTPATH) / BLOCK_SIZE_BYTES)
		with open(CRYPTPATH, "rb") as cryptfile:
			restoreimg = Image.new(mode="RGB", size=(break_pos, int(no_blocks/break_pos) + 1))
			pixels = restoreimg.load()
			for chunk_idx, chunk in enumerate(iter(partial(cryptfile.read, BLOCK_SIZE_BYTES), b'')):
				colorhash = sum(chunk)
				if colorhash == 1697:
					color = (0xff, 0xff, 0xff)
				else:
					color = (0, 0, 0)
				pixels[chunk_idx % break_pos, int(chunk_idx/break_pos)] = color # (colorhash & 0xff, (colorhash >> 8) & 0xff, (colorhash >> 16) & 0xff)
			restoreimg.save(f"breaks_{break_pos:05d}.png")

