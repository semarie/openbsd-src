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
#include <sys/systm.h>
#include <sys/vnode.h>
#include <sys/mount.h>

#include <qwefs/qwefs.h>


int qwefs_mount(struct mount *, const char *, void *, struct nameidata *,
    struct proc *);
int qwefs_start(struct mount *, int, struct proc *);
int qwefs_unmount(struct mount *, int, struct proc *);
int qwefs_root(struct mount *, struct vnode **);
int qwefs_quotactl(struct mount *, int, uid_t, caddr_t, struct proc *);
int qwefs_statfs(struct mount *, struct statfs *, struct proc *);
int qwefs_sync(struct mount *, int, int, struct ucred *, struct proc *);
int qwefs_vget(struct mount *, ino_t, struct vnode **);
int qwefs_fhtovp(struct mount *, struct fid *, struct vnode **);
int qwefs_vptofh(struct vnode *, struct fid *);
int qwefs_init(struct vfsconf *);
int qwefs_sysctl(int *, u_int, void *, size_t *, void *, size_t, struct proc *);
int qwefs_checkexp(struct mount *, struct mbuf *, int *, struct ucred **);


int
qwefs_mount(struct mount *mp, const char *path, void *data,
    struct nameidata *ndp, struct proc *p)
{
	//struct qwefs_args *args = data;

	if (mp->mnt_flag & MNT_UPDATE) {
		/* update isn't supported */
		return EINVAL;
	}
	
	return EOPNOTSUPP;
}


int
qwefs_start(struct mount *mp, int flags, struct proc *p)
{
	return EOPNOTSUPP;
}


int
qwefs_unmount(struct mount *mp, int mntflags, struct proc *p)
{
	return EOPNOTSUPP;
}


int
qwefs_root(struct mount *mp, struct vnode **vpp)
{
	return EOPNOTSUPP;
}


int
qwefs_quotactl(struct mount *mp, int cmds, uid_t uid, caddr_t arg,
    struct proc *p)
{
	return EOPNOTSUPP;
}


int
qwefs_statfs(struct mount *mp, struct statfs *sbp, struct proc *p)
{
	return EOPNOTSUPP;
}


int
qwefs_sync(struct mount *mp, int waitfor, int stall,
    struct ucred *cred, struct proc *p)
{
	return EOPNOTSUPP;
}


int
qwefs_vget(struct mount *mp, ino_t ino, struct vnode **vpp)
{
	return EOPNOTSUPP;
}


int
qwefs_fhtovp(struct mount *mp, struct fid *fhp, struct vnode **vpp)
{
	return EOPNOTSUPP;
}


int
qwefs_vptofh(struct vnode *vp, struct fid *fhp)
{
	return EOPNOTSUPP;
}


int
qwefs_init(struct vfsconf *vfsp)
{
	return 0;
}


int
qwefs_sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp,
    size_t newlenp, struct proc *p)
{
	return EOPNOTSUPP;
}


int
qwefs_checkexp(struct mount *mp, struct mbuf *nam, int *extflagsp,
    struct ucred **credanonp)
{
	return EOPNOTSUPP;
}




const struct vfsops qwefs_vfsops = {
	.vfs_mount	= qwefs_mount,
	.vfs_start	= qwefs_start,
	.vfs_unmount	= qwefs_unmount,
	.vfs_root	= qwefs_root,
	.vfs_quotactl	= qwefs_quotactl,
	.vfs_statfs	= qwefs_statfs,
	.vfs_sync	= qwefs_sync,
	.vfs_vget	= qwefs_vget,
	.vfs_fhtovp	= qwefs_fhtovp,
	.vfs_vptofh	= qwefs_vptofh,
	.vfs_init	= qwefs_init,
	.vfs_sysctl	= qwefs_sysctl,
	.vfs_checkexp	= qwefs_checkexp,
};
