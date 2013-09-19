/*
 * u_serial.h - interface to USB gadget "serial port"/TTY utilities
 *
 * Copyright (C) 2008 David Brownell
 * Copyright (C) 2008 by Nokia Corporation
 *
 * This software is distributed under the terms of the GNU General
 * Public License ("GPL") as published by the Free Software Foundation,
 * either version 2 of that License or (at your option) any later version.
 */

#ifndef __PXA910_U_SERIAL_H
#define __PXA910_U_SERIAL_H

#include <linux/usb/composite.h>
#include <linux/usb/cdc.h>

/*
 * One non-multiplexed "serial" I/O port ... there can be several of these
 * on any given USB peripheral device, if it provides enough endpoints.
 *
 * The "u_serial" utility component exists to do one thing:  manage TTY
 * style I/O using the USB peripheral endpoints listed here, including
 * hookups to sysfs and /dev for each logical "tty" device.
 *
 * REVISIT at least ACM could support tiocmget() if needed.
 *
 * REVISIT someday, allow multiplexing several TTYs over these endpoints.
 */
struct pxa910_gserial {
	struct usb_function		func;

	/* port is managed by gserial_{connect,disconnect} */
	struct pxa910_gs_port			*ioport;

	struct usb_ep			*in;
	struct usb_ep			*out;

	/* REVISIT avoid this CDC-ACM support harder ... */
	struct usb_cdc_line_coding port_line_coding;	/* 9600-8-N-1 etc */
	wait_queue_head_t port_send;

	/* notification callbacks */
	void (*connect)(struct pxa910_gserial *p);
	void (*disconnect)(struct pxa910_gserial *p);
	int (*send_break)(struct pxa910_gserial *p, int duration);
};

/* utilities to allocate/free request and buffer */
struct usb_request *
pxa910_gs_alloc_req(struct usb_ep *ep, unsigned len, gfp_t flags);
void pxa910_gs_free_req(struct usb_ep *, struct usb_request *req);

/* port setup/teardown is handled by gadget driver */
int pxa910_gserial_setup(struct usb_gadget *g, unsigned n_ports);
void pxa910_gserial_cleanup(void);

/* connect/disconnect is handled by individual functions */
int pxa910_gserial_connect(struct pxa910_gserial *, u8 port_num);
void pxa910_gserial_disconnect(struct pxa910_gserial *);

/* functions are bound to configurations by a config or gadget driver */
int pxa910_acm_bind_config(struct usb_configuration *c, u8 port_num);
int pxa910_diag_bind_config(struct usb_configuration *c, u8 port_num);

#endif /* __PXA910_U_SERIAL_H */