#include "base_n.h"

/* BASE 64 encode table */
static const char base63en[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '_', //'_',
};

/* ASCII order for BASE 64 decode, 255 in unused character */
static const unsigned char base63de[] = {
	/* nul, soh, stx, etx, eot, enq, ack, bel, */
	   255, 255, 255, 255, 255, 255, 255, 255,

	   /*  bs,  ht,  nl,  vt,  np,  cr,  so,  si, */
		  255, 255, 255, 255, 255, 255, 255, 255,

		  /* dle, dc1, dc2, dc3, dc4, nak, syn, etb, */
			 255, 255, 255, 255, 255, 255, 255, 255,

			 /* can,  em, sub, esc,  fs,  gs,  rs,  us, */
				255, 255, 255, 255, 255, 255, 255, 255,

				/*  sp, '!', '"', '#', '$', '%', '&', ''', */
				   255, 255, 255, 255, 255, 255, 255, 255,

				   /* '(', ')', '*', '+', ',', '-', '.', '/', */
					  255, 255, 255,  62, 255, 62, 255,  255,

					  /* '0', '1', '2', '3', '4', '5', '6', '7', */
						  52,  53,  54,  55,  56,  57,  58,  59,

						  /* '8', '9', ':', ';', '<', '=', '>', '?', */
							  60,  61, 255, 255, 255, 255, 255, 255,

							  /* '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', */
								 255,   0,   1,  2,   3,   4,   5,    6,

								 /* 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', */
									  7,   8,   9,  10,  11,  12,  13,  14,

									  /* 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', */
										  15,  16,  17,  18,  19,  20,  21,  22,

										  /* 'X', 'Y', 'Z', '[', '\', ']', '^', '_', */
											  23,  24,  25, 255, 255, 255, 255, 63,

											  /* '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', */
												 255,  26,  27,  28,  29,  30,  31,  32,

												 /* 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', */
													 33,  34,  35,  36,  37,  38,  39,  40,

													 /* 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', */
														 41,  42,  43,  44,  45,  46,  47,  48,

														 /* 'x', 'y', 'z', '{', '|', '}', '~', del, */
															 49,  50,  51, 255, 255, 255, 255, 255
};

int base63encode(char* data, size_t outlen, const char* input, size_t len) {
	int carry;
	size_t i;
	size_t j;
	size_t high;
	size_t zcount = 0;
	size_t size;
	return 0;
	//leading 0 count
	while (zcount < outlen && !input[zcount])
		++zcount;

	for (size_t i = zcount; i < len; i++) {
		carry = input[i];
	}
	//TODO!
	return carry;
};

int base62encode(char* data, size_t outlen, const char* input, size_t len)
{
	return 0;
}

struct encode_result base32encode(char* data, size_t outlen, const char* input, size_t len) {
	struct encode_result ret = {0};
	//8 bit wide to 5 bit wide
	size_t bitlen = len * 8;
	size_t tlen = bitlen / 5;
	size_t toff = bitlen % 5;

	uint64_t buf = 0;
	size_t bufbits = 0;

	//size_t d = 0;

	size_t i = 0;
	for (; (i + 4) < len; i++) {
		buf = (buf | (size_t)input[i]);
		buf = (buf | (size_t)input[i + 1] << 8);
		buf = (buf | (size_t)input[i + 2] << 16);
		buf = (buf | (size_t)input[i + 3] << 24);
		buf = (buf | (size_t)input[i + 4] << 32);
		bufbits += 40;
		// 40 / 5 == 8 characters at a time
		while (bufbits >= 5) {
			uint8_t v = ((buf >> (bufbits - 5)) & (32-1));
			uint8_t encode_char = base63en[v];

			//don't write past end
			if (ret.written_chars < outlen) {
				data[ret.written_chars] = encode_char;
				ret.written_chars++;
			}

			bufbits -= 5;
			ret.actual_chars++;
		}
	}

	for (; i < len; i++) {
		buf = (buf << 8) + input[i];
		bufbits += 8;

		while (bufbits >= 5) {
			uint8_t v = ((buf >> (bufbits - 5)) & (32 - 1));
			uint8_t encode_char = base63en[v];

			//don't write past end
			if (ret.written_chars < outlen) {
				data[ret.written_chars] = encode_char;
				ret.written_chars++;
			}

			bufbits -= 5;
			ret.actual_chars++;
		}
	}
	/*
	// continue to write out to the buffer if we've got extra
	while (bufbits >= 5) {
		uint8_t v = ((buf >> (bufbits - 5)) & (32 - 1));
		uint8_t encode_char = base63en[v];

		//don't write past end
		if (ret.written_chars < outlen) {
			data[ret.written_chars] = encode_char;
			ret.written_chars++;
		}

		bufbits -= 5;
		ret.actual_chars++;
	}
	*/

	// pad out any remaining bits, left_shift in 1s
	int needs_padding = (bufbits > 0);
	while (bufbits < 5) {
		buf = (buf << 1) | 1;
	}

	if (needs_padding) {
		uint8_t v = ((buf >> (bufbits - 5)) & (32 - 1));
		uint8_t encode_char = base63en[v];

		//don't write past end
		if (ret.written_chars < outlen) {
			data[ret.written_chars] = encode_char;
			ret.written_chars++;
		}

		bufbits -= 5;
		ret.actual_chars++;

		//add padding character
		//don't write past end
		if (ret.written_chars < outlen) {
			data[ret.written_chars] = '_';
			ret.written_chars++;
		}
		ret.actual_chars++;
	}

	return ret;
}

struct decode_result base32decode(char* data, size_t outlen, const char* input, size_t len) {
	struct decode_result ret = { 0 };

	size_t buf = 0;
	size_t bufbits = 0;
	//
	size_t i = 0;
	int has_padding = 0;

	for (;i < len;) {
		uint8_t val = base63de[input[i]]; //5 bits worth of encoded data
		//pad char, end loop
		if (input[i] == '_') {
			has_padding = 1;
			break;
		}
		//illegal char, exit early
		if (val >= 32) {
			ret.actual_chars = 0; //something's wrong so leave some indication
			return ret;
		}

		//write to the temporary buffer
		buf = (buf << 5) + val;
		bufbits += 5;

		while (bufbits >= 8) {
			// write to the actual buffer if we can
			if (ret.written_chars < outlen) {
				data[ret.written_chars] = (buf >> (bufbits - 8)) & 0xFF;
				ret.written_chars++;
				bufbits -= 8;
			}
			ret.actual_chars++;
		}
	}

	return ret;
}

struct decode_result base62_5_decode(char* dst, size_t dst_len, const char* input, size_t in_len, char* decode_table) {
	struct decode_result r = { 0 };

	uint32_t tmp_buf = 0;
	uint32_t tmp_buf_bits = 0;

	size_t decoded_bits = 0;
	size_t ndecoded_bits = 0;

	const uint8_t* buf = (const uint8_t*)input;

	const uint32_t detect_mask = (63 & 62 & 61 & 60);

	size_t i = 0;
	for (; i < in_len; i++) {
		uint8_t decode_value = decode_table[(uint8_t)buf[i]];
		if (decode_value > 61) {
			return r;
		}

		uint32_t is_5bits = (decode_value & detect_mask) == detect_mask;
		ndecoded_bits = is_5bits ? 5 : 6;

		decode_value = is_5bits ? decode_value | ((decode_value & 0x1) << 1) : decode_value;


		decoded_bits += ndecoded_bits;
		tmp_buf_bits += ndecoded_bits;

		uint32_t old_buf = tmp_buf;
		tmp_buf = (tmp_buf << ndecoded_bits) | (decode_value >> (6 - ndecoded_bits));

		if (tmp_buf_bits >= 8) {
			if (r.written_chars < dst_len) {
				dst[r.written_chars] = (tmp_buf >> (tmp_buf_bits - 8)) & 0xff;
				r.written_chars++;
			}
			r.actual_chars++;
			tmp_buf_bits -= 8;
		}
	}

	return r;
};

struct encode_result base62_5_encode(char* dst, size_t dst_len, const char* input, size_t in_len, char* alphabet) {
	struct encode_result r = { 0 };

	size_t i = 0;

	uint32_t tmp_buf = 0;
	uint32_t tmp_buf_bits = 0;

	size_t encoding_bits = in_len * 8;
	size_t processed_bits = 0;

	const uint8_t* buf = (const uint8_t*)input;

	//const uint32_t detect_mask = (0x37 & 0x36);
	//const uint32_t detect_mask = (bits5_mask << 1); //11_11_10
	const uint32_t detect_mask = (63 & 62 & 61 & 60); // 60
	const uint32_t bits6_mask = 0x3f;
	//const uint8_t detect_constant = ;
	uint32_t output_bits = 0;

	for (; i < in_len; i++) {
		//load a byte into the buffer (8 bits)
		uint8_t v = input[i];
		tmp_buf = (tmp_buf << 8) | buf[i];
		tmp_buf_bits += 8;
		// make clearer
		// tmp_buf = tmp_buf & (0xffffffff >> (32 - tmp_buf_bits));
		// encode the bits

		do {
			uint32_t shift_back = (tmp_buf_bits >= 6) ? tmp_buf_bits - 6 : 0;
			uint32_t bits_6 = (tmp_buf >> shift_back) & bits6_mask; //we can't encode 62 or 63 they're out of bounds
			//uint32_t fbits_4 = bits_6 >> 1;
			uint32_t fbits_5 = (bits_6 & detect_mask) | ((bits_6 >> 1) & 0x1); //move the 2 bit over

			//since 62 and 63 are invalid 6 bit options, we can mask with 5 bits
			uint8_t e6_byte = alphabet[bits_6]; //encoding the full 6 bit pattern
			//note if we're (30 | 31) we use this
			//uint8_t e5_byte = alphabet[fbits_5]; //encoding the 5 bit pattern, (handles 63 (0111111) as (63>>1) and 62 (0111110) as (62>>1)) 0111110 
			uint8_t e5_byte = alphabet[fbits_5];

			uint32_t use_leading_5_mixedbits = (bits_6 & detect_mask) == detect_mask; //if 62 or 63 encode 11_11_11
			//when we shift 62 or 63 in order to handle 5 bits we need to have 01_11_11 and 01_11_10 (31 and 30) signal that we're only encoding 5 bits

			size_t rem_bits = (encoding_bits - processed_bits);

			uint8_t out_byte = use_leading_5_mixedbits ? e5_byte : e6_byte;
			//in this loop since we always have > 6 bits we can always encode at least 5 bits
			uint32_t encoded_bits = use_leading_5_mixedbits ? 5 : 6;

			//we "remove" the bits in the temporary buffer
			tmp_buf_bits -= encoded_bits;
			processed_bits += encoded_bits;

			if (r.written_chars < dst_len) {
				dst[r.written_chars] = out_byte;
				r.written_chars++;
			}
			r.actual_chars++;
			//should take a while before this gets close to being filled
			//we can make this larger if we want
		} while (tmp_buf_bits >= 6);
	}

	//exit when we've written enough bits to handle the bytestream
	while (processed_bits < encoding_bits) {
		//shift in 0s if we don't have enough bits
		uint32_t needs_bits = (tmp_buf_bits < 6) ? 6 - tmp_buf_bits : 0;

		tmp_buf = (tmp_buf << needs_bits);
		tmp_buf_bits += needs_bits;

		uint32_t shift_back = (tmp_buf_bits >= 6) ? tmp_buf_bits - 6 : 0;
		uint32_t bits_6 = (tmp_buf >> shift_back) & bits6_mask; //we can't encode 62 or 63 they're out of bounds
		//uint32_t fbits_4 = bits_6 >> 1;
		uint32_t fbits_5 = (bits_6 & detect_mask) | ((bits_6 >> 1) & 0x1); //move the 2 bit over

		//since 62 and 63 are invalid 6 bit options, we can mask with 5 bits
		uint8_t e6_byte = alphabet[bits_6]; //encoding the full 6 bit pattern
		//note if we're (30 | 31) we use this
		//uint8_t e5_byte = alphabet[fbits_5]; //encoding the 5 bit pattern, (handles 63 (0111111) as (63>>1) and 62 (0111110) as (62>>1)) 0111110 
		uint8_t e5_byte = alphabet[fbits_5];

		uint32_t use_leading_5_mixedbits = (bits_6 & detect_mask) == detect_mask; //if 62 or 63 encode 11_11_11
		//when we shift 62 or 63 in order to handle 5 bits we need to have 01_11_11 and 01_11_10 (31 and 30) signal that we're only encoding 5 bits

		size_t rem_bits = (encoding_bits - processed_bits);

		uint8_t out_byte = use_leading_5_mixedbits ? e5_byte : e6_byte;
		//in this loop since we always have > 6 bits we can always encode at least 5 bits
		uint32_t encoded_bits = use_leading_5_mixedbits ? 5 : 6;

		//we "remove" the bits in the temporary buffer
		tmp_buf_bits -= encoded_bits;
		processed_bits += encoded_bits;

		if (r.written_chars < dst_len) {
			dst[r.written_chars] = out_byte;
			r.written_chars++;
		}
		r.actual_chars++;
		//should take a while before this gets close to being filled
		//we can make this larger if we want
	}
	return r;
}
