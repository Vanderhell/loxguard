#ifndef LOXGUARD_EXPERIMENTAL_H
#define LOXGUARD_EXPERIMENTAL_H

/*
 * Experimental API surface.
 *
 * Including this header acknowledges that the APIs pulled in here are not part
 * of the stable v1.0.0 contract, are not production support claims, and may
 * change in any 1.x release.
 */

#ifndef LOXGUARD_EXPERIMENTAL
#define LOXGUARD_EXPERIMENTAL 1
#endif

#include "loxguard_adapters.h"
#include "loxguard_backends.h"
#include "loxguard_ports.h"
#include "loxguard_rtos_bridge.h"
#include "loxguard_shell.h"

#endif
