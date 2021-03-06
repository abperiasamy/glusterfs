/*
   Copyright (c) 2006-2010 Gluster, Inc. <http://www.gluster.com>
   This file is part of GlusterFS.

   GlusterFS is free software; you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published
   by the Free Software Foundation; either version 3 of the License,
   or (at your option) any later version.

   GlusterFS is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see
   <http://www.gnu.org/licenses/>.
*/

#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>
#include <sys/types.h>
#include <dirent.h>

#ifdef GF_SOLARIS_HOST_OS
#include "logging.h"
#endif /* GF_SOLARIS_HOST_OS */

#include "compat.h"
#include "common-utils.h"

#ifdef GF_SOLARIS_HOST_OS

int 
solaris_fsetxattr(int fd, 
		  const char* key, 
		  const char *value, 
		  size_t size, 
		  int flags)
{
	int attrfd = -1;
	int ret = 0;
	
	attrfd = openat (fd, key, flags|O_CREAT|O_WRONLY|O_XATTR, 0777);
	if (attrfd >= 0) {
		ftruncate (attrfd, 0);
		ret = write (attrfd, value, size);
		close (attrfd);
	} else {
		if (errno != ENOENT)
			gf_log ("libglusterfs", GF_LOG_ERROR, 
				"Couldn't set extended attribute for %d (%d)", 
				fd, errno);
		return -1;
	}
	
	return 0;
}


int 
solaris_fgetxattr(int fd, 
		  const char* key, 
		  char *value, 
		  size_t size)
{
	int attrfd = -1;
	int ret = 0;
	
	attrfd = openat (fd, key, O_RDONLY|O_XATTR);
	if (attrfd >= 0) {
		if (size == 0) {
			struct stat buf;
			fstat (attrfd, &buf);
			ret = buf.st_size;
		} else {
			ret = read (attrfd, value, size);
		}
		close (attrfd);
	} else {
		if (errno == ENOENT)
			errno = ENODATA;
		if (errno != ENOENT)
			gf_log ("libglusterfs", GF_LOG_DEBUG, 
				"Couldn't read extended attribute for the file %d (%d)", 
				fd, errno);
		return -1;
	}
	
	return ret;
}


int 
solaris_setxattr(const char *path, 
		 const char* key, 
		 const char *value, 
		 size_t size, 
		 int flags)
{
	int attrfd = -1;
	int ret = 0;
	
	attrfd = attropen (path, key, flags|O_CREAT|O_WRONLY, 0777);
	if (attrfd >= 0) {
		ftruncate (attrfd, 0);
		ret = write (attrfd, value, size);
		close (attrfd);
	} else {
		if (errno != ENOENT)
			gf_log ("libglusterfs", GF_LOG_ERROR, 
				"Couldn't set extended attribute for %s (%d)", 
				path, errno);
		return -1;
	}
	
	return 0;
}


int
solaris_listxattr(const char *path, 
		  char *list, 
		  size_t size)
{
	int attrdirfd = -1;
	ssize_t len = 0;
	DIR *dirptr = NULL;
	struct dirent *dent = NULL;
	int newfd = -1;
	
	attrdirfd = attropen (path, ".", O_RDONLY, 0);
	if (attrdirfd >= 0) {
		newfd = dup(attrdirfd);
		dirptr = fdopendir(newfd);
		if (dirptr) {
			while ((dent = readdir(dirptr))) {
				size_t listlen = strlen(dent->d_name);
				if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")) {
					/* we don't want "." and ".." here */
					continue;
				}
				if (size == 0) {
					/* return the current size of the list of extended attribute names*/
					len += listlen + 1;
				} else {
					/* check size and copy entrie + nul into list. */
					if ((len + listlen + 1) > size) {
						errno = ERANGE;
						len = -1;
						break;
					} else {
						strncpy(list + len, dent->d_name, listlen);
						len += listlen;
						list[len] = '\0';
						++len;
					}
				}
			}
			
			if (closedir(dirptr) == -1) {
				close (attrdirfd);
				return -1;
			}
		} else {
			close (attrdirfd);
			return -1;
		}
		close (attrdirfd);
	}
	return len;
}


int
solaris_flistxattr(int fd,
                   char *list, 
                   size_t size)
{
	int attrdirfd = -1;
	ssize_t len = 0;
	DIR *dirptr = NULL;
	struct dirent *dent = NULL;
	int newfd = -1;
	
	attrdirfd = openat (fd, ".", O_RDONLY, 0);
	if (attrdirfd >= 0) {
		newfd = dup(attrdirfd);
		dirptr = fdopendir(newfd);
		if (dirptr) {
			while ((dent = readdir(dirptr))) {
				size_t listlen = strlen(dent->d_name);
				if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")) {
					/* we don't want "." and ".." here */
					continue;
				}
				if (size == 0) {
					/* return the current size of the list of extended attribute names*/
					len += listlen + 1;
				} else {
					/* check size and copy entrie + nul into list. */
					if ((len + listlen + 1) > size) {
						errno = ERANGE;
						len = -1;
						break;
					} else {
						strncpy(list + len, dent->d_name, listlen);
						len += listlen;
						list[len] = '\0';
						++len;
					}
				}
			}
			
			if (closedir(dirptr) == -1) {
				close (attrdirfd);
				return -1;
			}
		} else {
			close (attrdirfd);
			return -1;
		}
		close (attrdirfd);
	}
	return len;
}


int 
solaris_removexattr(const char *path, 
		    const char* key)
{
	int ret = -1;
	int attrfd = attropen (path, ".", O_RDONLY, 0);
	if (attrfd >= 0) {
		ret = unlinkat (attrfd, key, 0);
		close (attrfd);
	} else {
		if (errno == ENOENT)
			errno = ENODATA;
		return -1;
	}
	
	return ret;
}

int 
solaris_getxattr(const char *path, 
		 const char* key, 
		 char *value, 
		 size_t size)
{
	int attrfd = -1;
	int ret = 0;
	
	attrfd = attropen (path, key, O_RDONLY, 0);
	if (attrfd >= 0) {
		if (size == 0) {
			struct stat buf;
			fstat (attrfd, &buf);
			ret = buf.st_size;
		} else {
			ret = read (attrfd, value, size);
		}
		close (attrfd);
	} else {
		if (errno == ENOENT)
			errno = ENODATA;
		if (errno != ENOENT)
			gf_log ("libglusterfs", GF_LOG_DEBUG, 
				"Couldn't read extended attribute for the file %s (%d)", 
				path, errno);
		return -1;
	}
	return ret;
}


char* strsep(char** str, const char* delims)
{
	char* token;
	
	if (*str==NULL) {
		/* No more tokens */
		return NULL;
	}
	
	token=*str;
	while (**str!='\0') {
		if (strchr(delims,**str)!=NULL) {
			**str='\0';
			(*str)++;
			return token;
		}
		(*str)++;
	}
	/* There is no other token */
	*str=NULL;
	return token;
}

/* Code comes from libiberty */

int
vasprintf (char **result, const char *format, va_list args)
{
  return gf_vasprintf(result, format, args);
}

int
asprintf (char **buf, const char *fmt, ...)
{
  int status;
  va_list ap;

  va_start (ap, fmt);
  status = vasprintf (buf, fmt, ap);
  va_end (ap);
  return status;  
}

#endif /* GF_SOLARIS_HOST_OS */

#ifndef HAVE_STRNLEN
size_t 
strnlen(const char *string, size_t maxlen)                   
{
	int len = 0;
	while ((len < maxlen) && string[len])
		len++;
	return len;
}
#endif /* STRNLEN */
