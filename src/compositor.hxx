/*
 * compositor.hxx
 *
 *  Created on: 25 sept. 2015
 *      Author: gschwind
 */

#ifndef SRC_COMPOSITOR_HXX_
#define SRC_COMPOSITOR_HXX_

#include <wayland-server.h>
#include <compositor.h>

#include <list>

#include "xdg-shell-server-protocol.h"

#include "type.hxx"

namespace wpage {

using namespace std;

struct compositor_t {
	weston_compositor * wcmp;
	weston_layer default_layer;
	compositor_t(wl_display * dpy);

};

}



#endif /* SRC_COMPOSITOR_HXX_ */
