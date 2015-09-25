/*
 * compositor.cxx
 *
 *  Created on: 25 sept. 2015
 *      Author: gschwind
 */

#include "compositor.hxx"

namespace wpage {

using namespace std;

compositor_t::compositor_t(wl_display * dpy) {
	/* initialise the compositor */
	wcmp = weston_compositor_create(dpy, NULL);
	weston_layer_init(&default_layer, &wcmp->cursor_layer.link);
}

}


