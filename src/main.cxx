/*
 * page.cxx
 *
 * copyright (2015) Benoit Gschwind
 *
 * This code is licensed under the GPLv3. see COPYING file for more details.
 *
 */

#include <cstdio>
#include <cstdlib>

#include <poll.h>
#include <wayland-server.h>

#include <compositor.h>
#include <compositor-x11.h>
#include <compositor-drm.h>

#include <list>

#include "xdg-shell-server-protocol.h"

#include "client.hxx"
#include "xdg_shell.hxx"
#include "compositor.hxx"


using namespace std;
using namespace wpage;

struct callbacks {
	wl_listener destroy;

	/* surface signals */
	wl_listener create_surface;
	wl_listener activate;
	wl_listener transform;

	wl_listener kill;
	wl_listener idle;
	wl_listener wake;

	wl_listener show_input_panel;
	wl_listener hide_input_panel;
	wl_listener update_input_panel;

	wl_listener seat_created;
	wl_listener output_created;
	wl_listener output_destroyed;
	wl_listener output_moved;

	wl_listener session;

};

struct output_callback {
	wl_listener frame;
	wl_listener destroy;
};

wl_global_bind_func_t test;

static void bind_xdg_shell(
		wl_client * client,
		void * data,
		uint32_t version,
		uint32_t id) {
	weston_log("bind_xdg_shell\n");

	auto cmp = reinterpret_cast<compositor_t*>(data);
	/* this resources will be in the weston tree */
	new xdg_shell_t(client, cmp, id);

}

struct globals_t {
	list<client_t *> client_list;
};

int main(int argc, char ** argv) {
	struct callbacks cbk;
	output_callback ocbk;

	globals_t g;

	weston_layer default_layer;

    weston_log_set_handler(vprintf, vprintf);

	/* first create the wayland serveur */
	auto dpy = wl_display_create();

	auto sock_name = wl_display_add_socket_auto(dpy);
	weston_log("socket name = %s\n", sock_name);
	setenv("WAYLAND_DISPLAY", sock_name, 1);

	compositor_t cmp{dpy};

	/* setup the keyboard layout (MANDATORY) */
	xkb_rule_names names = {
			/*rules*/"",
			/*model*/"pc104",
			/*layout*/"us",
			/*variant*/"",
			/*option*/""
	};
	weston_compositor_xkb_init(cmp.wcmp, &names);

	/* define x1 backend configuration */
	weston_x11_backend_config conf;
	conf.use_pixman = 0;
	conf.no_input = 0;

	/* initialize the x11-backend */
	weston_compositor_init_backend(cmp.wcmp, "x11-backend.so", &conf.base);

	weston_x11_backend_output_config output_config = { { WL_OUTPUT_TRANSFORM_NORMAL, 800, 600, 1 }, 0 };

	weston_output * output = cmp.wcmp->backend->create_output(cmp.wcmp, NULL, &output_config.base);

    wl_signal_add(&output->frame_signal, &ocbk.frame);
    ocbk.frame.notify = [](wl_listener *l, void *data) { /*weston_log("output.frame\n");*/ };

    wl_signal_add(&output->destroy_signal, &ocbk.destroy);
    ocbk.destroy.notify = [](wl_listener *l, void *data) { weston_log("output.destroy\n"); };

    wl_signal_add(&cmp.wcmp->destroy_signal, &cbk.destroy);
    cbk.destroy.notify = [](wl_listener *l, void *data) { weston_log("destroy\n"); };

    wl_signal_add(&cmp.wcmp->output_moved_signal, &cbk.output_moved);
    cbk.output_moved.notify = [](wl_listener *l, void *data) { weston_log("output_moved\n"); };

    wl_signal_add(&cmp.wcmp->output_created_signal, &cbk.output_created);
    cbk.output_created.notify = [](wl_listener *l, void *data) { weston_log("output_created\n"); };

    wl_signal_add(&cmp.wcmp->session_signal, &cbk.session);
    cbk.session.notify = [](wl_listener *l, void *data) { weston_log("session\n"); };

    wl_signal_add(&cmp.wcmp->seat_created_signal, &cbk.seat_created);
    cbk.seat_created.notify = [](wl_listener *l, void *data) { weston_log("seat_created\n"); };

    wl_signal_add(&cmp.wcmp->create_surface_signal, &cbk.create_surface);
    cbk.create_surface.notify = [](wl_listener *l, void *data) { weston_log("create_surface\n"); };


	if (wl_global_create(dpy, &wl_shell_interface, 1,
				  &cmp, [](wl_client * client, void * data, uint32_t version, uint32_t id){ weston_log("bind wl_shell_interface\n");  }) == NULL)
		return -1;

	if (wl_global_create(dpy, &xdg_shell_interface, 1,
				  &cmp, bind_xdg_shell) == NULL)
		return -1;

	/** create the solid color background **/
	auto background = weston_surface_create(cmp.wcmp);
	weston_surface_set_color(background, 0.5, 0.5, 0.5, 1.0);
    weston_surface_set_size(background, 800, 600);
    pixman_region32_fini(&background->opaque);
    pixman_region32_init_rect(&background->opaque, 0, 0, 800, 600);
    weston_surface_damage(background);
    //pixman_region32_fini(&s->input);
    //pixman_region32_init_rect(&s->input, 0, 0, w, h);

    auto bview = weston_view_create(background);
	weston_view_set_position(bview, 0, 0);
	background->timeline.force_refresh = 1;
	weston_layer_entry_insert(&cmp.default_layer.view_list, &bview->layer_link);

    wl_display_run(dpy);

}

