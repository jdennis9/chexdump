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
#include "chexdump.h"
#include <stdint.h>

static inline size_t chex__calc_word_count(size_t word_size, size_t total_size) {
	return (total_size/word_size) + !!(total_size%word_size);
}

static void chex_dump_c__any(const Chex_Dump *info, const void *in, size_t in_size, FILE *out, const char *pfx);

static inline int chex__get_word_bits(size_t word_size) {
	int ret = word_size * 8;
	if (word_size != 8 && word_size != 4 && word_size != 2) ret = 8;
	return ret;
}

void chex_dump(const Chex_Dump *info, const void *in, size_t in_size, FILE *out, Chex_Format format) {
	CHEX_FORMAT__HOOKS(hooks);
	if ((format < CHEX_FORMAT__COUNT) && hooks[format]) hooks[format](info, in, in_size, out);
}

void chex_dump_words(const Chex_Dump *info, const void *in, size_t in_size, FILE *out, const char *word_pfx, const char *separator) {
	size_t word_count = chex__calc_word_count(info->word_size, in_size);
	switch (info->word_size) {
		case 8: {
			const uint64_t *words = (uint64_t*)in;
			for (int i = 0; i < word_count; ++i) {
				fprintf(out, "%s%016llx%s", word_pfx, words[i], separator);
				if (((i+1) % 4) == 0) fprintf(out, "\n");
			}
			break;
		}
		case 4: {
			const uint32_t *words = (uint32_t*)in;
			for (int i = 0; i < word_count; ++i) {
				fprintf(out, "%s%08x%s", word_pfx, words[i], separator);
				if (((i+1) % 8) == 0) fprintf(out, "\n");
			}
			break;
		}
		case 2: {
			const uint16_t *words = (uint16_t*)in;
			for (int i = 0; i < word_count; ++i) {
				fprintf(out, "%s%04x%s", word_pfx, words[i], separator);
				if (((i+1) % 8) == 0) fprintf(out, "\n");
			}
			break;
		}
		default: {
			const uint8_t *words = (uint8_t*)in;
			for (int i = 0; i < word_count; ++i) {
				fprintf(out, "%s%02x%s", word_pfx, words[i], separator);
				if (((i+1) % 16) == 0) fprintf(out, "\n");
			}
			break;
		}
	}
}

static void chex_dump_c__any(const Chex_Dump *info, const void *in, size_t in_size, FILE *out, const char *pfx) {
	size_t word_count = chex__calc_word_count(info->word_size, in_size);
	
	switch (info->word_size) {
		case 8: {
			const uint64_t *words = (uint64_t*)in;
			fprintf(out, "%suint64_t %s[%zu] = {\n", pfx, info->basename, word_count);
			break;
		}
		case 4: {
			const uint32_t *words = (uint32_t*)in;
			fprintf(out, "%suint32_t %s[%zu] = {\n", pfx, info->basename, word_count);
			break;
		}
		case 2: {
			const uint16_t *words = (uint16_t*)in;
			fprintf(out, "%suint16_t %s[%zu] = {\n", pfx, info->basename, word_count);
			break;
		}
		default: {
			const uint8_t *words = (uint8_t*)in;
			fprintf(out, "%suint8_t %s[%zu] = {\n", pfx, info->basename, word_count);
			break;
		}
	}
	
	chex_dump_words(info, in, in_size, out, "0x", ",");
	
	fprintf(out, "\n};\n");
}

void chex_dump_c_extern(const Chex_Dump *info, const void *in, size_t in_size, FILE *out) {
	size_t word_count = chex__calc_word_count(info->word_size, in_size);
	int word_size;
	const char *type;
	
	word_size = chex__get_word_bits(info->word_size);
	
	fprintf(out, "extern uint%d_t %s[%zu];\n", word_size, info->basename, word_count);
	fprintf(out, "static const size_t %s_SIZE = %zu;\n", info->basename, in_size);
}

void chex_dump_c_source(const Chex_Dump *info, const void *in, size_t in_size, FILE *out) {
	chex_dump_c__any(info, in, in_size, out, "");
}

void chex_dump_c_static(const Chex_Dump *info, const void *in, size_t in_size, FILE *out) {
	fprintf(out, "static const size_t %s_SIZE = %zu;\n", info->basename, in_size);
	chex_dump_c__any(info, in, in_size, out, "static const ");
}

void chex_dump_zig(const Chex_Dump *info, const void *in, size_t in_size, FILE *out) {
	int word_size;
	word_size = chex__get_word_bits(info->word_size);
	fprintf(out, "const %s [%zu]u%d = {\n", info->basename, in_size, word_size);
	chex_dump_words(info, in, in_size, out, "0x", ",");
	fprintf(out, "\n};\n");
}

void chex_dump_long(const Chex_Dump *info, const void *in, size_t in_size, FILE *out) {
	uint8_t *data = (uint8_t*)in;
	for (size_t i = 0; i < in_size; ++i) {
		fprintf(out, "%x", data[i]);
	}
}
