#include <grp.h>
#include <pwd.h>
#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sue.h"
#include "config.h"

#define LEN(A) (sizeof(A)/sizeof(A[0]))

static int die(int ret, const char *org, const char *str) {
	fprintf (stderr, "%s%s%s\n", org?org:"", org?": ":"", str);
	return ret;
}

// based of the helper function in ubase
int pw_check(const struct passwd *pw, const char *pass)
{
	char *cryptpass, *p;
	struct spwd *spw;

	p = pw->pw_passwd;
	if (p[0] == '!' || p[0] == '*') {
		printf("Account disabled\n");
		return -2;
	}

	if (pw->pw_passwd[0] == '\0' && pass[0] == '\0') {
        return 0;
	}

	if (pw->pw_passwd[0] == 'x' && pw->pw_passwd[1] == '\0') {
		spw = getspnam(pw->pw_name);
		if (!spw || spw->sp_pwdp[0] == '!' || spw->sp_pwdp[0] == '*') {
            printf("Account disabled\n");
			return -2;
		}
		p = spw->sp_pwdp;
	}

	cryptpass = crypt(pass, p);
	if (!cryptpass) {
		perror("crypt:");
		return -3;
	}
	if (strcmp(cryptpass, p) != 0) {
        printf("Incorrect password\n");
		return -1;
	}
	return 0;
}

int isInGroup(struct passwd *pw, const char* target_group) {
	gid_t groups[NGROUPS_MAX];
	int ngroups;

	ngroups = NGROUPS_MAX;
	getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups);
    for (int i = 0; i < ngroups; i++) {
        struct group *group = getgrgid(groups[i]);
        if(strcmp(target_group, group->gr_name) == 0)
            return 1;
    }
    return 0;
}

void usage(int exitCode) {
    printf("sue [-u user] [-g user] cmd args\n");
    exit(exitCode);
}
int main(int argc, char **argv) {
    argv++;
    struct passwd* pw = NULL;
    uid_t target_uid = 0;
    gid_t target_gid = 0;
    for(; argv[0]; argv++){
        if(argv[0][0] != '-')
            break;
        if(argv[0][1] == '-') {
            argv++;
            break;
        }
        switch(argv[0][1]) {
            case 'h':
                usage(0);
                break;
            case 'l':
                for (int i = 0; i < LEN(rules); i++)
                    printf ("%d %d %10s %d\n", rules[i].uid, rules[i].gid, rules[i].cmd, rules[i].no_strict_path);
                exit(0);
            case 'u':
            case 'g':
                if(!argv[1])
                    usage(1);
                pw = getpwnam(argv[1]);
                if(argv[0][1] == 'u')
                    target_uid = pw->pw_uid;
                else
                    target_gid = pw->pw_gid;
                argv++;
                break;
        }
    }
    if(!argv[0])
        usage(1);
    const char* cmd = argv[0];

	uid_t uid = getuid();
    gid_t gid = getgid();
    pw = getpwuid(uid);

	for (int i = 0; i < LEN(rules); i++) {
		if (rules[i].cmd[0] == '*' || !strcmp(cmd, rules[i].cmd)) {
            if ((uid == target_uid || rules[i].uid == uid || !rules[i].uid && !rules[i].uname || rules[i].uname && strcmp(pw->pw_name, rules[i].uname)) &&
                (gid == target_gid || rules[i].gid == gid || !rules[i].gid && !rules[i].gname || rules[i].gname && isInGroup(pw, rules[i].gname))) {
                if(!rules[i].no_strict_path) {
                    if(strstr(cmd, "/")) {
                        continue;
                    }
                    setenv("PATH", "/bin:/usr/bin", 1);
                }
                if(rules[i].password_needed) {
                    if (pw_check(pw, getpass("Password: ")) < 0)
                        exit(1);
                }

                if (setgid (target_gid) == -1 || setegid (target_gid) == -1 ||
                    setuid (target_uid) == -1 || seteuid (target_uid) == -1 ) {
                    perror("Failed to set gid/uid");
                    exit(2);
                }
                execvp(cmd, argv);
                perror("Failed execv");
                exit(2);
            }
        }
    }
    exit(255);
}
