/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "fileops.h"
#include "path.h"

extern int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size);
extern int LLVMFuzzerInitialize(int *argc, char ***argv);

static int run_one_file(const char *filename)
{
	git_buf buf = GIT_BUF_INIT;
	int error = 0;

	if (git_futils_readbuffer(&buf, filename) < 0) {
		fprintf(stderr, "Failed to read %s: %m\n", filename);
		error = -1;
		goto exit;
	}

	LLVMFuzzerTestOneInput((const unsigned char *)buf.ptr, buf.size);
exit:
	git_buf_dispose(&buf);
	return error;
}

int main(int argc, char **argv)
{
	git_vector corpus_files = GIT_VECTOR_INIT;
	char *filename = NULL;
	unsigned i = 0;
	int error = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <corpus directory>\n", argv[0]);
		error = -1;
		goto exit;
	}

	fprintf(stderr, "Running %s against %s\n", argv[0], argv[1]);
	LLVMFuzzerInitialize(&argc, &argv);

	if (git_path_dirload(&corpus_files, argv[1], 0, 0x0) < 0) {
		fprintf(stderr, "Failed to scan corpus directory: %m\n");
		error = -1;
		goto exit;
	}
	git_vector_foreach(&corpus_files, i, filename) {
		fprintf(stderr, "\tRunning %s...\n", filename);
		if (run_one_file(filename) < 0) {
			error = -1;
			goto exit;
		}
	}
	fprintf(stderr, "Done %d runs\n", i);

exit:
	git_vector_free_deep(&corpus_files);
	return error;
}
