shell : ls.c fetch.c execute.c shell.c pinfo.c jobs.c builtin.c
	gcc -o shell ls.c fetch.c execute.c shell.c pinfo.c jobs.c builtin.c
