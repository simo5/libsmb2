/* -*-  mode:c; tab-width:8; c-basic-offset:8; indent-tabs-mode:nil;  -*- */
/*
   Copyright (C) 2016 by Ronnie Sahlberg <ronniesahlberg@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef STDC_HEADERS
#include <stddef.h>
#endif

#include <stdio.h>

#include "smb2.h"
#include "libsmb2.h"
#include "libsmb2-private.h"

static int
smb2_encode_echo_request(struct smb2_context *smb2,
                         struct smb2_pdu *pdu)
{
        char *buf;
        int len;
        
        len = 4;

        buf = malloc(len);
        if (buf == NULL) {
                smb2_set_error(smb2, "Failed to allocate echo buffer");
                return -1;
        }
        memset(buf, 0, len);
        
        pdu->out.iov[pdu->out.niov].len = len;
        pdu->out.iov[pdu->out.niov].buf = buf;
        pdu->out.iov[pdu->out.niov].free = free;
        
        smb2_set_uint16(&pdu->out.iov[pdu->out.niov], 0, 4);

        pdu->out.niov++;
        return 0;
}

int smb2_echo_async(struct smb2_context *smb2,
                    smb2_command_cb cb, void *cb_data)
{
        struct smb2_pdu *pdu;
        
        pdu = smb2_allocate_pdu(smb2, SMB2_ECHO, cb, cb_data);
        if (pdu == NULL) {
                return -1;
        }

        if (smb2_encode_echo_request(smb2, pdu)) {
                smb2_free_pdu(smb2, pdu);
                return -1;
        }
        
        if (smb2_queue_pdu(smb2, pdu)) {
                smb2_free_pdu(smb2, pdu);
                return -1;
        }

        return 0;
}

int smb2_process_echo_reply(struct smb2_context *smb2,
                            struct smb2_pdu *pdu)
{
        pdu->cb(smb2, pdu->header.status, NULL, pdu->cb_data);
        
        return 0;
}

