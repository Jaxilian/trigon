#include "fs.h"

#ifdef _WIN32
#define USE_DIRENT_WIN32

// =============== DIRENT BEGIN ===========================

/*
	Code under dirent "DIRENT BEGIN" until "DIRENT END" is protected with a MIT License, written below
	This section of code might be removed in the future due to the copyright, licenses is a hell to
	keep track of. Which also can be changed in the future, not reliable!

	The MIT License (MIT)

	Copyright (c) 1998-2019 Toni Ronkko

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.


*/

#include <direct.h>
#include <shlobj_core.h>

#if defined(USE_DIRENT_WIN32) && defined(_WIN32)
/*
 * Dirent interface for Microsoft Visual Studio
 *
 * Copyright (C) 1998-2019 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */
#ifndef DIRENT_H
#define DIRENT_H

 /* Hide warnings about unreferenced local functions */
#if defined(__clang__)
#	pragma clang diagnostic ignored "-Wunused-function"
#elif defined(_MSC_VER)
#	pragma warning(disable:4505)
#elif defined(__GNUC__)
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif

/*
 * Include windows.h without Windows Sockets 1.1 to prevent conflicts with
 * Windows Sockets 2.0.
 */
#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

 /* Indicates that d_type field is available in dirent structure */
#define _DIRENT_HAVE_D_TYPE

/* Indicates that d_namlen field is available in dirent structure */
#define _DIRENT_HAVE_D_NAMLEN

/* Entries missing from MSVC 6.0 */
#if !defined(FILE_ATTRIBUTE_DEVICE)
#	define FILE_ATTRIBUTE_DEVICE 0x40
#endif

/* File type and permission flags for stat(), general mask */
#if !defined(S_IFMT)
#	define S_IFMT _S_IFMT
#endif

/* Directory bit */
#if !defined(S_IFDIR)
#	define S_IFDIR _S_IFDIR
#endif

/* Character device bit */
#if !defined(S_IFCHR)
#	define S_IFCHR _S_IFCHR
#endif

/* Pipe bit */
#if !defined(S_IFFIFO)
#	define S_IFFIFO _S_IFFIFO
#endif

/* Regular file bit */
#if !defined(S_IFREG)
#	define S_IFREG _S_IFREG
#endif

/* Read permission */
#if !defined(S_IREAD)
#	define S_IREAD _S_IREAD
#endif

/* Write permission */
#if !defined(S_IWRITE)
#	define S_IWRITE _S_IWRITE
#endif

/* Execute permission */
#if !defined(S_IEXEC)
#	define S_IEXEC _S_IEXEC
#endif

/* Pipe */
#if !defined(S_IFIFO)
#	define S_IFIFO _S_IFIFO
#endif

/* Block device */
#if !defined(S_IFBLK)
#	define S_IFBLK 0
#endif

/*
 * Symbolic link.  Be ware that S_IFLNK value and S_ISLNK() macro are only
 * usable with dirent - they do not work with stat() function call!
 */
#if !defined(S_IFLNK)
#	define S_IFLNK (_S_IFDIR | _S_IFREG)
#endif

 /* Socket */
#if !defined(S_IFSOCK)
#	define S_IFSOCK 0
#endif

/* Read user permission */
#if !defined(S_IRUSR)
#	define S_IRUSR S_IREAD
#endif

/* Write user permission */
#if !defined(S_IWUSR)
#	define S_IWUSR S_IWRITE
#endif

/* Execute user permission */
#if !defined(S_IXUSR)
#	define S_IXUSR 0
#endif

/* User full permissions */
#if !defined(S_IRWXU)
#	define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
#endif

/* Read group permission */
#if !defined(S_IRGRP)
#	define S_IRGRP 0
#endif

/* Write group permission */
#if !defined(S_IWGRP)
#	define S_IWGRP 0
#endif

/* Execute group permission */
#if !defined(S_IXGRP)
#	define S_IXGRP 0
#endif

/* Group full permissions */
#if !defined(S_IRWXG)
#	define S_IRWXG (S_IRGRP | S_IWGRP | S_IXGRP)
#endif

/* Read others permission */
#if !defined(S_IROTH)
#	define S_IROTH 0
#endif

/* Write others permission */
#if !defined(S_IWOTH)
#	define S_IWOTH 0
#endif

/* Execute others permission */
#if !defined(S_IXOTH)
#	define S_IXOTH 0
#endif

/* Other full permissions */
#if !defined(S_IRWXO)
#	define S_IRWXO (S_IROTH | S_IWOTH | S_IXOTH)
#endif

/* Maximum length of file name */
#if !defined(PATH_MAX)
#	define PATH_MAX MAX_PATH
#endif
#if !defined(FILENAME_MAX)
#	define FILENAME_MAX MAX_PATH
#endif
#if !defined(NAME_MAX)
#	define NAME_MAX FILENAME_MAX
#endif

/* File type flags for d_type */
#define DT_UNKNOWN 0
#define DT_REG S_IFREG
#define DT_DIR S_IFDIR
#define DT_FIFO S_IFIFO
#define DT_SOCK S_IFSOCK
#define DT_CHR S_IFCHR
#define DT_BLK S_IFBLK
#define DT_LNK S_IFLNK

/* Macros for converting between st_mode and d_type */
#define IFTODT(mode) ((mode) & S_IFMT)
#define DTTOIF(type) (type)

/*
 * File type macros.  Note that block devices and sockets cannot be
 * distinguished on Windows, and the macros S_ISBLK and S_ISSOCK are only
 * defined for compatibility.  These macros should always return false on
 * Windows.
 */
#if !defined(S_ISFIFO)
#	define S_ISFIFO(mode) (((mode) & S_IFMT) == S_IFIFO)
#endif
#if !defined(S_ISDIR)
#	define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG)
#	define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISLNK)
#	define S_ISLNK(mode) (((mode) & S_IFMT) == S_IFLNK)
#endif
#if !defined(S_ISSOCK)
#	define S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)
#endif
#if !defined(S_ISCHR)
#	define S_ISCHR(mode) (((mode) & S_IFMT) == S_IFCHR)
#endif
#if !defined(S_ISBLK)
#	define S_ISBLK(mode) (((mode) & S_IFMT) == S_IFBLK)
#endif

 /* Return the exact length of the file name without zero terminator */
#define _D_EXACT_NAMLEN(p) ((p)->d_namlen)

/* Return the maximum size of a file name */
#define _D_ALLOC_NAMLEN(p) ((PATH_MAX)+1)


#ifdef __cplusplus
extern "C" {
#endif


	/* Wide-character version */
	struct _wdirent {
		/* Always zero */
		long d_ino;

		/* Position of next file in a directory stream */
		long d_off;

		/* Structure size */
		unsigned short d_reclen;

		/* Length of name without \0 */
		size_t d_namlen;

		/* File type */
		int d_type;

		/* File name */
		wchar_t d_name[PATH_MAX + 1];
	};
	typedef struct _wdirent _wdirent;

	struct _WDIR {
		/* Current directory entry */
		struct _wdirent ent;

		/* Private file data */
		WIN32_FIND_DATAW data;

		/* True if data is valid */
		int cached;

		/* True if next entry is invalid */
		int invalid;

		/* Win32 search handle */
		HANDLE handle;

		/* Initial directory name */
		wchar_t* patt;
	};
	typedef struct _WDIR _WDIR;

	/* Multi-byte character version */
	struct dirent {
		/* Always zero */
		long d_ino;

		/* Position of next file in a directory stream */
		long d_off;

		/* Structure size */
		unsigned short d_reclen;

		/* Length of name without \0 */
		size_t d_namlen;

		/* File type */
		int d_type;

		/* File name */
		char d_name[PATH_MAX + 1];
	};
	typedef struct dirent dirent;

	struct DIR {
		struct dirent ent;
		struct _WDIR* wdirp;
	};
	typedef struct DIR DIR;


	/* Dirent functions */
	static DIR* opendir(const char* dirname);
	static _WDIR* _wopendir(const wchar_t* dirname);

	static struct dirent* readdir(DIR* dirp);
	static struct _wdirent* _wreaddir(_WDIR* dirp);

	static int readdir_r(
		DIR* dirp, struct dirent* entry, struct dirent** result);
	static int _wreaddir_r(
		_WDIR* dirp, struct _wdirent* entry, struct _wdirent** result);

	static int closedir(DIR* dirp);
	static int _wclosedir(_WDIR* dirp);

	static void rewinddir(DIR* dirp);
	static void _wrewinddir(_WDIR* dirp);

	static long telldir(DIR* dirp);
	static long _wtelldir(_WDIR* dirp);

	static void seekdir(DIR* dirp, long loc);
	static void _wseekdir(_WDIR* dirp, long loc);

	static int scandir(const char* dirname, struct dirent*** namelist,
		int (*filter)(const struct dirent*),
		int (*compare)(const struct dirent**, const struct dirent**));

	static int alphasort(const struct dirent** a, const struct dirent** b);

	static int versionsort(const struct dirent** a, const struct dirent** b);

	static int strverscmp(const char* a, const char* b);

	/* For compatibility with Symbian */
#define wdirent _wdirent
#define WDIR _WDIR
#define wopendir _wopendir
#define wreaddir _wreaddir
#define wclosedir _wclosedir
#define wrewinddir _wrewinddir
#define wtelldir _wtelldir
#define wseekdir _wseekdir

/* Compatibility with older Microsoft compilers and non-Microsoft compilers */
#if !defined(_MSC_VER) || _MSC_VER < 1400
#	define wcstombs_s dirent_wcstombs_s
#	define mbstowcs_s dirent_mbstowcs_s
#endif

/* Optimize dirent_set_errno() away on modern Microsoft compilers */
#if defined(_MSC_VER) && _MSC_VER >= 1400
#	define dirent_set_errno _set_errno
#endif


/* Internal utility functions */
	static WIN32_FIND_DATAW* dirent_first(_WDIR* dirp);
	static WIN32_FIND_DATAW* dirent_next(_WDIR* dirp);
	static long dirent_hash(WIN32_FIND_DATAW* datap);

#if !defined(_MSC_VER) || _MSC_VER < 1400
	static int dirent_mbstowcs_s(
		size_t* pReturnValue, wchar_t* wcstr, size_t sizeInWords,
		const char* mbstr, size_t count);
#endif

#if !defined(_MSC_VER) || _MSC_VER < 1400
	static int dirent_wcstombs_s(
		size_t* pReturnValue, char* mbstr, size_t sizeInBytes,
		const wchar_t* wcstr, size_t count);
#endif

#if !defined(_MSC_VER) || _MSC_VER < 1400
	static void dirent_set_errno(int error);
#endif


	/*
	 * Open directory stream DIRNAME for read and return a pointer to the
	 * internal working area that is used to retrieve individual directory
	 * entries.
	 */
	static _WDIR*
		_wopendir(const wchar_t* dirname)
	{
		wchar_t* p;

		/* Must have directory name */
		if (dirname == NULL || dirname[0] == '\0') {
			dirent_set_errno(ENOENT);
			return NULL;
		}

		/* Allocate new _WDIR structure */
		_WDIR* dirp = (_WDIR*)malloc(sizeof(struct _WDIR));
		if (!dirp)
			return NULL;

		/* Reset _WDIR structure */
		dirp->handle = INVALID_HANDLE_VALUE;
		dirp->patt = NULL;
		dirp->cached = 0;
		dirp->invalid = 0;

		/*
		 * Compute the length of full path plus zero terminator
		 *
		 * Note that on WinRT there's no way to convert relative paths
		 * into absolute paths, so just assume it is an absolute path.
		 */
#if !defined(WINAPI_FAMILY_PARTITION) || WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
		 /* Desktop */
		DWORD n = GetFullPathNameW(dirname, 0, NULL, NULL);
#else
		 /* WinRT */
		size_t n = wcslen(dirname);
#endif

		/* Allocate room for absolute directory name and search pattern */
		dirp->patt = (wchar_t*)malloc(sizeof(wchar_t) * n + 16);
		if (dirp->patt == NULL)
			goto exit_closedir;

		/*
		 * Convert relative directory name to an absolute one.  This
		 * allows rewinddir() to function correctly even when current
		 * working directory is changed between opendir() and rewinddir().
		 *
		 * Note that on WinRT there's no way to convert relative paths
		 * into absolute paths, so just assume it is an absolute path.
		 */
#if !defined(WINAPI_FAMILY_PARTITION) || WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
		 /* Desktop */
		n = GetFullPathNameW(dirname, n, dirp->patt, NULL);
		if (n <= 0)
			goto exit_closedir;
#else
		 /* WinRT */
		wcsncpy_s(dirp->patt, n + 1, dirname, n);
#endif

		/* Append search pattern \* to the directory name */
		p = dirp->patt + n;
		switch (p[-1]) {
		case '\\':
		case '/':
		case ':':
			/* Directory ends in path separator, e.g. c:\temp\ */
			/*NOP*/;
			break;

		default:
			/* Directory name doesn't end in path separator */
			*p++ = '\\';
		}
		*p++ = '*';
		*p = '\0';

		/* Open directory stream and retrieve the first entry */
		if (!dirent_first(dirp))
			goto exit_closedir;

		/* Success */
		return dirp;

		/* Failure */
	exit_closedir:
		_wclosedir(dirp);
		return NULL;
	}

	/*
	 * Read next directory entry.
	 *
	 * Returns pointer to static directory entry which may be overwritten by
	 * subsequent calls to _wreaddir().
	 */
	static struct _wdirent*
		_wreaddir(_WDIR* dirp)
	{
		/*
		 * Read directory entry to buffer.  We can safely ignore the return
		 * value as entry will be set to NULL in case of error.
		 */
		struct _wdirent* entry;
		(void)_wreaddir_r(dirp, &dirp->ent, &entry);

		/* Return pointer to statically allocated directory entry */
		return entry;
	}

	/*
	 * Read next directory entry.
	 *
	 * Returns zero on success.  If end of directory stream is reached, then sets
	 * result to NULL and returns zero.
	 */
	static int
		_wreaddir_r(
			_WDIR* dirp, struct _wdirent* entry, struct _wdirent** result)
	{
		/* Validate directory handle */
		if (!dirp || dirp->handle == INVALID_HANDLE_VALUE || !dirp->patt) {
			dirent_set_errno(EBADF);
			*result = NULL;
			return -1;
		}

		/* Read next directory entry */
		WIN32_FIND_DATAW* datap = dirent_next(dirp);
		if (!datap) {
			/* Return NULL to indicate end of directory */
			*result = NULL;
			return /*OK*/0;
		}

		/*
		 * Copy file name as wide-character string.  If the file name is too
		 * long to fit in to the destination buffer, then truncate file name
		 * to PATH_MAX characters and zero-terminate the buffer.
		 */
		size_t i = 0;
		while (i < PATH_MAX && datap->cFileName[i] != 0) {
			entry->d_name[i] = datap->cFileName[i];
			i++;
		}
		entry->d_name[i] = 0;

		/* Length of file name excluding zero terminator */
		entry->d_namlen = i;

		/* Determine file type */
		DWORD attr = datap->dwFileAttributes;
		if ((attr & FILE_ATTRIBUTE_DEVICE) != 0)
			entry->d_type = DT_CHR;
		else if ((attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
			entry->d_type = DT_LNK;
		else if ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
			entry->d_type = DT_DIR;
		else
			entry->d_type = DT_REG;

		/* Read the next directory entry to cache */
		datap = dirent_next(dirp);
		if (datap) {
			/* Compute 31-bit hash of the next directory entry */
			entry->d_off = dirent_hash(datap);

			/* Push the next directory entry back to cache */
			dirp->cached = 1;
		}
		else {
			/* End of directory stream */
			entry->d_off = (long)((~0UL) >> 1);
		}

		/* Reset other fields */
		entry->d_ino = 0;
		entry->d_reclen = sizeof(struct _wdirent);

		/* Set result address */
		*result = entry;
		return /*OK*/0;
	}

	/*
	 * Close directory stream opened by opendir() function.  This invalidates the
	 * DIR structure as well as any directory entry read previously by
	 * _wreaddir().
	 */
	static int
		_wclosedir(_WDIR* dirp)
	{
		if (!dirp) {
			dirent_set_errno(EBADF);
			return /*failure*/-1;
		}

		/*
		 * Release search handle if we have one.  Being able to handle
		 * partially initialized _WDIR structure allows us to use this
		 * function to handle errors occurring within _wopendir.
		 */
		if (dirp->handle != INVALID_HANDLE_VALUE) {
			FindClose(dirp->handle);
		}

		/*
		 * Release search pattern.  Note that we don't need to care if
		 * dirp->patt is NULL or not: function free is guaranteed to act
		 * appropriately.
		 */
		free(dirp->patt);

		/* Release directory structure */
		free(dirp);
		return /*success*/0;
	}

	/*
	 * Rewind directory stream such that _wreaddir() returns the very first
	 * file name again.
	 */
	static void _wrewinddir(_WDIR* dirp)
	{
		/* Check directory pointer */
		if (!dirp || dirp->handle == INVALID_HANDLE_VALUE || !dirp->patt)
			return;

		/* Release existing search handle */
		FindClose(dirp->handle);

		/* Open new search handle */
		dirent_first(dirp);
	}

	/* Get first directory entry */
	static WIN32_FIND_DATAW*
		dirent_first(_WDIR* dirp)
	{
		/* Open directory and retrieve the first entry */
		dirp->handle = FindFirstFileExW(
			dirp->patt, FindExInfoStandard, &dirp->data,
			FindExSearchNameMatch, NULL, 0);
		if (dirp->handle == INVALID_HANDLE_VALUE)
			goto error;

		/* A directory entry is now waiting in memory */
		dirp->cached = 1;
		return &dirp->data;

	error:
		/* Failed to open directory: no directory entry in memory */
		dirp->cached = 0;
		dirp->invalid = 1;

		/* Set error code */
		DWORD errorcode = GetLastError();
		switch (errorcode) {
		case ERROR_ACCESS_DENIED:
			/* No read access to directory */
			dirent_set_errno(EACCES);
			break;

		case ERROR_DIRECTORY:
			/* Directory name is invalid */
			dirent_set_errno(ENOTDIR);
			break;

		case ERROR_PATH_NOT_FOUND:
		default:
			/* Cannot find the file */
			dirent_set_errno(ENOENT);
		}
		return NULL;
	}

	/* Get next directory entry */
	static WIN32_FIND_DATAW*
		dirent_next(_WDIR* dirp)
	{
		/* Return NULL if seek position was invalid */
		if (dirp->invalid)
			return NULL;

		/* Is the next directory entry already in cache? */
		if (dirp->cached) {
			/* Yes, a valid directory entry found in memory */
			dirp->cached = 0;
			return &dirp->data;
		}

		/* Read the next directory entry from stream */
		if (FindNextFileW(dirp->handle, &dirp->data) == FALSE) {
			/* End of directory stream */
			return NULL;
		}

		/* Success */
		return &dirp->data;
	}

	/*
	 * Compute 31-bit hash of file name.
	 *
	 * See djb2 at http://www.cse.yorku.ca/~oz/hash.html
	 */
	static long
		dirent_hash(WIN32_FIND_DATAW* datap)
	{
		unsigned long hash = 5381;
		unsigned long c;
		const wchar_t* p = datap->cFileName;
		const wchar_t* e = p + MAX_PATH;
		while (p != e && (c = *p++) != 0) {
			hash = (hash << 5) + hash + c;
		}

		return (long)(hash & ((~0UL) >> 1));
	}

	/* Open directory stream using plain old C-string */
	static DIR* opendir(const char* dirname)
	{
		/* Must have directory name */
		if (dirname == NULL || dirname[0] == '\0') {
			dirent_set_errno(ENOENT);
			return NULL;
		}

		/* Allocate memory for DIR structure */
		struct DIR* dirp = (DIR*)malloc(sizeof(struct DIR));
		if (!dirp)
			return NULL;

		/* Convert directory name to wide-character string */
		wchar_t wname[PATH_MAX + 1];
		size_t n;
		int error = mbstowcs_s(&n, wname, PATH_MAX + 1, dirname, PATH_MAX + 1);
		if (error)
			goto exit_failure;

		/* Open directory stream using wide-character name */
		dirp->wdirp = _wopendir(wname);
		if (!dirp->wdirp)
			goto exit_failure;

		/* Success */
		return dirp;

		/* Failure */
	exit_failure:
		free(dirp);
		return NULL;
	}

	/* Read next directory entry */
	static struct dirent*
		readdir(DIR* dirp)
	{
		/*
		 * Read directory entry to buffer.  We can safely ignore the return
		 * value as entry will be set to NULL in case of error.
		 */
		struct dirent* entry;
		(void)readdir_r(dirp, &dirp->ent, &entry);

		/* Return pointer to statically allocated directory entry */
		return entry;
	}

	/*
	 * Read next directory entry into called-allocated buffer.
	 *
	 * Returns zero on success.  If the end of directory stream is reached, then
	 * sets result to NULL and returns zero.
	 */
	static int
		readdir_r(
			DIR* dirp, struct dirent* entry, struct dirent** result)
	{
		/* Read next directory entry */
		WIN32_FIND_DATAW* datap = dirent_next(dirp->wdirp);
		if (!datap) {
			/* No more directory entries */
			*result = NULL;
			return /*OK*/0;
		}

		/* Attempt to convert file name to multi-byte string */
		size_t n;
		int error = wcstombs_s(
			&n, entry->d_name, PATH_MAX + 1,
			datap->cFileName, PATH_MAX + 1);

		/*
		 * If the file name cannot be represented by a multi-byte string, then
		 * attempt to use old 8+3 file name.  This allows the program to
		 * access files although file names may seem unfamiliar to the user.
		 *
		 * Be ware that the code below cannot come up with a short file name
		 * unless the file system provides one.  At least VirtualBox shared
		 * folders fail to do this.
		 */
		if (error && datap->cAlternateFileName[0] != '\0') {
			error = wcstombs_s(
				&n, entry->d_name, PATH_MAX + 1,
				datap->cAlternateFileName, PATH_MAX + 1);
		}

		if (!error) {
			/* Length of file name excluding zero terminator */
			entry->d_namlen = n - 1;

			/* Determine file type */
			DWORD attr = datap->dwFileAttributes;
			if ((attr & FILE_ATTRIBUTE_DEVICE) != 0)
				entry->d_type = DT_CHR;
			else if ((attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
				entry->d_type = DT_LNK;
			else if ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
				entry->d_type = DT_DIR;
			else
				entry->d_type = DT_REG;

			/* Get offset of next file */
			datap = dirent_next(dirp->wdirp);
			if (datap) {
				/* Compute 31-bit hash of the next directory entry */
				entry->d_off = dirent_hash(datap);

				/* Push the next directory entry back to cache */
				dirp->wdirp->cached = 1;
			}
			else {
				/* End of directory stream */
				entry->d_off = (long)((~0UL) >> 1);
			}

			/* Reset fields */
			entry->d_ino = 0;
			entry->d_reclen = sizeof(struct dirent);
		}
		else {
			/*
			 * Cannot convert file name to multi-byte string so construct
			 * an erroneous directory entry and return that.  Note that
			 * we cannot return NULL as that would stop the processing
			 * of directory entries completely.
			 */
			entry->d_name[0] = '?';
			entry->d_name[1] = '\0';
			entry->d_namlen = 1;
			entry->d_type = DT_UNKNOWN;
			entry->d_ino = 0;
			entry->d_off = -1;
			entry->d_reclen = 0;
		}

		/* Return pointer to directory entry */
		*result = entry;
		return /*OK*/0;
	}

	/* Close directory stream */
	static int
		closedir(DIR* dirp)
	{
		int ok;

		if (!dirp)
			goto exit_failure;

		/* Close wide-character directory stream */
		ok = _wclosedir(dirp->wdirp);
		dirp->wdirp = NULL;

		/* Release multi-byte character version */
		free(dirp);
		return ok;

	exit_failure:
		/* Invalid directory stream */
		dirent_set_errno(EBADF);
		return /*failure*/-1;
	}

	/* Rewind directory stream to beginning */
	static void
		rewinddir(DIR* dirp)
	{
		if (!dirp)
			return;

		/* Rewind wide-character string directory stream */
		_wrewinddir(dirp->wdirp);
	}

	/* Get position of directory stream */
	static long
		_wtelldir(_WDIR* dirp)
	{
		if (!dirp || dirp->handle == INVALID_HANDLE_VALUE) {
			dirent_set_errno(EBADF);
			return /*failure*/-1;
		}

		/* Read next file entry */
		WIN32_FIND_DATAW* datap = dirent_next(dirp);
		if (!datap) {
			/* End of directory stream */
			return (long)((~0UL) >> 1);
		}

		/* Store file entry to cache for readdir() */
		dirp->cached = 1;

		/* Return the 31-bit hash code to be used as stream position */
		return dirent_hash(datap);
	}

	/* Get position of directory stream */
	static long
		telldir(DIR* dirp)
	{
		if (!dirp) {
			dirent_set_errno(EBADF);
			return -1;
		}

		return _wtelldir(dirp->wdirp);
	}

	/* Seek directory stream to offset */
	static void
		_wseekdir(_WDIR* dirp, long loc)
	{
		if (!dirp)
			return;

		/* Directory must be open */
		if (dirp->handle == INVALID_HANDLE_VALUE)
			goto exit_failure;

		/* Ensure that seek position is valid */
		if (loc < 0)
			goto exit_failure;

		/* Restart directory stream from the beginning */
		FindClose(dirp->handle);
		if (!dirent_first(dirp))
			goto exit_failure;

		/* Reset invalid flag so that we can read from the stream again */
		dirp->invalid = 0;

		/*
		 * Read directory entries from the beginning until the hash matches a
		 * file name.  Be ware that hash code is only 31 bits longs and
		 * duplicates are possible: the hash code cannot return the position
		 * with 100.00% accuracy! Moreover, the method is slow for large
		 * directories.
		 */
		long hash;
		do {
			/* Read next directory entry */
			WIN32_FIND_DATAW* datap = dirent_next(dirp);
			if (!datap) {
				/*
				 * End of directory stream was reached before finding
				 * the requested location.  Perhaps the file in
				 * question was deleted or moved out of the directory.
				 */
				goto exit_failure;
			}

			/* Does the file name match the hash? */
			hash = dirent_hash(datap);
		} while (hash != loc);

		/*
		 * File name matches the hash!  Push the directory entry back to cache
		 * from where next readdir() will return it.
		 */
		dirp->cached = 1;
		dirp->invalid = 0;
		return;

	exit_failure:
		/* Ensure that readdir will return NULL */
		dirp->invalid = 1;
	}

	/* Seek directory stream to offset */
	static void
		seekdir(DIR* dirp, long loc)
	{
		if (!dirp)
			return;

		_wseekdir(dirp->wdirp, loc);
	}

	/* Scan directory for entries */
	static int
		scandir(
			const char* dirname, struct dirent*** namelist,
			int (*filter)(const struct dirent*),
			int (*compare)(const struct dirent**, const struct dirent**))
	{
		int result;

		/* Open directory stream */
		DIR* dir = opendir(dirname);
		if (!dir) {
			/* Cannot open directory */
			return /*Error*/ -1;
		}

		/* Read directory entries to memory */
		struct dirent* tmp = NULL;
		struct dirent** files = NULL;
		size_t size = 0;
		size_t allocated = 0;
		while (1) {
			/* Allocate room for a temporary directory entry */
			if (!tmp) {
				tmp = (struct dirent*)malloc(sizeof(struct dirent));
				if (!tmp)
					goto exit_failure;
			}

			/* Read directory entry to temporary area */
			struct dirent* entry;
			if (readdir_r(dir, tmp, &entry) != /*OK*/0)
				goto exit_failure;

			/* Stop if we already read the last directory entry */
			if (entry == NULL)
				goto exit_success;

			/* Determine whether to include the entry in results */
			if (filter && !filter(tmp))
				continue;

			/* Enlarge pointer table to make room for another pointer */
			if (size >= allocated) {
				/* Compute number of entries in the new table */
				size_t num_entries = size * 2 + 16;

				/* Allocate new pointer table or enlarge existing */
				void* p = realloc(files, sizeof(void*) * num_entries);
				if (!p)
					goto exit_failure;

				/* Got the memory */
				files = (dirent**)p;
				allocated = num_entries;
			}

			/* Store the temporary entry to ptr table */
			files[size++] = tmp;
			tmp = NULL;
		}

	exit_failure:
		/* Release allocated entries */
		for (size_t i = 0; i < size; i++) {
			free(files[i]);
		}

		/* Release the pointer table */
		free(files);
		files = NULL;

		/* Exit with error code */
		result = /*error*/ -1;
		goto exit_status;

	exit_success:
		/* Sort directory entries */
		if (size > 1 && compare) {
			qsort(files, size, sizeof(void*),
				(int (*) (const void*, const void*)) compare);
		}

		/* Pass pointer table to caller */
		if (namelist)
			*namelist = files;

		/* Return the number of directory entries read */
		result = (int)size;

	exit_status:
		/* Release temporary directory entry, if we had one */
		free(tmp);

		/* Close directory stream */
		closedir(dir);
		return result;
	}

	/* Alphabetical sorting */
	static int
		alphasort(const struct dirent** a, const struct dirent** b)
	{
		return strcoll((*a)->d_name, (*b)->d_name);
	}

	/* Sort versions */
	static int
		versionsort(const struct dirent** a, const struct dirent** b)
	{
		return strverscmp((*a)->d_name, (*b)->d_name);
	}

	/* Compare strings */
	static int
		strverscmp(const char* a, const char* b)
	{
		size_t i = 0;
		size_t j;

		/* Find first difference */
		while (a[i] == b[i]) {
			if (a[i] == '\0') {
				/* No difference */
				return 0;
			}
			++i;
		}

		/* Count backwards and find the leftmost digit */
		j = i;
		while (j > 0 && isdigit(a[j - 1])) {
			--j;
		}

		/* Determine mode of comparison */
		if (a[j] == '0' || b[j] == '0') {
			/* Find the next non-zero digit */
			while (a[j] == '0' && a[j] == b[j]) {
				j++;
			}

			/* String with more digits is smaller, e.g 002 < 01 */
			if (isdigit(a[j])) {
				if (!isdigit(b[j])) {
					return -1;
				}
			}
			else if (isdigit(b[j])) {
				return 1;
			}
		}
		else if (isdigit(a[j]) && isdigit(b[j])) {
			/* Numeric comparison */
			size_t k1 = j;
			size_t k2 = j;

			/* Compute number of digits in each string */
			while (isdigit(a[k1])) {
				k1++;
			}
			while (isdigit(b[k2])) {
				k2++;
			}

			/* Number with more digits is bigger, e.g 999 < 1000 */
			if (k1 < k2)
				return -1;
			else if (k1 > k2)
				return 1;
		}

		/* Alphabetical comparison */
		return (int)((unsigned char)a[i]) - ((unsigned char)b[i]);
	}

	/* Convert multi-byte string to wide character string */
#if !defined(_MSC_VER) || _MSC_VER < 1400
	static int
		dirent_mbstowcs_s(
			size_t* pReturnValue, wchar_t* wcstr,
			size_t sizeInWords, const char* mbstr, size_t count)
	{
		/* Older Visual Studio or non-Microsoft compiler */
		size_t n = mbstowcs(wcstr, mbstr, sizeInWords);
		if (wcstr && n >= count)
			return /*error*/ 1;

		/* Zero-terminate output buffer */
		if (wcstr && sizeInWords) {
			if (n >= sizeInWords)
				n = sizeInWords - 1;
			wcstr[n] = 0;
		}

		/* Length of multi-byte string with zero terminator */
		if (pReturnValue) {
			*pReturnValue = n + 1;
		}

		/* Success */
		return 0;
	}
#endif

	/* Convert wide-character string to multi-byte string */
#if !defined(_MSC_VER) || _MSC_VER < 1400
	static int
		dirent_wcstombs_s(
			size_t* pReturnValue, char* mbstr,
			size_t sizeInBytes, const wchar_t* wcstr, size_t count)
	{
		/* Older Visual Studio or non-Microsoft compiler */
		size_t n = wcstombs(mbstr, wcstr, sizeInBytes);
		if (mbstr && n >= count)
			return /*error*/1;

		/* Zero-terminate output buffer */
		if (mbstr && sizeInBytes) {
			if (n >= sizeInBytes) {
				n = sizeInBytes - 1;
			}
			mbstr[n] = '\0';
		}

		/* Length of resulting multi-bytes string WITH zero-terminator */
		if (pReturnValue) {
			*pReturnValue = n + 1;
		}

		/* Success */
		return 0;
	}
#endif

	/* Set errno variable */
#if !defined(_MSC_VER) || _MSC_VER < 1400
	static void
		dirent_set_errno(int error)
	{
		/* Non-Microsoft compiler or older Microsoft compiler */
		errno = error;
	}
#endif

#ifdef __cplusplus
}
#endif
#endif /*DIRENT_H*/
// ==================== DIRENT END ======================
#endif

#else
#include <unistd.h> 
#include <dirent.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32 
#define __FS_STD_DIREND '\\'
#define __FS_NSTD_DIREND '/'
#else
#define __FS_STD_DIREND '/'
#define __FS_NSTD_DIREND '\\'
#endif

static fs_t fs_custom_projdir = { 0 };

typedef struct {
	int     len;
	int     max;
	fs_t* entries;
} fs_ls_entries_left_t;

void fs_validate(fs_t in) {
	size_t str_len = strlen(in);
	for (int i = 0; i < str_len; i++)
		if (in[i] == __FS_NSTD_DIREND) in[i] = __FS_STD_DIREND;

	if (!fs_exist(in)) return;

	if (fs_isfile(in)) {
		if (in[str_len - 1] != __FS_STD_DIREND) return;
		in[str_len - 1] = '\0';
	}
	else {
		if (in[str_len - 1] == __FS_STD_DIREND) return;
		in[str_len] = __FS_STD_DIREND;
		in[str_len + 1] = '\0';
	}
}

void fs_parent(fs_t dest, fs_t path) {
	size_t len = strlen(path);
	strcpy(dest, path);
	for (size_t i = len; i > 0; i--) {
		if (dest[i] == __FS_STD_DIREND) {
			dest[i + 1] = '\0';
			return;
		}
	}
}

void fs_app(fs_t dest) {
#ifdef _WIN32
	GetModuleFileNameA(NULL, dest, __STD_FS_MAX);
#else
	readlink("/proc/self/exe", dest, __STD_FS_MAX);
#endif
}

void fs_dir(fs_t dest) {
#ifdef _WIN32
	char* buffer;
	if ((buffer = _getcwd(NULL, 0)) == NULL) {
		memset(dest, 0, sizeof(fs_t));
		return;
	}

	strcpy(dest, buffer);
	free(buffer);

#else
	getcwd(dest, __STD_FS_MAX);
#endif
	fs_validate(dest);
}

bool fs_isfile(fs_t dest) {
	struct stat path_stat = { 0 };
	stat(dest, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

void fs_usr(fs_t dest) {
#ifdef _WIN32
	SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, dest);
#else
	const char* home = getenv("HOME");
	if (home) strcpy(dest, home);
	else {
		const char* user_profile = getenv("USERPROFILE");
		if (user_profile) {
			strcpy(dest, user_profile);
		}
		else {
			dest[0] = '\0';
		}
	}
#endif
}

void fs_root(fs_t dest) {
#ifdef _WIN32
	strcpy(dest, "/");
#else
	strcpy(dest, "C:\\");
#endif
}

void fs_add(fs_t dest, const char* extra) {
	strcat(dest, extra);
}

bool fs_exist(fs_t dest) {
#ifdef _WIN32
	DWORD dwAttrib = GetFileAttributesA(dest);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
	if (fs_isfile(dest)) return access(dest, F_OK) == 0;
	else return opendir(dest) != NULL;
#endif
}


bool fs_cd(fs_t dest, const char* src) {
	strcat(dest, src);
	fs_validate(dest);
	return fs_exist(dest);
}

bool fs_find(fs_t path, fs_t child) {
	fs_t tmp = { 0 };
	fs_cd(tmp, path);
	fs_validate(tmp);
	fs_add(tmp, child);
	return fs_exist(tmp);
}

void fs_get(fs_t path, fs_t child) {
	fs_validate(path);
	fs_add(path, child);

	if (fs_exist(path)) {
		fs_validate(path);
		return;
	}

	if (path[strlen(path) - 1] == __FS_STD_DIREND) {
		#ifdef _WIN32 
			int result = _mkdir(path);
		#else
			int result = mkdir(path, 0700);
		#endif
		return;
	}

	FILE* f = fopen(path, "w");
	if (!f) return;
	fclose(f);
}


void fs_ext(fs_t out, fs_t path) {
	const char* dot = strrchr(path, '.');
	if (!dot || dot == path) {
		memset(out, 0, sizeof(fs_t));
		return;
	}

	strcpy(out, dot + 1);
}

void fs_name(fs_t dest, fs_t path) {
	const char* filename = strrchr(path, __FS_STD_DIREND);
	if (filename) {
		filename++;
	}
	else {
		filename = path;
	}

	const char* ext = strrchr(filename, '.');
	size_t length;
	if (ext) {
		length = ext - filename;
	}
	else {
		length = strlen(filename);
	}

	if (length >= __STD_FS_MAX) {
		length = __STD_FS_MAX - 1;
	}

	strncpy(dest, filename, length);
	dest[length] = '\0';
}

void fs_rel(fs_t dest, fs_t ancestor, fs_t descandant) {
	memset(dest, 0, __STD_FS_MAX);
	int tl = 0;
	int al = (int)strlen(ancestor);
	int ad = (int)strlen(descandant);
	int mx = MAX(al, ad);
	for (int i = 0; i < mx; i++) {
		if (i < ad && i < al && ancestor[i] == descandant[i]) continue;
		dest[tl] = descandant[i];
		tl++;
	}
}

int fs_ls(const fs_t inpath, fs_t* out, const char* exc_ext, bool recursive, const char* name) {
	fs_t path = { 0 };
	strcpy(path, inpath);
	fs_validate(path);

	struct dirent* dir;
	fs_t extension = { 0 };
	fs_t subdir = { 0 };
	int len = 0;
	int too_long = 0;

	fs_ls_entries_left_t left = { 0 };
	left.max = 64;
	left.len = 0;
	left.entries = malloc(sizeof(fs_t) * left.max);

	if (!left.entries) return 0;

	strcpy(left.entries[left.len++], path);

	while (left.len > 0) {
		strcpy(path, left.entries[--left.len]);
		DIR* d = opendir(path);

		if (!d) {
			continue;
		}

		int path_len = (int)strlen(path);

		while ((dir = readdir(d)) != NULL) {
			if (strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0) {
				continue;
			}

			if (name && strcmp(dir->d_name, name) != 0) {
				continue;
			}

			int name_len = (int)strlen(dir->d_name);
			if (path_len + name_len >= __STD_FS_MAX - 1) {
				too_long++;
				continue;
			}

			snprintf(subdir, sizeof(fs_t), "%s%s", path, dir->d_name);
			fs_validate(subdir);
			fs_ext(extension, dir->d_name);

			bool isfile = fs_isfile(subdir);

			if (!isfile && recursive && strlen(extension) == 0) {
				if (left.len >= left.max) {
					fs_t* tmp = realloc(left.entries, sizeof(fs_t) * left.max * 2);
					if (!tmp) {
						free(left.entries);
						closedir(d);
						printf("realloc failed! returning 0 from fs_ls()\n");
						return 0;
					}
					left.entries = tmp;
					left.max = left.max * 3 / 2;
				}

				strcpy(left.entries[left.len++], subdir);
			}
			else {
				if (exc_ext && strcmp(extension, exc_ext) != 0) {
					continue;
				}

				if (out) {
					strcpy(out[len], subdir);
				}
				len++;
			}
		}
		closedir(d);
	}

	if (too_long > 0) {
		printf("%d paths where longer than %d! Those paths were skipped...\n", too_long, __STD_FS_MAX);
	}

	free(left.entries);
	return len;
}

void fs_pop(fs_t path) {
	size_t len = strlen(path);

	while (len > 0 && path[len - 1] == __FS_STD_DIREND) {
		path[--len] = '\0';
	}

	char* last_separator = strrchr(path, __FS_STD_DIREND);
	if (last_separator != NULL) {
		*last_separator = '\0';
	}
}

void fs_mkdir(fs_t dir) {
	fs_validate(dir);

	if (fs_exist(dir)) return;

	char cmd[512] = { 0 };
	strcpy(cmd, "mkdir ");
	strcat(cmd, dir);
	system(cmd);
}

size_t fs_readf(fs_t file, char* buffer) {
	FILE* f = fopen(file, "rb");

	if (!f) {
		printf("failed to read file %s\n", file);
		return 0;
	}
	
	fseek(f, 0, SEEK_END);
	size_t len = (size_t)ftell(f);
	fseek(f, 0, SEEK_SET);

	if (!buffer) {
		fclose(f);
		return len;
	}

	fread(buffer, 1, len, f);
	fclose(f);

	buffer[len] = '\0';
	return len;
}

void fs_writef(fs_t file, char* buffer, size_t size) {
	FILE* f = fopen(file, "wb");

	if (!f) {
		printf("failed to write file %s\n", file);
		return;
	}

	if (!buffer) {
		printf("write buffer was NULL\n");
		fclose(f);
		return;
	}

	fwrite(buffer, size, 1, f);
	fclose(f);
	return;
}

void fs_projdir(fs_t path) {

	if (strlen(fs_custom_projdir) > 0) {
		strcpy(path, fs_custom_projdir);
		fs_validate(path);
	}
	else {
		strcpy(path, PROJ_ROOT);
		fs_validate(path);
	}
}

void fs_new(fs_t dest, const char* str) {
	strcpy(dest, str);
}

void fs_last(const char* path, char* out) {
	const char* slash_back = strrchr(path, '\\');
	const char* slash_fwd = strrchr(path, '/');

	const char* last_slash = (slash_back > slash_fwd) ? slash_back : slash_fwd;

	if (last_slash == NULL) {
		strcpy(out, path);
	}
	else {
		strcpy(out, last_slash + 1);
	}
}