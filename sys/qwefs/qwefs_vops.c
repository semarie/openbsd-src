/*	$OpenBSD$ */
/*
 * Copyright (c) 2020 Sebastien Marie <semarie@online.fr>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/vnode.h>
#include <sys/mount.h>

#include <qwefs/qwefs.h>

/*
 * Global vfs data structures
 */
const struct vops qwefs_vops = {
	.vop_lock	= NULL,
	.vop_unlock	= NULL,
	.vop_islocked	= NULL,
	.vop_abortop	= NULL,
	.vop_access	= NULL,
	.vop_advlock	= NULL,
	.vop_bmap	= NULL,
	.vop_bwrite	= NULL,
	.vop_close	= NULL,
	.vop_create	= NULL,
	.vop_fsync	= NULL,
	.vop_getattr	= NULL,
	.vop_inactive	= NULL,
	.vop_ioctl	= NULL,
	.vop_link	= NULL,
	.vop_lookup	= NULL,
	.vop_mknod	= NULL,
	.vop_open	= NULL,
	.vop_pathconf	= NULL,
	.vop_poll	= NULL,
	.vop_print	= NULL,
	.vop_read	= NULL,
	.vop_readdir	= NULL,
	.vop_readlink	= NULL,
	.vop_reclaim	= NULL,
	.vop_remove	= NULL,
	.vop_rename	= NULL,
	.vop_revoke	= NULL,
	.vop_mkdir	= NULL,
	.vop_rmdir	= NULL,
	.vop_setattr	= NULL,
	.vop_strategy	= NULL,
	.vop_symlink	= NULL,
	.vop_write	= NULL,
	.vop_kqfilter	= NULL,
};
