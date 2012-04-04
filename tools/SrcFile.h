#ifndef SRC_FILE_H
#define SRC_FILE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct SrcFile
{
	FILE *fp;
	const char *name;
	int line;
	int col;
} SrcFile;

#define SrcFile_error(src_file, ...) \
	fprintf(stderr, "ERROR: ");           \
	fprintf(stderr, __VA_ARGS__);    \
	fprintf(stderr, " in file: \"%s\" at %d:%d\n", src_file->name, src_file->line, src_file->col);

static inline SrcFile *SrcFile_openFile(const char *filename)
{
	SrcFile *f;
	FILE *file = fopen(filename, "r");
	
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: \"%s\" could not be opened in read-mode\n", filename);
		
		return NULL;
	}
	
	f       = malloc(sizeof(SrcFile));
	f->fp   = file;
	/* TODO: Need to copy the *filename here? */
	f->name = filename;
	f->line = 1;
	f->col  = 1;
	
	return f;
}

static inline void SrcFile_free(SrcFile *f)
{
	fclose(f->fp);
	free(f);
}

static inline char SrcFile_getc(SrcFile *f)
{
	return fgetc(f->fp);
}

static inline void SrcFile_ungetc(SrcFile *f, char c)
{
	ungetc(c, f->fp);
}

static inline char SrcFile_peek(SrcFile *f)
{
	char c;
	
	c = fgetc(f->fp);
	ungetc(c, f->fp);
	
	return c;
}

static inline int SrcFile_testc(SrcFile *f, const char chr)
{
	char c;
	
	if((c = fgetc(f->fp)) == chr)
	{
		return 1;
	}
	
	ungetc(c, f->fp);
	return 0;
}

/**
 * Counts the character in the file as read and adds to line
 * or col accordingly.
 */
static inline void SrcFile_count(SrcFile *f, const char chr)
{
	if(chr == '\n') {
		f->line++;
		f->col = 1;
	} else { f->col++; }
}

/**
 * Tests if the *fp contains *str at the beginning of the buffer,
 * if it does, 1 is returned, otherwise 0 and the read characters
 * will be put back.
 */
static inline int SrcFile_testStr(SrcFile *f, const char *str)
{
	char c;
	char *tmp = malloc(sizeof(char) * strlen(str));
	const char *s;
	int i;
	
	for(s = str; *s; ++s)
	{
		/* No need to check for EOF, as EOF is not in str (hopefully) */
		if((c = fgetc(f->fp)) != *s)
		{
			ungetc(c, f->fp);
			for(i = s - str - 1; i >= 0; i--)
			{
				ungetc(tmp[i], f->fp);
			}
			
			free(tmp);
			
			return 0;
		}
		
		tmp[s - str] = *s;
	}
	
	/* Count characters */
	for(i = s - str - 1; i >= 0; i--)
	{
		SrcFile_count(f, tmp[i]);
	}
	
	free(tmp);
	
	return 1;
}

#endif