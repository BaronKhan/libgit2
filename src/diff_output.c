#include "diff_output.h"
#define KNOWN_BINARY_FLAGS (GIT_DIFF_FILE_BINARY|GIT_DIFF_FILE_NOT_BINARY)
#define NOT_BINARY_FLAGS   (GIT_DIFF_FILE_NOT_BINARY|GIT_DIFF_FILE_NO_DATA)
	/* because of blob diffs, cannot assume path is set */
	if (!file->path || !strlen(file->path))

	else if ((delta->old_file.flags & NOT_BINARY_FLAGS) != 0 &&
			 (delta->new_file.flags & NOT_BINARY_FLAGS) != 0)

static int diff_delta_is_binary_by_attr(
	diff_context *ctxt, git_diff_patch *patch)
	git_diff_delta *delta = patch->delta;
	if (ctxt->opts && (ctxt->opts->flags & GIT_DIFF_FORCE_TEXT) != 0) {
		delta->new_file.flags |= (delta->old_file.flags & KNOWN_BINARY_FLAGS);
static int diff_delta_is_binary_by_content(
	diff_context *ctxt, git_diff_delta *delta, git_diff_file *file, git_map *map)
	GIT_UNUSED(ctxt);

	if ((file->flags & KNOWN_BINARY_FLAGS) == 0) {
		search.ptr  = map->data;
		search.size = min(map->len, 4000);
			file->flags |= GIT_DIFF_FILE_BINARY;
			file->flags |= GIT_DIFF_FILE_NOT_BINARY;
	update_delta_is_binary(delta);
	return 0;
}

static int diff_delta_is_binary_by_size(
	diff_context *ctxt, git_diff_delta *delta, git_diff_file *file)
{
	git_off_t threshold = MAX_DIFF_FILESIZE;

	if ((file->flags & KNOWN_BINARY_FLAGS) != 0)
		return 0;

	if (ctxt && ctxt->opts) {
		if (ctxt->opts->max_size < 0)
			return 0;

		if (ctxt->opts->max_size > 0)
			threshold = ctxt->opts->max_size;
	if (file->size > threshold)
		file->flags |= GIT_DIFF_FILE_BINARY;
	update_delta_is_binary(delta);
	const git_diff_options *opts, xdemitconf_t *cfg, xpparam_t *param)

	diff_context *ctxt,
	git_diff_delta *delta,
	git_diff_file *file,
	int error;
	git_odb_object *odb_obj = NULL;

	if (git_oid_iszero(&file->oid))
	if (!file->size) {
		git_odb *odb;
		size_t len;
		git_otype type;

		/* peek at object header to avoid loading if too large */
		if ((error = git_repository_odb__weakptr(&odb, ctxt->repo)) < 0 ||
			(error = git_odb__read_header_or_object(
				&odb_obj, &len, &type, odb, &file->oid)) < 0)
			return error;

		assert(type == GIT_OBJ_BLOB);

		file->size = len;
	}

	/* if blob is too large to diff, mark as binary */
	if ((error = diff_delta_is_binary_by_size(ctxt, delta, file)) < 0)
		return error;
	if (delta->binary == 1)
		return 0;

	if (odb_obj != NULL) {
		error = git_object__from_odb_object(
			(git_object **)blob, ctxt->repo, odb_obj, GIT_OBJ_BLOB);
		git_odb_object_free(odb_obj);
	} else
		error = git_blob_lookup(blob, ctxt->repo, &file->oid);

	if (error)
		return error;

	return diff_delta_is_binary_by_content(ctxt, delta, file, map);
	diff_context *ctxt,
	git_diff_delta *delta,
	const char *wd = git_repository_workdir(ctxt->repo);
	if (git_buf_joinpath(&path, wd, file->path) < 0)
			goto cleanup;
		}

		map->len = read_len;
		git_file fd = git_futils_open_ro(path.ptr);
		git_vector filters = GIT_VECTOR_INIT;

		if (fd < 0) {
			error = fd;
			goto cleanup;
		}

		if (!file->size)
			file->size = git_futils_filesize(fd);

		if ((error = diff_delta_is_binary_by_size(ctxt, delta, file)) < 0 ||
			delta->binary == 1)
			goto close_and_cleanup;

		error = git_filters_load(
			&filters, ctxt->repo, file->path, GIT_FILTER_TO_ODB);
		if (error < 0)
			goto close_and_cleanup;

		if (error == 0) { /* note: git_filters_load returns filter count */
			error = git_futils_mmap_ro(map, fd, 0, (size_t)file->size);
			file->flags |= GIT_DIFF_FILE_UNMAP_DATA;
		} else {
			git_buf raw = GIT_BUF_INIT, filtered = GIT_BUF_INIT;

			if (!(error = git_futils_readbuffer_fd(&raw, fd, (size_t)file->size)) &&
				!(error = git_filters_apply(&filtered, &raw, &filters)))
			{
				map->len  = git_buf_len(&filtered);
				map->data = git_buf_detach(&filtered);

				file->flags |= GIT_DIFF_FILE_FREE_DATA;
			}

			git_buf_free(&raw);
			git_buf_free(&filtered);
		}

close_and_cleanup:
		git_filters_free(&filters);
		p_close(fd);
	}

	/* once data is loaded, update OID if we didn't have it previously */
	if (!error && (file->flags & GIT_DIFF_FILE_VALID_OID) == 0) {
		error = git_odb_hash(
			&file->oid, map->data, map->len, GIT_OBJ_BLOB);
		if (!error)
			file->flags |= GIT_DIFF_FILE_VALID_OID;

	if (!error)
		error = diff_delta_is_binary_by_content(ctxt, delta, file, map);

cleanup:

static void diff_context_init(
	diff_context *ctxt,
	git_diff_list *diff,
	git_repository *repo,
	const git_diff_options *opts,
	void *data,
	git_diff_file_fn file_cb,
	git_diff_hunk_fn hunk_cb,
	git_diff_data_fn data_cb)
	memset(ctxt, 0, sizeof(diff_context));
	ctxt->diff = diff;
	ctxt->file_cb = file_cb;
	ctxt->hunk_cb = hunk_cb;
	ctxt->data_cb = data_cb;
	ctxt->cb_data = data;
	ctxt->cb_error = 0;
	setup_xdiff_options(ctxt->opts, &ctxt->xdiff_config, &ctxt->xdiff_params);
static int diff_delta_file_callback(
	diff_context *ctxt, git_diff_delta *delta, size_t idx)
	float progress;

	if (!ctxt->file_cb)
		return 0;

	progress = ctxt->diff ? ((float)idx / ctxt->diff->deltas.length) : 1.0f;

	if (ctxt->file_cb(ctxt->cb_data, delta, progress) != 0)
		ctxt->cb_error = GIT_EUSER;

	return ctxt->cb_error;
static void diff_patch_init(
	diff_context *ctxt, git_diff_patch *patch)
	memset(patch, 0, sizeof(git_diff_patch));
	patch->diff = ctxt->diff;
	patch->ctxt = ctxt;
	if (patch->diff) {
		patch->old_src = patch->diff->old_src;
		patch->new_src = patch->diff->new_src;
	} else {
		patch->old_src = patch->new_src = GIT_ITERATOR_TREE;
	}
static git_diff_patch *diff_patch_alloc(
	diff_context *ctxt, git_diff_delta *delta)
	git_diff_patch *patch = git__malloc(sizeof(git_diff_patch));
	if (!patch)
		return NULL;
	diff_patch_init(ctxt, patch);
	git_diff_list_addref(patch->diff);
	GIT_REFCOUNT_INC(patch);
	patch->delta = delta;
	patch->flags = GIT_DIFF_PATCH_ALLOCATED;

	return patch;
static int diff_patch_load(
	diff_context *ctxt, git_diff_patch *patch)
	git_diff_delta *delta = patch->delta;
	bool check_if_unmodified = false;
	if ((patch->flags & GIT_DIFF_PATCH_LOADED) != 0)
	error = diff_delta_is_binary_by_attr(ctxt, patch);
	patch->old_data.data = "";
	patch->old_data.len  = 0;
	patch->old_blob      = NULL;
	patch->new_data.data = "";
	patch->new_data.len  = 0;
	patch->new_blob      = NULL;

	if (delta->binary == 1)
		goto cleanup;

	switch (delta->status) {
	case GIT_DELTA_ADDED:
		delta->old_file.flags |= GIT_DIFF_FILE_NO_DATA;
		break;
	case GIT_DELTA_DELETED:
		delta->new_file.flags |= GIT_DIFF_FILE_NO_DATA;
		break;
	case GIT_DELTA_MODIFIED:
		break;
	default:
		delta->new_file.flags |= GIT_DIFF_FILE_NO_DATA;
		delta->old_file.flags |= GIT_DIFF_FILE_NO_DATA;
		break;
#define CHECK_UNMODIFIED (GIT_DIFF_FILE_NO_DATA | GIT_DIFF_FILE_VALID_OID)
	check_if_unmodified =
		(delta->old_file.flags & CHECK_UNMODIFIED) == 0 &&
		(delta->new_file.flags & CHECK_UNMODIFIED) == 0;
	/* Always try to load workdir content first, since it may need to be
	 * filtered (and hence use 2x memory) and we want to minimize the max
	 * memory footprint during diff.
	 */
	if ((delta->old_file.flags & GIT_DIFF_FILE_NO_DATA) == 0 &&
		patch->old_src == GIT_ITERATOR_WORKDIR) {
		if ((error = get_workdir_content(
				ctxt, delta, &delta->old_file, &patch->old_data)) < 0)
			goto cleanup;
		if (delta->binary == 1)
			goto cleanup;
	}
	if ((delta->new_file.flags & GIT_DIFF_FILE_NO_DATA) == 0 &&
		patch->new_src == GIT_ITERATOR_WORKDIR) {
		if ((error = get_workdir_content(
				ctxt, delta, &delta->new_file, &patch->new_data)) < 0)
			goto cleanup;
		if (delta->binary == 1)
			goto cleanup;
	}
	if ((delta->old_file.flags & GIT_DIFF_FILE_NO_DATA) == 0 &&
		patch->old_src != GIT_ITERATOR_WORKDIR) {
		if ((error = get_blob_content(
				ctxt, delta, &delta->old_file,
				&patch->old_data, &patch->old_blob)) < 0)
			goto cleanup;
		if (delta->binary == 1)
			goto cleanup;
	}

	if ((delta->new_file.flags & GIT_DIFF_FILE_NO_DATA) == 0 &&
		patch->new_src != GIT_ITERATOR_WORKDIR) {
		if ((error = get_blob_content(
				ctxt, delta, &delta->new_file,
				&patch->new_data, &patch->new_blob)) < 0)
			goto cleanup;
		if (delta->binary == 1)
			goto cleanup;
	/* if we did not previously have the definitive oid, we may have
	 * incorrect status and need to switch this to UNMODIFIED.
	if (check_if_unmodified &&
		delta->old_file.mode == delta->new_file.mode &&
		!git_oid_cmp(&delta->old_file.oid, &delta->new_file.oid))
	{
		delta->status = GIT_DELTA_UNMODIFIED;

		if ((ctxt->opts->flags & GIT_DIFF_INCLUDE_UNMODIFIED) == 0)
			goto cleanup;
	}
	if (delta->binary == -1)
		update_delta_is_binary(delta);
	if (!error) {
		patch->flags |= GIT_DIFF_PATCH_LOADED;

		if (delta->binary != 1 &&
			delta->status != GIT_DELTA_UNMODIFIED &&
			(patch->old_data.len || patch->new_data.len) &&
			!git_oid_equal(&delta->old_file.oid, &delta->new_file.oid))
			patch->flags |= GIT_DIFF_PATCH_DIFFABLE;
	}
static int diff_patch_cb(void *priv, mmbuffer_t *bufs, int len)
	git_diff_patch *patch = priv;
	diff_context   *ctxt  = patch->ctxt;
		ctxt->cb_error = parse_hunk_header(&ctxt->cb_range, bufs[0].ptr);
		if (ctxt->cb_error < 0)
		if (ctxt->hunk_cb != NULL &&
			ctxt->hunk_cb(ctxt->cb_data, patch->delta, &ctxt->cb_range,
		if (ctxt->data_cb != NULL &&
			ctxt->data_cb(ctxt->cb_data, patch->delta, &ctxt->cb_range,
				origin, bufs[1].ptr, bufs[1].size))
		/* If we have a '+' and a third buf, then we have added a line
		 * without a newline and the old code had one, so DEL_EOFNL.
		 * If we have a '-' and a third buf, then we have removed a line
		 * with out a newline but added a blank line, so ADD_EOFNL.
		if (ctxt->data_cb != NULL &&
			ctxt->data_cb(ctxt->cb_data, patch->delta, &ctxt->cb_range,
				origin, bufs[2].ptr, bufs[2].size))
static int diff_patch_generate(
	diff_context *ctxt, git_diff_patch *patch)
	if ((patch->flags & GIT_DIFF_PATCH_DIFFED) != 0)
	if ((patch->flags & GIT_DIFF_PATCH_LOADED) == 0)
		if ((error = diff_patch_load(ctxt, patch)) < 0)
			return error;
	if ((patch->flags & GIT_DIFF_PATCH_DIFFABLE) == 0)
	if (ctxt)
		patch->ctxt = ctxt;
	xdiff_callback.outf = diff_patch_cb;
	xdiff_callback.priv = patch;
	old_xdiff_data.ptr  = patch->old_data.data;
	old_xdiff_data.size = patch->old_data.len;
	new_xdiff_data.ptr  = patch->new_data.data;
	new_xdiff_data.size = patch->new_data.len;
	if (!error)
		patch->flags |= GIT_DIFF_PATCH_DIFFED;
static void diff_patch_unload(git_diff_patch *patch)
{
	if ((patch->flags & GIT_DIFF_PATCH_DIFFED) != 0) {
		patch->flags = (patch->flags & ~GIT_DIFF_PATCH_DIFFED);

		patch->hunks_size = 0;
		patch->lines_size = 0;
	}

	if ((patch->flags & GIT_DIFF_PATCH_LOADED) != 0) {
		patch->flags = (patch->flags & ~GIT_DIFF_PATCH_LOADED);

		release_content(
			&patch->delta->old_file, &patch->old_data, patch->old_blob);
		release_content(
			&patch->delta->new_file, &patch->new_data, patch->new_blob);
	}
}

static void diff_patch_free(git_diff_patch *patch)
{
	diff_patch_unload(patch);

	git__free(patch->lines);
	patch->lines = NULL;
	patch->lines_asize = 0;

	git__free(patch->hunks);
	patch->hunks = NULL;
	patch->hunks_asize = 0;

	if (!(patch->flags & GIT_DIFF_PATCH_ALLOCATED))
		return;

	patch->flags = 0;

	git_diff_list_free(patch->diff); /* decrements refcount */

	git__free(patch);
}

#define MAX_HUNK_STEP 128
#define MIN_HUNK_STEP 8
#define MAX_LINE_STEP 256
#define MIN_LINE_STEP 8

static int diff_patch_hunk_cb(
	void *cb_data,
	const git_diff_delta *delta,
	const git_diff_range *range,
	const char *header,
	size_t header_len)
{
	git_diff_patch *patch = cb_data;
	diff_patch_hunk *hunk;

	GIT_UNUSED(delta);

	if (patch->hunks_size >= patch->hunks_asize) {
		size_t new_size;
		diff_patch_hunk *new_hunks;

		if (patch->hunks_asize > MAX_HUNK_STEP)
			new_size = patch->hunks_asize + MAX_HUNK_STEP;
		else
			new_size = patch->hunks_asize * 2;
		if (new_size < MIN_HUNK_STEP)
			new_size = MIN_HUNK_STEP;

		new_hunks = git__realloc(
			patch->hunks, new_size * sizeof(diff_patch_hunk));
		if (!new_hunks)
			return -1;

		patch->hunks = new_hunks;
		patch->hunks_asize = new_size;
	}

	hunk = &patch->hunks[patch->hunks_size++];

	memcpy(&hunk->range, range, sizeof(hunk->range));

	assert(header_len + 1 < sizeof(hunk->header));
	memcpy(&hunk->header, header, header_len);
	hunk->header[header_len] = '\0';
	hunk->header_len = header_len;

	hunk->line_start = patch->lines_size;
	hunk->line_count = 0;

	return 0;
}

static int diff_patch_line_cb(
	void *cb_data,
	const git_diff_delta *delta,
	const git_diff_range *range,
	char line_origin,
	const char *content,
	size_t content_len)
{
	git_diff_patch *patch = cb_data;
	diff_patch_hunk *hunk;
	diff_patch_line *last, *line;

	GIT_UNUSED(delta);
	GIT_UNUSED(range);

	assert(patch->hunks_size > 0);
	assert(patch->hunks != NULL);

	hunk = &patch->hunks[patch->hunks_size - 1];

	if (patch->lines_size >= patch->lines_asize) {
		size_t new_size;
		diff_patch_line *new_lines;

		if (patch->lines_asize > MAX_LINE_STEP)
			new_size = patch->lines_asize + MAX_LINE_STEP;
		else
			new_size = patch->lines_asize * 2;
		if (new_size < MIN_LINE_STEP)
			new_size = MIN_LINE_STEP;

		new_lines = git__realloc(
			patch->lines, new_size * sizeof(diff_patch_line));
		if (!new_lines)
			return -1;

		patch->lines = new_lines;
		patch->lines_asize = new_size;
	}

	last = (patch->lines_size > 0) ?
		&patch->lines[patch->lines_size - 1] : NULL;
	line = &patch->lines[patch->lines_size++];

	line->ptr = content;
	line->len = content_len;
	line->origin = line_origin;

	/* do some bookkeeping so we can provide old/new line numbers */

	for (line->lines = 0; content_len > 0; --content_len) {
		if (*content++ == '\n')
			++line->lines;
	}

	if (!last) {
		line->oldno = hunk->range.old_start;
		line->newno = hunk->range.new_start;
	} else {
		switch (last->origin) {
		case GIT_DIFF_LINE_ADDITION:
			line->oldno = last->oldno;
			line->newno = last->newno + last->lines;
			break;
		case GIT_DIFF_LINE_DELETION:
			line->oldno = last->oldno + last->lines;
			line->newno = last->newno;
			break;
		default:
			line->oldno = last->oldno + last->lines;
			line->newno = last->newno + last->lines;
			break;
		}
	}

	hunk->line_count++;

	return 0;
}


	void *cb_data,
	git_diff_data_fn data_cb)
	diff_context ctxt;
	git_diff_patch patch;

	diff_context_init(
		&ctxt, diff, diff->repo, &diff->opts,
		cb_data, file_cb, hunk_cb, data_cb);
	diff_patch_init(&ctxt, &patch);
	git_vector_foreach(&diff->deltas, idx, patch.delta) {
		/* check flags against patch status */
		if (git_diff_delta__should_skip(ctxt.opts, patch.delta))
		if (!(error = diff_patch_load(&ctxt, &patch))) {
			/* invoke file callback */
			error = diff_delta_file_callback(&ctxt, patch.delta, idx);
			/* generate diffs and invoke hunk and line callbacks */
			if (!error)
				error = diff_patch_generate(&ctxt, &patch);
			diff_patch_unload(&patch);
		}
		giterr_clear(); /* don't let error message leak */

char git_diff_status_char(git_delta_t status)
{
	char code;

	switch (status) {
	case GIT_DELTA_ADDED:     code = 'A'; break;
	case GIT_DELTA_DELETED:   code = 'D'; break;
	case GIT_DELTA_MODIFIED:  code = 'M'; break;
	case GIT_DELTA_RENAMED:   code = 'R'; break;
	case GIT_DELTA_COPIED:    code = 'C'; break;
	case GIT_DELTA_IGNORED:   code = 'I'; break;
	case GIT_DELTA_UNTRACKED: code = '?'; break;
	default:                  code = ' '; break;
	}

	return code;
}

static int print_compact(
	void *data, const git_diff_delta *delta, float progress)
	char old_suffix, new_suffix, code = git_diff_status_char(delta->status);
	if (code == ' ')
static int print_oid_range(diff_print_info *pi, const git_diff_delta *delta)
static int print_patch_file(
	void *data, const git_diff_delta *delta, float progress)
	const git_diff_delta *d,
	const git_diff_range *r,
	const git_diff_delta *delta,
	const git_diff_range *range,
		file->mode = 0644;
		file->flags |= GIT_DIFF_FILE_NO_DATA;
	const git_diff_options *options,
	git_diff_data_fn data_cb)
	git_repository *repo;
	diff_context ctxt;
	git_diff_patch patch;
		git_blob *swap = old_blob;
		old_blob = new_blob;
		new_blob = swap;
	if (new_blob)
		repo = git_object_owner((git_object *)new_blob);
	else if (old_blob)
		repo = git_object_owner((git_object *)old_blob);
	else
		repo = NULL;

	diff_context_init(
		&ctxt, NULL, repo, options,
		cb_data, file_cb, hunk_cb, data_cb);

	diff_patch_init(&ctxt, &patch);
	/* create a fake delta record and simulate diff_patch_load */
	delta.binary = -1;
	set_data_from_blob(old_blob, &patch.old_data, &delta.old_file);
	set_data_from_blob(new_blob, &patch.new_data, &delta.new_file);
	delta.status = new_blob ?
		(old_blob ? GIT_DELTA_MODIFIED : GIT_DELTA_ADDED) :
		(old_blob ? GIT_DELTA_DELETED : GIT_DELTA_UNTRACKED);
	patch.delta = &delta;
	if ((error = diff_delta_is_binary_by_content(
			 &ctxt, &delta, &delta.old_file, &patch.old_data)) < 0 ||
		(error = diff_delta_is_binary_by_content(
			&ctxt, &delta, &delta.new_file, &patch.new_data)) < 0)
	patch.flags |= GIT_DIFF_PATCH_LOADED;
	if (delta.binary != 1 && delta.status != GIT_DELTA_UNMODIFIED)
		patch.flags |= GIT_DIFF_PATCH_DIFFABLE;
	if (!(error = diff_delta_file_callback(&ctxt, patch.delta, 1)))
		error = diff_patch_generate(&ctxt, &patch);
	diff_patch_unload(&patch);

size_t git_diff_num_deltas(git_diff_list *diff)
	assert(diff);
	return (size_t)diff->deltas.length;
size_t git_diff_num_deltas_of_type(git_diff_list *diff, git_delta_t type)
	size_t i, count = 0;
	git_diff_delta *delta;
	assert(diff);
	git_vector_foreach(&diff->deltas, i, delta) {
		count += (delta->status == type);
	return count;
int git_diff_get_patch(
	git_diff_patch **patch_ptr,
	const git_diff_delta **delta_ptr,
	git_diff_list *diff,
	size_t idx)
	diff_context ctxt;
	git_diff_delta *delta;
	git_diff_patch *patch;
	if (patch_ptr)
		*patch_ptr = NULL;
	delta = git_vector_get(&diff->deltas, idx);
	if (!delta) {
		if (delta_ptr)
			*delta_ptr = NULL;
		giterr_set(GITERR_INVALID, "Index out of range for delta in diff");
		return GIT_ENOTFOUND;
	if (delta_ptr)
		*delta_ptr = delta;
	if (!patch_ptr && delta->binary != -1)
		return 0;
	diff_context_init(
		&ctxt, diff, diff->repo, &diff->opts,
		NULL, NULL, diff_patch_hunk_cb, diff_patch_line_cb);
	if (git_diff_delta__should_skip(ctxt.opts, delta))
		return 0;
	patch = diff_patch_alloc(&ctxt, delta);
	if (!patch)
		return -1;
	if (!(error = diff_patch_load(&ctxt, patch))) {
		ctxt.cb_data = patch;
		error = diff_patch_generate(&ctxt, patch);
		if (error == GIT_EUSER)
			error = ctxt.cb_error;
	if (error)
		git_diff_patch_free(patch);
	else if (patch_ptr)
		*patch_ptr = patch;
	return error;
void git_diff_patch_free(git_diff_patch *patch)
	if (patch)
		GIT_REFCOUNT_DEC(patch, diff_patch_free);
const git_diff_delta *git_diff_patch_delta(git_diff_patch *patch)
	assert(patch);
	return patch->delta;
size_t git_diff_patch_num_hunks(git_diff_patch *patch)
	assert(patch);
	return patch->hunks_size;
int git_diff_patch_get_hunk(
	const git_diff_range **range,
	size_t *lines_in_hunk,
	git_diff_patch *patch,
	size_t hunk_idx)
	diff_patch_hunk *hunk;
	assert(patch);
	if (hunk_idx >= patch->hunks_size) {
		if (range) *range = NULL;
		if (lines_in_hunk) *lines_in_hunk = 0;
		return GIT_ENOTFOUND;
	hunk = &patch->hunks[hunk_idx];
	if (range) *range = &hunk->range;
	if (header) *header = hunk->header;
	if (header_len) *header_len = hunk->header_len;
	if (lines_in_hunk) *lines_in_hunk = hunk->line_count;
	return 0;
}
int git_diff_patch_num_lines_in_hunk(
	git_diff_patch *patch,
	size_t hunk_idx)
{
	assert(patch);
	if (hunk_idx >= patch->hunks_size)
		return GIT_ENOTFOUND;
	else
		return patch->hunks[hunk_idx].line_count;
int git_diff_patch_get_line_in_hunk(
	char *line_origin,
	const char **content,
	int *old_lineno,
	int *new_lineno,
	git_diff_patch *patch,
	size_t hunk_idx,
	size_t line_of_hunk)
	diff_patch_hunk *hunk;
	diff_patch_line *line;
	assert(patch);
	if (hunk_idx >= patch->hunks_size)
		goto notfound;
	hunk = &patch->hunks[hunk_idx];
	if (line_of_hunk >= hunk->line_count)
		goto notfound;
	line = &patch->lines[hunk->line_start + line_of_hunk];
	if (line_origin) *line_origin = line->origin;
	if (content) *content = line->ptr;
	if (content_len) *content_len = line->len;
	if (old_lineno) *old_lineno = line->oldno;
	if (new_lineno) *new_lineno = line->newno;
	return 0;

notfound:
	if (line_origin) *line_origin = GIT_DIFF_LINE_CONTEXT;
	if (content) *content = NULL;
	if (content_len) *content_len = 0;
	if (old_lineno) *old_lineno = -1;
	if (new_lineno) *new_lineno = -1;

	return GIT_ENOTFOUND;
