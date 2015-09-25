/*
 * xdg_popup.hxx
 *
 *  Created on: 25 sept. 2015
 *      Author: gschwind
 */

#ifndef SRC_XDG_POPUP_HXX_
#define SRC_XDG_POPUP_HXX_

#include <wayland-server.h>
#include <compositor.h>

#include <list>

#include "xdg-shell-server-protocol.h"

#include "type.hxx"

namespace wpage {

using namespace std;

struct xdg_popup_t {
	wl_client *client;
	xdg_shell_t * shell;
	uint32_t id;
	weston_surface * surface;

	/* handle the wayland resource related to this xdg_surface */
	wl_resource * resource;
	weston_view * view;

	xdg_popup_t(wl_client *client, xdg_shell_t * shell, uint32_t id, weston_surface * surface);

	static xdg_popup_t * get(wl_resource * resource);

};

}





#endif /* SRC_XDG_POPUP_HXX_ */
