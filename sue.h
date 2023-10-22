#ifndef SUE_H
#define SUE_H
struct rule_t {
	int uid;
	int gid;
    const char* uname;
    const char* gname;
	const char *cmd;
    int password_needed;
};
#endif
