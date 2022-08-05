#pragma once
#ifndef BASE64_ENCODER_H
#define BASE64_ENCODER_H
unsigned int base64_decode(const char* in, unsigned int inlen, unsigned char* out);
unsigned int base64_encode(const unsigned char* in, unsigned int inlen, char* out);
#endif // !BASE64_ENCODER_H