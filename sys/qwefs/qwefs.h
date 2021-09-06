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

/*
 * QWE file system - Experimental file system for learning purpose.
 *
 * The name is "qwe" like kids which are using "abc" for learning
 * alphabet.
 *
 * v1 - no metadata on disk. only one file on the filesystem (the
 *      whole size). root_vnode (VDIR) exists on memory only and
 *      contains always 1 vnode (VREG). vnode attributes comes from
 *      mount(8) arguments.
 *
 * 2020-12-20 start
 *
 */

struct qwefsmount {
	struct mount *qwem_mountp;	/* filesystem vfs structure */
	dev_t qwem_dev;			/* device mounted */
	struct vnode *qwem_devvp;	/* block device mounted vnode */
	struct netexport qwem_export;	/* export information */
	
	/* attributes storage */
	uid_t  qwem_uid;
	gid_t  qwem_gid;
	mode_t qwem_mode;
};

#define QWEFS_ROOTINO	1
#define QWEFS_FILEINO	2


#define VFSTOQWEFS(mp)	((struct qwefsmount *)((mp)->mnt_data))

#ifdef _KERNEL
extern const struct vops qwefs_vops;
#endif
