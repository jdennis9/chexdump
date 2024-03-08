/*
	MIT License

	Copyright (c) 2024 Jamie Dennis
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#ifndef CHEXDUMP_H
#define CHEXDUMP_H

#include <stdio.h>

typedef enum Chex_Format {
	/*
	Single-line long hex string
	*/
	CHEX_FORMAT_LONG,

	/* 	
	C header declaration. input data can be NULL for this, but size must be provided.
	e.g. extern uint32_t MY_DATA[10000]; static const size_t MY_DATA_SIZE = ...
	*/
	CHEX_FORMAT_C_EXTERN_HEADER,
	/*
	C source definition for extern declaration
	*/
	CHEX_FORMAT_C_SOURCE,
	/*
	C static array
	*/
	CHEX_FORMAT_C_STATIC,
	
	/*
	Zig array
	*/
	CHEX_FORMAT_ZIG,
	
	CHEX_FORMAT__COUNT,
} Chex_Format;

/*
Create array with specified name containing
the names of formats
*/
#define CHEX_FORMAT__NAMES(var) \
const char *var[CHEX_FORMAT__COUNT];\
var[CHEX_FORMAT_LONG] = "long";\
var[CHEX_FORMAT_C_EXTERN_HEADER] = "c-extern";\
var[CHEX_FORMAT_C_SOURCE] = "c-source";\
var[CHEX_FORMAT_C_STATIC] = "c-static";\
var[CHEX_FORMAT_ZIG] = "zig";\

#define CHEX_FORMAT__DESCRIPTIONS(var) \
const char *var[CHEX_FORMAT__COUNT];\
var[CHEX_FORMAT_LONG] = "Long one-line string of hex characters";\
var[CHEX_FORMAT_C_EXTERN_HEADER] = "C header extern declaration";\
var[CHEX_FORMAT_C_SOURCE] = "C source definition";\
var[CHEX_FORMAT_C_STATIC] = "C static definition";\
var[CHEX_FORMAT_ZIG] = "Zig array";\

#define CHEX_FORMAT__HOOKS(var) \
Chex_Dump_Func *var[CHEX_FORMAT__COUNT];\
var[CHEX_FORMAT_LONG] = &chex_dump_long;\
var[CHEX_FORMAT_C_EXTERN_HEADER] = &chex_dump_c_extern;\
var[CHEX_FORMAT_C_SOURCE] = &chex_dump_c_source;\
var[CHEX_FORMAT_C_STATIC] = &chex_dump_c_static;\
var[CHEX_FORMAT_ZIG] = &chex_dump_zig;\

typedef struct Chex_Option {
	const char *key;
	const char *value;
} Chex_Option;

typedef struct Chex_Dump {
	const char *basename;
	// Last option is {NULL, NULL}
	const Chex_Option *options;
	// Input size needs to be divisible by word size
	int word_size;
} Chex_Dump;

typedef void Chex_Dump_Func(const Chex_Dump *info, const void *in, size_t in_size, FILE *out);

// Warning: If the input size is not divisible by the word size, the input buffer must be padded to fit
void chex_dump(const Chex_Dump *info, const void *in, size_t in_size, FILE *out, Chex_Format format);
void chex_dump_long(const Chex_Dump *info, const void *in, size_t in_size, FILE *out);
void chex_dump_c_extern(const Chex_Dump *info, const void *in, size_t in_size, FILE *out);
void chex_dump_c_source(const Chex_Dump *info, const void *in, size_t in_size, FILE *out);
void chex_dump_c_static(const Chex_Dump *info, const void *in, size_t in_size, FILE *out);
void chex_dump_zig(const Chex_Dump *info, const void *in, size_t in_size, FILE *out);
void chex_dump_words(const Chex_Dump *info, const void *in, size_t in_size, FILE *out, const char *word_pfx, const char *separator);

#endif //CHEXDUMP_H
