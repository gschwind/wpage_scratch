/*
 * xdg_shell.hxx
 *
 *  Created on: 25 sept. 2015
 *      Author: gschwind
 */

#ifndef SRC_XDG_SHELL_HXX_
#define SRC_XDG_SHELL_HXX_

#include <wayland-server.h>
#include <compositor.h>

#include <list>

#include "type.hxx"

#include "xdg-shell-server-protocol.h"

namespace wpage {

using namespace std;

struct xdg_shell_t {
	compositor_t * cmp;
	wl_client * client;
	uint32_t id;

	wl_resource * resource;

	list<wl_resource *> xdg_shell_surfaces;

	xdg_shell_t(wl_client * client, compositor_t * data, uint32_t id);

	static xdg_shell_t * get(wl_resource * resource);

};

extern const struct xdg_shell_interface xdg_shell_implementation;

}



#endif /* SRC_XDG_SHELL_HXX_ */
