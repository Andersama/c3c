#pragma once
#include <stdint.h>

struct encode_result {
	size_t written_chars;
	size_t actual_chars;
};

struct decode_result {
	size_t written_chars;
	size_t actual_chars;
};

struct encode_result base32encode(char* data, size_t outlen, const char* input, size_t len);

struct decode_result base32decode(char* data, size_t outlen, const char* input, size_t len);

struct encode_result base62_5_encode(char* dst, size_t dst_len, const char* input, size_t in_len, char* alphabet);

struct decode_result base62_5_decode(char* dst, size_t dst_len, const char* input, size_t in_len, char* decode_table);