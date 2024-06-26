/*
 * avrdude - A Downloader/Uploader for AVR device programmers
 * Copyright (C) 2000-2004  Brian S. Dean <bsd@bsdhome.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* $Id: ppi.h,v 1.18 2007/01/24 22:43:46 joerg_wunsch Exp $ */

#define HAVE_PARPORT 1
#ifndef ppi_h
#define ppi_h


#define B0 (1<<0)
#define B1 (1<<1)
#define B2 (1<<2)
#define B3 (1<<3)
#define B4 (1<<4)
#define B5 (1<<5)
#define B6 (1<<6)
#define B7 (1<<7)


union filedescriptor
{
  int ifd;
  void *pfd;
};

/*
 * PPI registers
 */
enum {
  PPIDATA,
  PPICTRL,
  PPISTATUS
};

#ifdef __cplusplus
extern "C" {
#endif

void usleep(unsigned long us);

int ppi_get       (union filedescriptor *fdp, int reg, int bit);

int ppi_set       (union filedescriptor *fdp, int reg, int bit);

int ppi_clr       (union filedescriptor *fdp, int reg, int bit);

int ppi_getall    (union filedescriptor *fdp, int reg);

int ppi_setall    (union filedescriptor *fdp, int reg, int val);

int ppi_toggle    (union filedescriptor *fdp, int reg, int bit);

void ppi_open     (char * port, union filedescriptor *fdp);

void ppi_close    (union filedescriptor *fdp);

#ifdef __cplusplus
}
#endif

#endif


