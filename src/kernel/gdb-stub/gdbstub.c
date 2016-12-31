/**
 * Copyright (C) 2016  tcz717
 * 
 * This file is part of teos.
 * 
 * teos is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * teos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with teos.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <stdint.h>
#include <stddef.h>

#include <teos.h>
#include <kernel/interrupt.h>
#include <kernel/debug.h>
#include <kernel/gdb.h>

#define gdb_is_printable_char(ch) \
	 ((ch) >= 0x20 && (ch) <= 0x7e)

/* System functions, supported by all stubs */

int gdb_sys_getc(void);
int gdb_sys_putchar(int ch);
int gdb_sys_mem_readb(uint32_t addr, uint8_t *val);
int gdb_sys_mem_writeb(uint32_t addr, uint8_t val);
int gdb_sys_continue();
int gdb_sys_step();
void gdb_set_trap();

const char digits[] = "0123456789abcdef";

typedef int (*gdb_enc_func)(char *buf, size_t buf_len, const char *data, size_t data_len);
typedef int (*gdb_dec_func)(const char *buf, size_t buf_len, char *data, size_t data_len);

/*
 * Get the corresponding ASCII hex digit character for a value.
 */
char gdb_get_digit(int32_t val)
{
	if ((val >= 0) && (val <= 0xf)) {
		return digits[val];
	} else {
		return TEOS_EOF;
	}
}

/*
 * Get the corresponding value for a ASCII digit character.
 *
 * Supports bases 2-16.
 */
int gdb_get_val(char digit, int base)
{
	int value;

	if ((digit >= '0') && (digit <= '9')) {
		value = digit-'0';
	} else if ((digit >= 'a') && (digit <= 'f')) {
		value = digit-'a'+0xa;
	} else if ((digit >= 'A') && (digit <= 'F')) {
		value = digit-'A'+0xa;
	} else {
		return TEOS_EOF;
	}

	return (value < base) ? value : TEOS_EOF;
}

/*
 * Get integer value for a string representation.
 *
 * If the string starts with + or -, it will be signed accordingly.
 *
 * If base == 0, the base will be determined:
 *   base 16 if the string starts with 0x or 0X,
 *   base 10 otherwise
 *
 * If endptr is specified, it will point to the last non-digit in the
 * string. If there are no digits in the string, it will be set to NULL.
 */
int gdb_strtol(const char *str, size_t len, int base, const char **endptr)
{
	size_t pos;
	int sign, tmp, value, valid;

	value = 0;
	pos   = 0;
	sign  = 1;
	valid = 0;

	if (endptr) {
		*endptr = TEOS_NULL;
	}

	if (len < 1) {
		return 0;
	}

	/* Detect negative numbers */
	if (str[pos] == '-') {
		sign = -1;
		pos += 1;
	} else if (str[pos] == '+') {
		sign = 1;
		pos += 1;
	}

	/* Detect '0x' hex prefix */
	if ((pos + 2 < len) && (str[pos] == '0') &&
		((str[pos+1] == 'x') || (str[pos+1] == 'X'))) {
		base = 16;
		pos += 2;
	}

	if (base == 0) {
		base = 10;
	}

	for (; (pos < len) && (str[pos] != '\x00'); pos++) {
		tmp = gdb_get_val(str[pos], base);
		if (tmp == TEOS_EOF) {
			break;
		}

		value = value*base + tmp;
		valid = 1; /* At least one digit is valid */
	}

	if (!valid) {
		return 0;
	}

	if (endptr) {
		*endptr = str+pos;
	}

	value *= sign;

	return value;
}

/*
 * Calculate 8-bit checksum of a buffer.
 *
 * Returns:
 *    8-bit checksum.
 */
int gdb_checksum(const char *buf, size_t len)
{
	char csum;

	csum = 0;

	while (len--) {
		csum += *buf++;
	}

	return csum;
}

/*
 * Encode data to its hex-value representation in a buffer.
 *
 * Returns:
 *    0+  number of bytes written to buf
 *    TEOS_EOF if the buffer is too small
 */
int gdb_enc_hex(char *buf, size_t buf_len, const char *data, size_t data_len)
{
	size_t pos;

	if (buf_len < data_len*2) {
		/* Buffer too small */
		return TEOS_EOF;
	}

	for (pos = 0; pos < data_len; pos++) {
		*buf++ = gdb_get_digit((data[pos] >> 4) & 0xf);
		*buf++ = gdb_get_digit((data[pos]     ) & 0xf);
	}

	return data_len*2;
}

/*
 * Decode data from its hex-value representation to a buffer.
 *
 * Returns:
 *    0   if successful
 *    TEOS_EOF if the buffer is too small
 */
int gdb_dec_hex(const char *buf, size_t buf_len, char *data, size_t data_len)
{
	size_t pos;
	int tmp;

	if (buf_len != data_len*2) {
		/* Buffer too small */
		return TEOS_EOF;
	}

	for (pos = 0; pos < data_len; pos++) {
		/* Decode high nibble */
		tmp = gdb_get_val(*buf++, 16);
		if (tmp == TEOS_EOF) {
			/* Buffer contained junk. */
			tassert(0);
			return TEOS_EOF;
		}

		data[pos] = tmp << 4;

		/* Decode low nibble */
		tmp = gdb_get_val(*buf++, 16);
		if (tmp == TEOS_EOF) {
			/* Buffer contained junk. */
			tassert(0);
			return TEOS_EOF;
		}
		data[pos] |= tmp;
	}

	return 0;
}

/*
 * Write a sequence of bytes.
 *
 * Returns:
 *    0   if successful
 *    TEOS_EOF if failed to write all bytes
 */
int gdb_write(const char *buf, size_t len)
{
	while (len--) {
		if (gdb_sys_putchar(*buf++) == TEOS_EOF) {
			return TEOS_EOF;
		}
	}

	return 0;
}

/*
 * Read a sequence of bytes.
 *
 * Returns:
 *    0   if successfully read len bytes
 *    TEOS_EOF if failed to read all bytes
 */
int gdb_read(char *buf, size_t buf_len, size_t len)
{
	char c;

	if (buf_len < len) {
		/* Buffer too small */
		return TEOS_EOF;
	}

	while (len--) {
        if ((c = (char)gdb_sys_getc()) == (char)TEOS_EOF) {
			return TEOS_EOF;
		}
		*buf++ = c;
	}

	return 0;
}

/*
 * Receive a packet acknowledgment
 *
 * Returns:
 *    0   if an ACK (+) was received
 *    1   if a NACK (-) was received
 *    TEOS_EOF otherwise
 */
int gdb_recv_ack(void)
{
	int response;

	/* Wait for packet ack */
	switch (response = gdb_sys_getc()) {
	case '+':
		/* Packet acknowledged */
		return 0;
	case '-':
		/* Packet negative acknowledged */
		return 1;
	default:
		/* Bad response! */
		printd("received bad packet response: 0x%2x\n", response);
		return TEOS_EOF;
	}
}

/*
 * Receives a packet of data, assuming a 7-bit clean connection.
 *
 * Returns:
 *    0   if the packet was received
 *    TEOS_EOF otherwise
 */
int gdb_recv_packet(char *pkt_buf, size_t pkt_buf_len, size_t *pkt_len)
{
	int data;
	char expected_csum, actual_csum;
	char buf[2];

	/* Wait for packet start */
	actual_csum = 0;

	while (1) {
		data = gdb_sys_getc();
		if (data == '$') {
			/* Detected start of packet. */
			break;
		}
	}

	/* Read until checksum */
	*pkt_len = 0;
	while (1) {
		data = gdb_sys_getc();

		if (data == TEOS_EOF) {
			/* Error receiving character */
			return TEOS_EOF;
		} else if (data == '#') {
			/* End of packet */
			break;
		} else {
			/* Check for space */
			if (*pkt_len >= pkt_buf_len) {
				printd("packet buffer overflow\n");
				return TEOS_EOF;
			}

			/* Store character and update checksum */
			pkt_buf[(*pkt_len)++] = (char) data;
		}
	}

#if DEBUG_GDB
	{
		size_t p;
		printd("<- ");
		for (p = 0; p < *pkt_len; p++) {
			if (gdb_is_printable_char(pkt_buf[p])) {
				printd("%c", pkt_buf[p]);
			} else {
				printd("\\x%02x", pkt_buf[p] & 0xff);
			}
		}
		printd("\n");
	}
#endif

	/* Receive the checksum */
	if ((gdb_read(buf, sizeof(buf), 2) == TEOS_EOF) ||
		(gdb_dec_hex(buf, 2, &expected_csum, 1) == TEOS_EOF)) {
		return TEOS_EOF;
	}

	/* Verify checksum */
	actual_csum = gdb_checksum(pkt_buf, *pkt_len);
	if (actual_csum != expected_csum) {
		/* Send packet nack */
		printd("received packet with bad checksum\n");
		gdb_sys_putchar('-');
		return TEOS_EOF;
	}

	/* Send packet ack */
	gdb_sys_putchar('+');
	return 0;
}

/*
 * Transmits a packet of data.
 * Packets are of the form: $<packet-data>#<checksum>
 *
 * Returns:
 *    0   if the packet was transmitted and acknowledged
 *    1   if the packet was transmitted but not acknowledged
 *    TEOS_EOF otherwise
 */
int gdb_send_packet(const char *pkt_data, size_t pkt_len)
{
	char buf[3];
	char csum;

	/* Send packet start */
	if (gdb_sys_putchar('$') == TEOS_EOF) {
		return TEOS_EOF;
	}

#if DEBUG_GDB
	{
		size_t p;
		printd("-> ");
		for (p = 0; p < pkt_len; p++) {
			if (gdb_is_printable_char(pkt_data[p])) {
				printd("%c", pkt_data[p]);
			} else {
				printd("\\x%02x", pkt_data[p]&0xff);
			}
		}
		printd("\n");
	}
#endif

	/* Send packet data */
	if (gdb_write(pkt_data, pkt_len) == TEOS_EOF) {
		return TEOS_EOF;
	}

	/* Send the checksum */
	buf[0] = '#';
	csum = gdb_checksum(pkt_data, pkt_len);
	if ((gdb_enc_hex(buf+1, sizeof(buf)-1, &csum, 1) == TEOS_EOF) ||
		(gdb_write(buf, sizeof(buf)) == TEOS_EOF)) {
		return TEOS_EOF;
	}

	return gdb_recv_ack();
}

/*
 * Send OK packet
 */
int gdb_send_ok_packet()
{
	return gdb_send_packet("OK", 2);
}

/*
 * Send a signal packet (S AA).
 */
int gdb_send_signal_packet(char *buf, size_t buf_len, char signal)
{
	size_t size;
	int32_t status;

	if (buf_len < 4) {
		/* Buffer too small */
		return TEOS_EOF;
	}

	buf[0] = 'S';
	status = gdb_enc_hex(&buf[1], buf_len-1, &signal, 1);
	if (status == TEOS_EOF) {
		return TEOS_EOF;
	}
	size = 1 + status;
	return gdb_send_packet(buf, size);
}

/*
 * Send a error packet (E AA).
 */
int gdb_send_error_packet(char *buf, size_t buf_len, char error)
{
	size_t size;
	int status;

	if (buf_len < 4) {
		/* Buffer too small */
		return TEOS_EOF;
	}

	buf[0] = 'E';
	status = gdb_enc_hex(&buf[1], buf_len-1, &error, 1);
	if (status == TEOS_EOF) {
		return TEOS_EOF;
	}
	size = 1 + status;
	return gdb_send_packet(buf, size);
}

/*
 * Encode data to its binary representation in a buffer.
 *
 * Returns:
 *    0+  number of bytes written to buf
 *    TEOS_EOF if the buffer is too small
 */
int gdb_enc_bin(char *buf, size_t buf_len, const char *data, size_t data_len)
{
	size_t buf_pos, data_pos;

	for (buf_pos = 0, data_pos = 0; data_pos < data_len; data_pos++) {
		if (data[data_pos] == '$' ||
			data[data_pos] == '#' ||
			data[data_pos] == '}' ||
			data[data_pos] == '*') {
			if (buf_pos+1 >= buf_len) {
				tassert(0);
				return TEOS_EOF;
			}
			buf[buf_pos++] = '}';
			buf[buf_pos++] = data[data_pos] ^ 0x20;
		} else {
			if (buf_pos >= buf_len) {
				tassert(0);
				return TEOS_EOF;
			}
			buf[buf_pos++] = data[data_pos];
		}
	}

	return buf_pos;
}

/*
 * Decode data from its bin-value representation to a buffer.
 *
 * Returns:
 *    0+  if successful, number of bytes decoded
 *    TEOS_EOF if the buffer is too small
 */
int gdb_dec_bin(const char *buf, size_t buf_len, char *data, size_t data_len)
{
	size_t buf_pos, data_pos;

	for (buf_pos = 0, data_pos = 0; buf_pos < buf_len; buf_pos++) {
		if (data_pos >= data_len) {
			/* Output buffer overflow */
			tassert(0);
			return TEOS_EOF;
		}
		if (buf[buf_pos] == '}') {
			/* The next byte is escaped! */
			if (buf_pos+1 >= buf_len) {
				/* There's an escape character, but no escaped character
				 * following the escape character. */
				tassert(0);
				return TEOS_EOF;
			}
			buf_pos += 1;
			data[data_pos++] = buf[buf_pos] ^ 0x20;
		} else {
			data[data_pos++] = buf[buf_pos];
		}
	}

	return data_pos;
}

/*
 * Read from memory and encode into buf.
 *
 * Returns:
 *    0+  number of bytes written to buf
 *    TEOS_EOF if the buffer is too small
 */
int gdb_mem_read(char *buf, size_t buf_len, uint32_t addr, size_t len, gdb_enc_func enc)
{
	uint8_t data[64];
	size_t pos;

	if (len > sizeof(data)) {
		return TEOS_EOF;
	}

	/* Read from system memory */
	for (pos = 0; pos < len; pos++) {
		if (gdb_sys_mem_readb(addr+pos, &data[pos])) {
			/* Failed to read */
			return TEOS_EOF;
		}
	}

	/* Encode data */
    return enc(buf, buf_len, (char*)data, len);
}

/*
 * Write to memory from encoded buf.
 */
int gdb_mem_write(const char *buf, size_t buf_len, uint32_t addr, size_t len, gdb_dec_func dec)
{
	uint8_t data[64];
	size_t pos;

	if (len > sizeof(data)) {
		return TEOS_EOF;
	}

	/* Decode data */
    if (dec(buf, buf_len, (char*)data, len) == TEOS_EOF) {
		return TEOS_EOF;
	}

	/* Write to system memory */
	for (pos = 0; pos < len; pos++) {
		if (gdb_sys_mem_writeb(addr+pos, data[pos])) {
			/* Failed to write */
			return TEOS_EOF;
		}
	}

	return 0;
}

void gdb_main(uint32_t regs[GDB_CPU_I386_REG_NUM])
{
    uint32_t        addr;
	int             status;
	size_t          length;
	size_t          pkt_len;
	const char   *ptr_next;
    char         pkt_buf[256];

	gdb_send_signal_packet(pkt_buf, sizeof(pkt_buf), 0);

    while (1) {
		/* Receive the next packet */
		status = gdb_recv_packet(pkt_buf, sizeof(pkt_buf), &pkt_len);
		if (status == TEOS_EOF) {
			break;
		}

		if (pkt_len == 0) {
			/* Received empty packet.. */
			continue;
		}

		ptr_next = pkt_buf;

		/*
		 * Handle one letter commands
		 */
		switch (pkt_buf[0]) {

		/* Calculate remaining space in packet from ptr_next position. */
		#define token_remaining_buf (pkt_len-(ptr_next-pkt_buf))

		/* Expecting a seperator. If not present, go to error */
		#define token_expect_seperator(c) \
			{ \
				if (!ptr_next || *ptr_next != c) { \
					goto error; \
				} else { \
					ptr_next += 1; \
				} \
			}

		/* Expecting an integer argument. If not present, go to error */
		#define token_expect_integer_arg(arg) \
			{ \
				arg = gdb_strtol(ptr_next, token_remaining_buf, \
				                 16, &ptr_next); \
				if (!ptr_next) { \
					goto error; \
				} \
			}

		/*
		 * Read Registers
		 * Command Format: g
		 */
		case 'g':
			/* Encode registers */
			status = gdb_enc_hex(pkt_buf, sizeof(pkt_buf),
			                     (char *)regs,
			                     sizeof(uint32_t) * GDB_CPU_I386_REG_NUM);
			if (status == TEOS_EOF) {
				goto error;
			}
			pkt_len = status;
			gdb_send_packet(pkt_buf, pkt_len);
			break;
		
		/*
		 * Write Registers
		 * Command Format: G XX...
		 */
		case 'G':
			status = gdb_dec_hex(pkt_buf+1, pkt_len-1,
			                     (char *)regs,
			                     sizeof(uint32_t) * GDB_CPU_I386_REG_NUM);
			if (status == TEOS_EOF) {
				goto error;
			}
			gdb_send_ok_packet();
			break;

		/*
		 * Read a Register
		 * Command Format: p n
		 */
		case 'p':
			ptr_next += 1;
			token_expect_integer_arg(addr);

			if (addr >= GDB_CPU_I386_REG_NUM) {
				goto error;
			}

			/* Read Register */
			status = gdb_enc_hex(pkt_buf, sizeof(pkt_buf),
			                     (char *)&(regs[addr]),
                                 sizeof(regs[addr]));
			if (status == TEOS_EOF) {
				goto error;
			}
			gdb_send_packet(pkt_buf, status);
			break;
		
		/*
		 * Write a Register
		 * Command Format: P n...=r...
		 */
		case 'P':
			ptr_next += 1;
			token_expect_integer_arg(addr);
			token_expect_seperator('=');

			if (addr >= GDB_CPU_I386_REG_NUM) {
				goto error;
			}

			status = gdb_dec_hex(ptr_next, token_remaining_buf,
			                     (char *)&(regs[addr]),
			                     sizeof(regs[addr]));
			if (status == TEOS_EOF) {
				goto error;
			}
			gdb_send_ok_packet();
			break;
		
		/*
		 * Read Memory
		 * Command Format: m addr,length
		 */
		case 'm':
			ptr_next += 1;
			token_expect_integer_arg(addr);
			token_expect_seperator(',');
			token_expect_integer_arg(length);

			/* Read Memory */
			status = gdb_mem_read(pkt_buf, sizeof(pkt_buf),
			                      addr, length, gdb_enc_hex);
			if (status == TEOS_EOF) {
				goto error;
			}
			gdb_send_packet(pkt_buf, status);
			break;
		
		/*
		 * Write Memory
		 * Command Format: M addr,length:XX..
		 */
		case 'M':
			ptr_next += 1;
			token_expect_integer_arg(addr);
			token_expect_seperator(',');
			token_expect_integer_arg(length);
			token_expect_seperator(':');

			/* Write Memory */
			status = gdb_mem_write(ptr_next, token_remaining_buf,
			                       addr, length, gdb_dec_hex);
			if (status == TEOS_EOF) {
				goto error;
			}
			gdb_send_ok_packet();
			break;

		/*
		 * Write Memory (Binary)
		 * Command Format: X addr,length:XX..
		 */
		case 'X':
			ptr_next += 1;
			token_expect_integer_arg(addr);
			token_expect_seperator(',');
			token_expect_integer_arg(length);
			token_expect_seperator(':');

			/* Write Memory */
			status = gdb_mem_write(ptr_next, token_remaining_buf,
			                       addr, length, gdb_dec_bin);
			if (status == TEOS_EOF) {
				goto error;
			}
			gdb_send_ok_packet();
			break;

		/* 
		 * Continue
		 * Command Format: c [addr]
		 */
		case 'c':
			gdb_sys_continue();
			return;

		/*
		 * Single-step
		 * Command Format: s [addr]
		 */
		case 's':
			gdb_sys_step();
			return;

		case '?':
			gdb_send_signal_packet(pkt_buf, sizeof(pkt_buf), 0);
			break;

		/*
		 * Unsupported Command
		 */
		default:
			gdb_send_packet(NULL, 0);
		}

		continue;

	error:
		gdb_send_error_packet(pkt_buf, sizeof(pkt_buf), 0x00);

		#undef token_remaining_buf
		#undef token_expect_seperator
		#undef token_expect_integer_arg
	}
}
