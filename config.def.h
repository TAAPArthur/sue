#define SETUID 0
#define SETGID 0

#define NGROUPS_MAX 16

static struct rule_t rules[] = {
    { .gname="sys", .cmd = "reboot"},
    { .gname="sys", .cmd = "poweroff"},
    { .gname="sys", .cmd = "suspend"},
    { .gname="sys", .cmd = "hibernate"},
    // allow members of group sys to execute any program in default PATH
    { .gname="sys", .cmd = "*"},
    // allow members of group sys to execute any program with password
    { .gname="sys", .cmd = "*", .no_strict_path=1, .password_needed = 1},
    // allow members of group wheel to execute any program in default PATH
    { .gname="wheel", .cmd = "*", .no_strict_path=1, .password_needed = 1},
};
