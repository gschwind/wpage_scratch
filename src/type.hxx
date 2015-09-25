/*
 * type.hxx
 *
 *  Created on: 25 sept. 2015
 *      Author: gschwind
 */

#ifndef SRC_TYPE_HXX_
#define SRC_TYPE_HXX_

/**
 * Returns a pointer the the containing struct of a given member item.
 *
 * To demonstrate, the following example retrieves a pointer to
 * `example_container` given only its `destroy_listener` member:
 *
 * @code
 * struct example_container {
 *     struct wl_listener destroy_listener;
 *     // other members...
 * };
 *
 * void example_container_destroy(struct wl_listener *listener, void *data)
 * {
 *     struct example_container *ctr;
 *
 *     ctr = wl_container_of(listener, ctr, destroy_listener);
 *     // destroy ctr...
 * }
 * @endcode
 *
 * @param ptr A valid pointer to the contained item.
 *
 * @param type A pointer to the type of content that the list item
 * stores. Type does not need be a valid pointer; a null or
 * an uninitialised pointer will suffice.
 *
 * @param member The named location of ptr within the sample type.
 *
 * @return The container for the specified pointer.
 */
#ifndef container_of
#define container_of(ptr, type, member) ({				\
	const __typeof__( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

namespace wpage {

struct compositor_t;
struct client_t;
struct xdg_shell_t;
struct xdg_surface_t;

}

#endif /* SRC_TYPE_HXX_ */
