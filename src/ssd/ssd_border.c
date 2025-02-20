// SPDX-License-Identifier: GPL-2.0-only

#include "common/scene-helpers.h"
#include "labwc.h"
#include "ssd-internal.h"
#include "theme.h"
#include "view.h"

#define FOR_EACH_STATE(ssd, tmp) FOR_EACH(tmp, \
	&(ssd)->border.active, \
	&(ssd)->border.inactive)

void
ssd_border_create(struct ssd *ssd)
{
	struct view *view = ssd->view;
	struct theme *theme = view->server->theme;
	int width = view->w;
	int height = view->h;
	int full_width = width + 2 * theme->border_width;

	float *color;
	struct wlr_scene_tree *parent;
	struct ssd_sub_tree *subtree;

	FOR_EACH_STATE(ssd, subtree) {
		subtree->tree = wlr_scene_tree_create(ssd->tree);
		parent = subtree->tree;
		wlr_scene_node_set_position(&parent->node, -theme->border_width, 0);
		if (subtree == &ssd->border.active) {
			color = theme->window_active_border_color;
		} else {
			color = theme->window_inactive_border_color;
			wlr_scene_node_set_enabled(&parent->node, false);
		}
		wl_list_init(&subtree->parts);
		add_scene_rect(&subtree->parts, LAB_SSD_PART_LEFT, parent,
			theme->border_width, height, 0, 0, color);
		add_scene_rect(&subtree->parts, LAB_SSD_PART_RIGHT, parent,
			theme->border_width, height,
			theme->border_width + width, 0, color);
		add_scene_rect(&subtree->parts, LAB_SSD_PART_BOTTOM, parent,
			full_width, theme->border_width, 0, height, color);
		add_scene_rect(&subtree->parts, LAB_SSD_PART_TOP, parent,
			width - 2 * BUTTON_WIDTH, theme->border_width,
			theme->border_width + BUTTON_WIDTH,
			-(theme->title_height + theme->border_width), color);
	} FOR_EACH_END
}

void
ssd_border_update(struct ssd *ssd)
{
	struct view *view = ssd->view;
	struct theme *theme = view->server->theme;

	int width = view->w;
	int height = view->h;
	int full_width = width + 2 * theme->border_width;

	struct ssd_part *part;
	struct wlr_scene_rect *rect;
	struct ssd_sub_tree *subtree;
	FOR_EACH_STATE(ssd, subtree) {
		wl_list_for_each(part, &subtree->parts, link) {
			rect = lab_wlr_scene_get_rect(part->node);
			switch (part->type) {
			case LAB_SSD_PART_LEFT:
				wlr_scene_rect_set_size(rect,
					theme->border_width, height);
				continue;
			case LAB_SSD_PART_RIGHT:
				wlr_scene_rect_set_size(rect,
					theme->border_width, height);
				wlr_scene_node_set_position(part->node,
					theme->border_width + width, 0);
				continue;
			case LAB_SSD_PART_BOTTOM:
				wlr_scene_rect_set_size(rect,
					full_width, theme->border_width);
				wlr_scene_node_set_position(part->node,
					0, height);
				continue;
			case LAB_SSD_PART_TOP:
				wlr_scene_rect_set_size(rect,
					width - 2 * BUTTON_WIDTH,
					theme->border_width);
				continue;
			default:
				continue;
			}
		}
	} FOR_EACH_END
}

void
ssd_border_destroy(struct ssd *ssd)
{
	if (!ssd->border.active.tree) {
		return;
	}

	struct ssd_sub_tree *subtree;
	FOR_EACH_STATE(ssd, subtree) {
		ssd_destroy_parts(&subtree->parts);
		wlr_scene_node_destroy(&subtree->tree->node);
		subtree->tree = NULL;
	} FOR_EACH_END
}

#undef FOR_EACH_STATE
