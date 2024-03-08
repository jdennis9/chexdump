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
#include "chexdump.c"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef _WIN32
#define stat _stat
#endif

enum  {
	ARG_PREFIX,
	ARG_NAME,
	ARG__COUNT,
};

static inline size_t pad_to_alignment(size_t x, size_t alignment) {
	size_t m = x%alignment;
	return m ? x + (alignment-m) : x;
}

static void print_usage() {
	CHEX_FORMAT__NAMES(format_names);
	CHEX_FORMAT__DESCRIPTIONS(format_descriptions);
	printf("chex [options...] format infile\n");
	
	printf("\nOptions:\n");
	printf("\t-h,--help: Show this help and exit\n");
	printf("\t-wordsize <value>: Set word size in bytes. Supported sizes: 1, 2, 4, 8\n");
	printf("\t-caps: Capitilize variable names\n");
	printf("\t-name <value>: Set base name for variables\n");
	printf("\t-prefix <string>: Prefix variable names with string\n");
	
	printf("\nFormats:\n");
	for (int i = 0; i < CHEX_FORMAT__COUNT; ++i) {
		printf("\t%s: %s\n", format_names[i], format_descriptions[i]);
	}
}

int main(int argc, char *argv[]) {
	CHEX_FORMAT__NAMES(format_names);
	Chex_Dump info = {0};
	struct {
		int caps : 1;
	} flags;
	
	const char *args[ARG__COUNT] = {NULL};
	
	info.word_size = 1;
	
	if (argc < 3) {
		print_usage();
		return 1;
	}
	
	argc--;
	argv++;
	
	// Parse options
	while (argc && argv[0][0] == '-') {
		int ok = 0;
		const char *string_arg_names[] = {
			[ARG_PREFIX] = "-prefix",
			[ARG_NAME] = "-name",
		};
		
		if (!strcmp(*argv, "--help") || !strcmp(*argv, "-h")) {
			print_usage();
			return 0;
		}
		else if (!strcmp(*argv, "-wordsize")) {
			--argc;
			++argv;
			if (!argc) {
				print_usage();
				return 1;
			}
			
			info.word_size = atoi(*argv);
			if (info.word_size != 8 && info.word_size != 4 && info.word_size != 2 && info.word_size != 1) {
				printf("Unsupported word size %s\n", *argv);
				return 1;
			}
			
			ok = 1;
		}
		else if (!strcmp(*argv, "-caps")) {
			flags.caps = 1;
			ok = 1;
		}
		
		else for (int i = 0; i < ARG__COUNT; ++i) {
			if (!strcmp(*argv, string_arg_names[i])) {
				argc--;
				argv++;
				if (!argc) {
					print_usage();
					return 1;
				}
				
				args[i] = *argv;
				ok = 1;
				break;
			}
		}
		
		if (!ok) {
			printf("Unrecognised argument: %s\n", *argv);
			return 1;
		}
		
		--argc;
		++argv;
	}
	
	if (argc < 2) {
		print_usage();
		return 1;
	}
	
	Chex_Format format = CHEX_FORMAT__COUNT;
	for (int i = 0; i < CHEX_FORMAT__COUNT; ++i) {
		if (!strcmp(*argv, format_names[i])) {
			format = i;
			break;
		}
	}
	
	if (format == CHEX_FORMAT__COUNT) {
		printf("Unrecognised format: %s\n", *argv);
		return 1;
	}
	
	argc--;
	argv++;
	
	struct stat st;
	if (stat(*argv, &st)) {
		perror("Could not stat input file");
		return 1;
	}
	
	// Format name
	char basename[512];
	info.basename = basename;
	{
		const char *c;
		int len = 0;
		
		if (args[ARG_PREFIX]) {
			c = args[ARG_PREFIX];
			for (; *c; ++c) basename[len++] = *c;
		}
		
		c = args[ARG_NAME] ? args[ARG_NAME] : *argv;
		if (flags.caps) for (; *c; ++c, ++len) {
			if (!isalnum(*c)) basename[len] = '_';
			else basename[len] = toupper(*c);
		}
		else for (; *c; ++c, ++len) {
			if (!isalnum(*c)) basename[len] = '_';
			else basename[len] = *c;
		}
		basename[len] = 0;
	}
	
	if (format == CHEX_FORMAT_C_EXTERN_HEADER) {
		chex_dump(&info, NULL, st.st_size, stdout, format);
	}
	else {
		FILE *infile = fopen(*argv, "rb");
		size_t padding_bytes;
		size_t padded_size;
		void *buffer;
		
		if (infile) {
			perror("Failed to open input file for reading");
			return 1;
		}
		
		padded_size = pad_to_alignment(st.st_size, info.word_size);
		buffer = malloc(pad_to_alignment(st.st_size, info.word_size));
		padding_bytes = st.st_size - ((st.st_size/info.word_size)*info.word_size);
		if (padding_bytes) memset((uint8_t*)buffer + st.st_size, 0, padding_bytes);
		
		fread(buffer, st.st_size, 1, infile);
		
		chex_dump(&info, buffer, st.st_size, stdout, format);
		
		free(buffer);
	}
	
	return 0;
}
	
	