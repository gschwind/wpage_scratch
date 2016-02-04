/*
 * xdg_shell_surface.cxx
 *
 *  Created on: 25 sept. 2015
 *      Author: gschwind
 */

#include <wayland-server-core.h>
#include <wayland-server.h>
#include "xdg-shell-server-protocol.h"
#include "compositor.hxx"
#include "xdg_surface.hxx"
#include "xdg_shell.hxx"

namespace wpage {

using namespace std;

extern const struct xdg_surface_interface xdg_surface_implementation;


static void
move_grab_motion(struct weston_pointer_grab *grab,
		uint32_t time,
		wl_fixed_t x,
		wl_fixed_t y)
{
	struct weston_pointer *pointer = grab->pointer;

	weston_log("motion %f, %f\n",
			wl_fixed_to_double(grab->pointer->grab_x),
			wl_fixed_to_double(grab->pointer->grab_y));

	float fx = wl_fixed_to_double(x);
	float fy = wl_fixed_to_double(y);

	weston_log("xy %f, %f\n",
			wl_fixed_to_double(x),
			wl_fixed_to_double(y));

	weston_pointer_move(pointer, x, y);

	/** TODO: relative move **/
	weston_view_set_position(grab->pointer->focus, fx, fy);
	weston_compositor_schedule_repaint(grab->pointer->focus->surface->compositor);
}

static void
move_grab_button(struct weston_pointer_grab *grab,
		 uint32_t time, uint32_t button, uint32_t state_w)
{
	struct weston_pointer *pointer = grab->pointer;

	grab->pointer->focus;
	weston_log("move %f, %f\n",
			wl_fixed_to_double(grab->pointer->grab_x),
			wl_fixed_to_double(grab->pointer->grab_y));

	if (pointer->button_count == 0 &&
			state_w == WL_POINTER_BUTTON_STATE_RELEASED) {
		weston_pointer_end_grab(grab->pointer);
	}
}

static void
move_grab_cancel(struct weston_pointer_grab *grab)
{
	weston_pointer_end_grab(grab->pointer);
}

static void
move_grab_focus(struct weston_pointer_grab *grab)
{

}

static const struct weston_pointer_grab_interface move_grab_interface = {
	move_grab_focus,
	move_grab_motion,
	move_grab_button,
	move_grab_cancel,
};
xdg_surface_t * xdg_surface_t::get(wl_resource * resource) {
	return reinterpret_cast<xdg_surface_t*>(wl_resource_get_user_data(resource));
}

static void
shell_surface_configure(struct weston_surface * s, int32_t x , int32_t y) {
	weston_log("call %s\n", __PRETTY_FUNCTION__);
	weston_log("s = %p, x = %d, y = %d\n", s, x, y);

	auto sh = reinterpret_cast<xdg_surface_t*>(s->configure_private);

	weston_seat * seat;
	wl_list_for_each(seat, &sh->shell->cmp->wcmp->seat_list, link)
		weston_surface_activate(s, seat);

	if(s->output != nullptr)
		weston_output_schedule_repaint(s->output);
}

static void
xdg_surface_delete(struct wl_resource *resource)
{
	delete xdg_surface_t::get(resource);
}


xdg_surface_t::xdg_surface_t(wl_client *client, xdg_shell_t * shell, uint32_t id, weston_surface * surface) :
		client{client},
		shell{shell},
		id{id},
		surface{surface}
{

	resource = wl_resource_create(client, &xdg_surface_interface, 1, id);
	wl_resource_set_implementation(resource, &xdg_surface_implementation,
			this, xdg_surface_delete);

	/** tell weston how to use this data **/
	if (weston_surface_set_role(surface, "xdg_surface",
				    resource, XDG_SHELL_ERROR_ROLE) < 0)
		throw "TODO";

	/* the first output */
	auto output = container_of(shell->cmp->wcmp->output_list.next,
		    weston_output, link);

	surface->configure = &shell_surface_configure;
	surface->configure_private = this;

	surface->output = output;

	view = weston_view_create(surface);
	weston_view_set_position(view, 0, 0);
	surface->timeline.force_refresh = 1;
	weston_layer_entry_insert(&shell->cmp->default_layer.view_list, &view->layer_link);

	wl_array array;
	wl_array_init(&array);
	wl_array_add(&array, sizeof(uint32_t)*2);
	((uint32_t*)array.data)[0] = XDG_SURFACE_STATE_MAXIMIZED;
	((uint32_t*)array.data)[1] = XDG_SURFACE_STATE_ACTIVATED;
	xdg_surface_send_configure(resource, 300, 300, &array, 10);
	wl_array_release(&array);

	weston_view_geometry_dirty(view);

}


static void
xdg_surface_destroy(struct wl_client *client,
		    struct wl_resource *resource)
{
	wl_resource_destroy(resource);
}

static void
xdg_surface_set_parent(struct wl_client *client,
		       struct wl_resource *resource,
		       struct wl_resource *parent_resource)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_set_app_id(struct wl_client *client,
		       struct wl_resource *resource,
		       const char *app_id)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_show_window_menu(struct wl_client *client,
			     struct wl_resource *surface_resource,
			     struct wl_resource *seat_resource,
			     uint32_t serial,
			     int32_t x,
			     int32_t y)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_set_title(struct wl_client *client,
			struct wl_resource *resource, const char *title)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_move(struct wl_client *client, struct wl_resource *resource,
		 struct wl_resource *seat_resource, uint32_t serial)
{
	/* only one grab at time ? */
	static weston_pointer_grab grab_data{&move_grab_interface, nullptr};

	weston_log("call %s\n", __PRETTY_FUNCTION__);
	auto xdg_surface = xdg_surface_t::get(resource);
	auto seat = reinterpret_cast<weston_seat*>(wl_resource_get_user_data(seat_resource));

	weston_pointer_start_grab(seat->pointer, &grab_data);

}

static void
xdg_surface_resize(struct wl_client *client, struct wl_resource *resource,
		   struct wl_resource *seat_resource, uint32_t serial,
		   uint32_t edges)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_ack_configure(struct wl_client *client,
			  struct wl_resource *resource,
			  uint32_t serial)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_set_window_geometry(struct wl_client *client,
				struct wl_resource *resource,
				int32_t x,
				int32_t y,
				int32_t width,
				int32_t height)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_set_maximized(struct wl_client *client,
			  struct wl_resource *resource)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_unset_maximized(struct wl_client *client,
			    struct wl_resource *resource)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_set_fullscreen(struct wl_client *client,
			   struct wl_resource *resource,
			   struct wl_resource *output_resource)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_unset_fullscreen(struct wl_client *client,
			     struct wl_resource *resource)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

static void
xdg_surface_set_minimized(struct wl_client *client,
			    struct wl_resource *resource)
{
	weston_log("call %s\n", __PRETTY_FUNCTION__);
}

const struct xdg_surface_interface xdg_surface_implementation = {
	xdg_surface_destroy,
	xdg_surface_set_parent,
	xdg_surface_set_title,
	xdg_surface_set_app_id,
	xdg_surface_show_window_menu,
	xdg_surface_move,
	xdg_surface_resize,
	xdg_surface_ack_configure,
	xdg_surface_set_window_geometry,
	xdg_surface_set_maximized,
	xdg_surface_unset_maximized,
	xdg_surface_set_fullscreen,
	xdg_surface_unset_fullscreen,
	xdg_surface_set_minimized,
};



}


