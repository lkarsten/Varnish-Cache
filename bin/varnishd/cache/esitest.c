#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cache.h"

#include "vdef.h" /* v_printflike */
#include "vsb.h"
#include "miniobj.h"

/* crc32 */
#include "vgz.h"

#include "stdint.h"  /* uint8_t for vct.h */
#include "vct.h"  /* */
#include "vend.h"  /* vbe32dec &co */

#include "cache_esi.h"

volatile struct params *cache_param;
struct VSC_C_main *VSC_C_main;

void
VSLb(struct vsl_log *vsl, enum VSL_tag_e tag, const char *fmt, ...)
{

	(void)vsl;
	(void)tag;
	(void)fmt;

	/*
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, fmt, ap);
	va_end(ap);
	*/
}

void *
WS_Alloc(struct ws *ws, unsigned bytes)
{
	(void)ws;
	return(malloc(bytes));
}

int main(int argc, char *argv[]) {
	FILE *input;

	if (argc == 1) {
		printf("Usage: esitest sample\n");
		exit(-2);
	}

	struct params cp = {};
	cache_param = &cp;

	(void)argc;
	struct VSC_C_main counters = {};
	VSC_C_main = &counters;

	input = fopen(argv[1], "r");
	if (input == NULL)
	    perror("fopen");

	struct stat st;
	(void)stat(argv[1], &st);

	char *buf = malloc(st.st_size + 1);
	AN(buf);

	(void)fread(buf, st.st_size, 1, input);
	// printf("buf[%lu]: %s\n", strlen(buf), buf);

	void *vp = Fake_VEP_Init();
	VEP_Parse(vp, buf, st.st_size);

	struct vsb *res;
	res = VEP_Finish(vp);
	if (res == NULL) {
		// assert(counters.esi_errors == 1);
		return(1);  /* No ESI found, or parse error. */
	}
	// printf("%s", VSB_data(res));
	return(0);  /* ESI found and parsed ok. */
}
