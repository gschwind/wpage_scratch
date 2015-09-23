/*
 * page.cxx
 *
 * copyright (2015) Benoit Gschwind
 *
 * This code is licensed under the GPLv3. see COPYING file for more details.
 *
 */

#include <cstdio>

#include <poll.h>


#include <wayland-server.h>
#include <compositor.h>
#include <compositor-x11.h>
#include <compositor-drm.h>


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

int main(int argc, char ** argv) {
	struct callbacks cbk;

    weston_log_set_handler(vprintf, vprintf);

	/* first create the wayland serveur */
	wl_display * dpy = wl_display_create();

	/* initialise the compositor */
	weston_compositor * cmp = weston_compositor_create(dpy, NULL);

	/* setup the keyboard layout (MANDATORY) */
	xkb_rule_names names = {
			/*rules*/"",
			/*model*/"pc104",
			/*layout*/"us",
			/*variant*/"",
			/*option*/""
	};
	weston_compositor_xkb_init(cmp, &names);

	/* define x1 backend configuration */
	weston_x11_backend_config conf = { { }, 0 };
	conf.use_pixman = 0;

	/* initialize the x11-backend */
	weston_compositor_init_backend(cmp, "x11-backend.so", (struct weston_backend_config*)&conf);


    wl_signal_add(&cmp->destroy_signal, &cbk.destroy);
    cbk.destroy.notify = [](wl_listener *l, void *data) { weston_log("destroy\n"); };

    wl_signal_add(&cmp->output_moved_signal, &cbk.output_moved);
    cbk.output_moved.notify = [](wl_listener *l, void *data) { weston_log("output_moved\n"); };

    wl_signal_add(&cmp->output_created_signal, &cbk.output_created);
    cbk.output_created.notify = [](wl_listener *l, void *data) { weston_log("output_created\n"); };

    wl_signal_add(&cmp->session_signal, &cbk.session);
    cbk.session.notify = [](wl_listener *l, void *data) { weston_log("session\n"); };

    wl_signal_add(&cmp->seat_created_signal, &cbk.seat_created);
    cbk.seat_created.notify = [](wl_listener *l, void *data) { weston_log("seat_created\n"); };

    wl_event_loop * loop = wl_display_get_event_loop(dpy);
    int fd = wl_event_loop_get_fd(loop);

	pollfd poll_list;
	poll_list.fd = fd;
	poll_list.events = POLLIN|POLLOUT|POLLERR;


    bool running = true;
    while(running) {
    	poll(&poll_list, 1, 0);
        wl_event_loop_dispatch(loop, 0);
        wl_display_flush_clients(dpy);
    }

}

