
#ifndef TRACE_ITEM_H
#define TRACE_ITEM_H

// this is tpts
enum trace_item_type {
	ti_NOP = 0,

	ti_RTYPE,
	ti_ITYPE,
	ti_LOAD,
	ti_STORE,
	ti_BRANCH,
	ti_JTYPE,
	ti_SPECIAL,
	ti_JRTYPE
};

struct trace_item {
	unsigned char type;			// see above
	unsigned char sReg_a;			// 1st operand
	unsigned char sReg_b;			// 2nd operand
	unsigned char dReg;			// dest. operand
	unsigned int PC;			// program counter
	unsigned int Addr;			// mem. address
};

#endif

#define TRACE_BUFSIZE 1024*1024

static FILE *trace_fd;
static int trace_buf_ptr;
static int trace_buf_end;
static struct trace_item *trace_buf;

int is_big_endian(void)
{
	union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };

	return bint.c[0] == 1;
}

uint32_t my_ntohl(uint32_t x)
{
	u_char *s = (u_char *)&x;
	return (uint32_t)(s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0]);
}

void trace_init()
{
	trace_buf = malloc(sizeof(struct trace_item) * TRACE_BUFSIZE);

	if (!trace_buf) {
		fprintf(stdout, "** trace_buf not allocated\n");
		exit(-1);
	}

	trace_buf_ptr = 0;
	trace_buf_end = 0;
}

void trace_uninit()
{
	free(trace_buf);
	fclose(trace_fd);
}

int trace_get_item(struct trace_item **item)
{
	int n_items;

	if (trace_buf_ptr == trace_buf_end) {	/* if no more unprocessed items in the trace buffer, get new data  */
		n_items = fread(trace_buf, sizeof(struct trace_item), TRACE_BUFSIZE, trace_fd);
		if (!n_items) return 0;				/* if no more items in the file, we are done */

		trace_buf_ptr = 0;
		trace_buf_end = n_items;			/* n_items were read and placed in trace buffer */
	}

	*item = &trace_buf[trace_buf_ptr];	/* read a new trace item for processing */
	trace_buf_ptr++;

	if (is_big_endian()) {
		(*item)->PC = my_ntohl((*item)->PC);
		(*item)->Addr = my_ntohl((*item)->Addr);
	}

	return 1;
}