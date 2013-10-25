#include "fileops.h"
	git_diff *diff;
	git_diff_format_t format;
	git_diff_line_cb print_cb;
	uint32_t flags;
	git_diff_line line;
	git_buf *out,
	git_diff *diff,
	git_diff_format_t format,
	git_diff_line_cb cb,
	void *payload)
	pi->format   = format;
	if (diff)
		pi->flags = diff->opts.flags;

	if (diff && diff->opts.oid_abbrev != 0)
		pi->oid_strlen = diff->opts.oid_abbrev;
	else if (!diff || !diff->repo)
	memset(&pi->line, 0, sizeof(pi->line));
	pi->line.old_lineno = -1;
	pi->line.new_lineno = -1;
	pi->line.num_lines  = 1;

	else if (GIT_PERMS_IS_EXEC(mode)) /* -V536 */
static int diff_print_one_name_only(
	const git_diff_delta *delta, float progress, void *data)
{
	diff_print_info *pi = data;
	git_buf *out = pi->buf;

	GIT_UNUSED(progress);

	if ((pi->flags & GIT_DIFF_SHOW_UNMODIFIED) == 0 &&
		delta->status == GIT_DELTA_UNMODIFIED)
		return 0;

	git_buf_clear(out);

	if (git_buf_puts(out, delta->new_file.path) < 0 ||
		git_buf_putc(out, '\n'))
		return -1;

	pi->line.origin      = GIT_DIFF_LINE_FILE_HDR;
	pi->line.content     = git_buf_cstr(out);
	pi->line.content_len = git_buf_len(out);

	if (pi->print_cb(delta, NULL, &pi->line, pi->payload))
		return callback_error();

	return 0;
}

static int diff_print_one_name_status(
	if ((pi->flags & GIT_DIFF_SHOW_UNMODIFIED) == 0 && code == ' ')
		git_buf_printf(out, "%c\t%s%c %s%c\n", code,
		git_buf_printf(out, "%c\t%s%c %s%c\n", code,
			delta->old_file.path, old_suffix, delta->new_file.path, new_suffix);
	pi->line.origin      = GIT_DIFF_LINE_FILE_HDR;
	pi->line.content     = git_buf_cstr(out);
	pi->line.content_len = git_buf_len(out);

	if (pi->print_cb(delta, NULL, &pi->line, pi->payload))
	if ((pi->flags & GIT_DIFF_SHOW_UNMODIFIED) == 0 && code == ' ')
	pi->line.origin      = GIT_DIFF_LINE_FILE_HDR;
	pi->line.content     = git_buf_cstr(out);
	pi->line.content_len = git_buf_len(out);

	if (pi->print_cb(delta, NULL, &pi->line, pi->payload))
static int diff_print_oid_range(
	git_buf *out, const git_diff_delta *delta, int oid_strlen)
	git_oid_tostr(start_oid, oid_strlen, &delta->old_file.oid);
	git_oid_tostr(end_oid, oid_strlen, &delta->new_file.oid);
static int diff_delta_format_with_paths(
	git_buf *out,
	const git_diff_delta *delta,
	const char *oldpfx,
	const char *newpfx,
	const char *template)
	if (git_oid_iszero(&delta->old_file.oid)) {
		oldpfx = "";
		oldpath = "/dev/null";
	}
	if (git_oid_iszero(&delta->new_file.oid)) {
		newpfx = "";
		newpath = "/dev/null";
	}
	return git_buf_printf(out, template, oldpfx, oldpath, newpfx, newpath);
}
int git_diff_delta__format_file_header(
	git_buf *out,
	const git_diff_delta *delta,
	const char *oldpfx,
	const char *newpfx,
	int oid_strlen)
{
	if (!oid_strlen)
		oid_strlen = GIT_ABBREV_DEFAULT + 1;
	git_buf_clear(out);

	git_buf_printf(out, "diff --git %s%s %s%s\n",
	if (diff_print_oid_range(out, delta, oid_strlen) < 0)
	if ((delta->flags & GIT_DIFF_FLAG_BINARY) == 0)
		diff_delta_format_with_paths(
			out, delta, oldpfx, newpfx, "--- %s%s\n+++ %s%s\n");
	return git_buf_oom(out) ? -1 : 0;
}

static int diff_print_patch_file(
	const git_diff_delta *delta, float progress, void *data)
{
	diff_print_info *pi = data;
	const char *oldpfx =
		pi->diff ? pi->diff->opts.old_prefix : DIFF_OLD_PREFIX_DEFAULT;
	const char *newpfx =
		pi->diff ? pi->diff->opts.new_prefix : DIFF_NEW_PREFIX_DEFAULT;
	GIT_UNUSED(progress);

	if (S_ISDIR(delta->new_file.mode) ||
		delta->status == GIT_DELTA_UNMODIFIED ||
		delta->status == GIT_DELTA_IGNORED ||
		(delta->status == GIT_DELTA_UNTRACKED &&
		 (pi->flags & GIT_DIFF_SHOW_UNTRACKED_CONTENT) == 0))
		return 0;

	if (git_diff_delta__format_file_header(
			pi->buf, delta, oldpfx, newpfx, pi->oid_strlen) < 0)
	pi->line.origin      = GIT_DIFF_LINE_FILE_HDR;
	pi->line.content     = git_buf_cstr(pi->buf);
	pi->line.content_len = git_buf_len(pi->buf);

	if (pi->print_cb(delta, NULL, &pi->line, pi->payload))

	if (diff_delta_format_with_paths(
			pi->buf, delta, oldpfx, newpfx,
			"Binary files %s%s and %s%s differ\n") < 0)
	pi->line.origin      = GIT_DIFF_LINE_BINARY;
	pi->line.content     = git_buf_cstr(pi->buf);
	pi->line.content_len = git_buf_len(pi->buf);
	pi->line.num_lines   = 1;

	if (pi->print_cb(delta, NULL, &pi->line, pi->payload))
	const git_diff_hunk *h,
	pi->line.origin      = GIT_DIFF_LINE_HUNK_HDR;
	pi->line.content     = h->header;
	pi->line.content_len = h->header_len;
	if (pi->print_cb(d, h, &pi->line, pi->payload))
	const git_diff_hunk *hunk,
	const git_diff_line *line,
	if (pi->print_cb(delta, hunk, line, pi->payload))
/* print a git_diff to an output callback */
int git_diff_print(
	git_diff *diff,
	git_diff_format_t format,
	git_diff_line_cb print_cb,
	git_diff_file_cb print_file = NULL;
	git_diff_hunk_cb print_hunk = NULL;
	git_diff_line_cb print_line = NULL;

	switch (format) {
	case GIT_DIFF_FORMAT_PATCH:
		print_file = diff_print_patch_file;
		print_hunk = diff_print_patch_hunk;
		print_line = diff_print_patch_line;
		break;
	case GIT_DIFF_FORMAT_PATCH_HEADER:
		print_file = diff_print_patch_file;
		break;
	case GIT_DIFF_FORMAT_RAW:
		print_file = diff_print_one_raw;
		break;
	case GIT_DIFF_FORMAT_NAME_ONLY:
		print_file = diff_print_one_name_only;
		break;
	case GIT_DIFF_FORMAT_NAME_STATUS:
		print_file = diff_print_one_name_status;
		break;
	default:
		giterr_set(GITERR_INVALID, "Unknown diff output format (%d)", format);
		return -1;
	}
	if (!(error = diff_print_info_init(
			&pi, &buf, diff, format, print_cb, payload)))
			diff, print_file, print_hunk, print_line, &pi);
/* print a git_patch to an output callback */
int git_patch_print(
	git_patch *patch,
	git_diff_line_cb print_cb,
			&pi, &temp, git_patch__diff(patch),
			GIT_DIFF_FORMAT_PATCH, print_cb, payload)))
		error = git_patch__invoke_callbacks(
	const git_diff_hunk *hunk,
	const git_diff_line *line,
	GIT_UNUSED(delta); GIT_UNUSED(hunk);

	if (line->origin == GIT_DIFF_LINE_ADDITION ||
		line->origin == GIT_DIFF_LINE_DELETION ||
		line->origin == GIT_DIFF_LINE_CONTEXT)
		git_buf_putc(output, line->origin);

	return git_buf_put(output, line->content, line->content_len);
/* print a git_patch to a string buffer */
int git_patch_to_str(
	git_patch *patch)
	error = git_patch_print(patch, diff_print_to_buffer_cb, &output);