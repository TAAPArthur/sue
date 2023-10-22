#ifndef SUE2_H
#define SUE2_H
struct rule_t {
	int uid;
    const char* uname;
	int gid;
    const char* gname;
	const char *cmd;
    int password_needed;
};
#endif
