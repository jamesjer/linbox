/* -*- mode: c; style: linux -*- */

/* renderer.c
 * Copyright (C) 2000 Helix Code, Inc.
 *
 * Written by Bradford Hovinen <hovinen@helixcode.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "renderer.h"

enum {
	ARG_0,
	ARG_SAMPLE
};

struct _RendererPrivate 
{
	/* Private data members */
};

static GtkObjectClass *parent_class;

static void renderer_init        (Renderer *renderer);
static void renderer_class_init  (RendererClass *class);

static void renderer_set_arg     (GtkObject *object, 
					   GtkArg *arg, 
					   guint arg_id);
static void renderer_get_arg     (GtkObject *object, 
					   GtkArg *arg, 
					   guint arg_id);

static void renderer_finalize    (GtkObject *object);

guint
renderer_get_type (void)
{
	static guint renderer_type = 0;

	if (!renderer_type) {
		GtkTypeInfo renderer_info = {
			"Renderer",
			sizeof (Renderer),
			sizeof (RendererClass),
			(GtkClassInitFunc) renderer_class_init,
			(GtkObjectInitFunc) renderer_init,
			(GtkArgSetFunc) NULL,
			(GtkArgGetFunc) NULL
		};

		renderer_type = 
			gtk_type_unique (gtk_object_get_type (), 
					 &renderer_info);
	}

	return renderer_type;
}

static void
renderer_init (Renderer *renderer)
{
	renderer->p = g_new0 (RendererPrivate, 1);
}

static void
renderer_class_init (RendererClass *class) 
{
	GtkObjectClass *object_class;

	gtk_object_add_arg_type ("Renderer::sample",
				 GTK_TYPE_POINTER,
				 GTK_ARG_READWRITE,
				 ARG_SAMPLE);

	object_class = GTK_OBJECT_CLASS (class);
	object_class->finalize = renderer_finalize;
	object_class->set_arg = renderer_set_arg;
	object_class->get_arg = renderer_get_arg;

	parent_class = GTK_OBJECT_CLASS
		(gtk_type_class (gtk_object_get_type ()));
}

static void
renderer_set_arg (GtkObject *object, GtkArg *arg, guint arg_id) 
{
	Renderer *renderer;

	g_return_if_fail (object != NULL);
	g_return_if_fail (IS_RENDERER (object));

	renderer = RENDERER (object);

	switch (arg_id) {
	case ARG_SAMPLE:
		break;

	default:
		g_warning ("Bad argument set");
		break;
	}
}

static void
renderer_get_arg (GtkObject *object, GtkArg *arg, guint arg_id) 
{
	Renderer *renderer;

	g_return_if_fail (object != NULL);
	g_return_if_fail (IS_RENDERER (object));

	renderer = RENDERER (object);

	switch (arg_id) {
	case ARG_SAMPLE:
		break;

	default:
		g_warning ("Bad argument get");
		break;
	}
}

static void
renderer_finalize (GtkObject *object) 
{
	Renderer *renderer;

	g_return_if_fail (object != NULL);
	g_return_if_fail (IS_RENDERER (object));

	renderer = RENDERER (object);

	g_free (renderer->p);
}

GtkObject *
renderer_new (void) 
{
	return gtk_object_new (renderer_get_type (),
			       NULL);
}
