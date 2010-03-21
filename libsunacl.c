/*-
 * Copyright (c) 2008, 2009 Edward Tomasz Napierała <trasz@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <err.h>
#include <sys/acl.h>

#include "acl.h"
#include "opensolaris_acl.h"

static int
xacl(const char *path, int fd, int cmd, int cnt, void *buf)
{
	int error, nentries;
	acl_t aclp;

	switch (cmd) {
	case ACE_SETACL:
		if (buf == NULL || cnt <= 0) {
			errno = EINVAL;
			return (-1);
		}

		if (cnt >= ACL_MAX_ENTRIES) {
			errno = ENOSPC;
			return (-1);
		}

		aclp = acl_init(cnt);

		error = acl_from_aces(&(aclp->ats_acl), buf, cnt);
		if (error) {
			warnx("aces_from_acl failed");
			errno = EIO;
			return (-1);
		}

		if (path != NULL)
			error = acl_set_file(path, ACL_TYPE_NFS4, aclp);
		else
			error = acl_set_fd_np(fd, aclp, ACL_TYPE_NFS4);
		if (error) {
			if (errno == EOPNOTSUPP)
				errno = ENOSYS;
			return (-1);
		}

		return (0);

	case ACE_GETACL:
		if (buf == NULL) {
			errno = EINVAL;
			return (-1);
		}

		if (path != NULL)
			aclp = acl_get_file(path, ACL_TYPE_NFS4);
		else
			aclp = acl_get_fd_np(fd, ACL_TYPE_NFS4);
		if (aclp == NULL) {
			if (errno == EOPNOTSUPP)
				errno = ENOSYS;
			return (-1);
		}

		if (aclp->ats_acl.acl_cnt > cnt) {
			errno = ENOSPC;
			return (-1);
		}

		error = aces_from_acl(buf, &nentries, &(aclp->ats_acl));
		if (error) {
			warnx("aces_from_acl failed");
			errno = EIO;
			return (-1);
		}

		return (nentries);

	case ACE_GETACLCNT:
		if (path != NULL)
			aclp = acl_get_file(path, ACL_TYPE_NFS4);
		else
			aclp = acl_get_fd_np(fd, ACL_TYPE_NFS4);
		if (aclp == NULL) {
			if (errno == EOPNOTSUPP)
				errno = ENOSYS;
			return (-1);
		}

		nentries = aclp->ats_acl.acl_cnt;
		return (nentries);

	default:
		errno = EINVAL;
		return (-1);
	}
}

int
acl(const char *path, int cmd, int cnt, void *buf)
{
	if (path == NULL) {
		errno = EINVAL;
		return (-1);
	}

	return xacl(path, -1, cmd, cnt, buf);
}

int
facl(int fd, int cmd, int cnt, void *buf)
{
	return xacl(NULL, fd, cmd, cnt, buf);
}

