#ifndef LOXGUARD_PROFILES_H
#define LOXGUARD_PROFILES_H

/* Descriptive profile labels only; they do not gate source files or stable APIs. */
#define LOXGUARD_PROFILE_MINIMAL 1
#define LOXGUARD_PROFILE_BLACKBOX 2
#define LOXGUARD_PROFILE_RECOVERY 3
#define LOXGUARD_PROFILE_FULL 4

#ifndef LOXGUARD_PROFILE
#define LOXGUARD_PROFILE LOXGUARD_PROFILE_MINIMAL
#endif

#endif
