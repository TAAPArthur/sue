#include "sue.h"
#define NGROUPS_MAX 16

static struct rule_t rules[] = {
    { .gname="sys", .cmd = "reboot"},
    { .gname="sys", .cmd = "poweroff"},
    { .gname="sys", .cmd = "suspend"},
    { .gname="sys", .cmd = "hibernate"},
    // allow members of group sys to execute any program
    { .gname="sys", .cmd = "*"},
    // allow members of group wheel to execute any program with password
    { .gname="wheel", .cmd = "*", .password_needed = 1},
};
