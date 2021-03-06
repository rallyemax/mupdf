#include "mupdf/fitz.h"

fz_outline *
fz_new_outline(fz_context *ctx)
{
	fz_outline *outline = fz_malloc_struct(ctx, fz_outline);
	outline->refs = 1;
	return outline;
}

fz_outline *
fz_keep_outline(fz_context *ctx, fz_outline *outline)
{
	return fz_keep_imp(ctx, outline, &outline->refs);
}

void
fz_drop_outline(fz_context *ctx, fz_outline *outline)
{
	while (fz_drop_imp(ctx, outline, &outline->refs))
	{
		fz_outline *next = outline->next;
		fz_drop_outline(ctx, outline->down);
		fz_free(ctx, outline->title);
		fz_free(ctx, outline->uri);
		fz_free(ctx, outline);
		outline = next;
	}
}

static void
fz_debug_outline_xml_imp(fz_context *ctx, fz_output *out, fz_outline *outline, int level)
{
	while (outline)
	{
		fz_printf(ctx, out, "<outline title=%q uri=\"%s\"", outline->title, outline->uri);
		if (outline->down)
		{
			fz_printf(ctx, out, ">\n");
			fz_debug_outline_xml_imp(ctx, out, outline->down, level + 1);
			fz_printf(ctx, out, "</outline>\n");
		}
		else
		{
			fz_printf(ctx, out, " />\n");
		}
		outline = outline->next;
	}
}

void
fz_print_outline_xml(fz_context *ctx, fz_output *out, fz_outline *outline)
{
	fz_debug_outline_xml_imp(ctx, out, outline, 0);
}

static void
fz_print_outline_imp(fz_context *ctx, fz_output *out, fz_outline *outline, int level)
{
	int i;
	while (outline)
	{
		for (i = 0; i < level; i++)
			fz_printf(ctx, out, "\t");
		fz_printf(ctx, out, "%s\t%s\n", outline->title, outline->uri);
		if (outline->down)
			fz_print_outline_imp(ctx, out, outline->down, level + 1);
		outline = outline->next;
	}
}

void
fz_print_outline(fz_context *ctx, fz_output *out, fz_outline *outline)
{
	fz_print_outline_imp(ctx, out, outline, 0);
}
