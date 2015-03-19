/*2. Найти в заданном каталоге (аргумент 1 командной строки) и всех его
подкаталогах заданный файл (аргумент 2 командной строки). Вывести на консоль
полный путь к файлу, размер, дату создания, права доступа, номер индексного
дескриптора. Вывести также общее количество просмотренных каталогов и файлов.*/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#define STR_SIZE 1024

char *execfile;
int count_dir = 0;
int count_files = 0;
FILE *file;


void getprop(struct stat buf, char *filepath)
{
	char *string;
	struct 	tm lt;
	string=(char *)malloc(sizeof(char)*STR_SIZE);
	localtime_r(&buf.st_mtime,&lt);
	strftime(string, sizeof(char*)*STR_SIZE, "%c", &lt);
					
	fprintf(file,"%s %ld %lo %ld %s\n", 
		filepath, buf.st_size, (unsigned long)buf.st_mode,
		(long)buf.st_ino, string);
	free(string);	
}

int findindir(char *folder, char *filename, int *file_exist)
{
	DIR *dfd;
	struct dirent *dir;
	struct stat buf;
	char *fullpath, *filepath;
	int ret, temperrno;
	setlocale(LC_TIME,"ru_RU.UTF-8");

	fullpath=(char *)malloc(sizeof(char)*STR_SIZE);
	filepath=(char *)malloc(sizeof(char)*STR_SIZE);

	if ((dfd=opendir(folder)) == NULL)
	{
		fprintf(stderr, "%s : %s : %s\n", execfile, strerror(errno), realpath(folder,fullpath));
		return errno;
	}

	while((dir=readdir(dfd)) != NULL)
	{
		if (dir->d_type != DT_DIR)
		{
			if (dir->d_type == DT_REG)
				count_files++;

			if (strcmp(filename,dir->d_name) == 0)
			{
				
				if (realpath(folder, filepath) != NULL)
            	{
	            	strcat(filepath,"/");
	            	strcat(filepath,filename);
	            }
	            else
	            {
	            	fprintf(stderr, "%s : %s : %s\n", execfile, strerror(errno), realpath(folder,fullpath));
					return errno;
	            }
				
				if ((ret = stat(filepath, &buf)) == 0 )
				{
					if (*file_exist == 1)
					{						
						if ((file = fopen("result.txt", "a")) != NULL)
						{
							getprop(buf, filepath);
							fclose(file);
						}
						else
						{
							fprintf(stderr, "%s : %s : %s\n", execfile, strerror(errno), filepath);
							return errno;
						}
					}
					else
					{
						if ((file = fopen("result.txt", "w")) != NULL)
						{
							getprop(buf, filepath);
							fclose(file);
							*file_exist = 1;
						}
						else
						{
							fprintf(stderr, "%s : %s : %s\n", execfile, strerror(errno), filepath);
							return errno;
						}
					}
				}
				else
				{
					fprintf(stderr, "%s : %s : %s\n", execfile, strerror(errno), filepath);
					continue;
				}		
			}
		}
		else 
		{
			if (((dir->d_type == DT_DIR)) 
				&& ((strcmp(dir->d_name,".")) != 0) 
				&& ((strcmp(dir->d_name,"..")) != 0))
			{
				count_dir++;
				if (realpath(folder, filepath) != NULL)
				{
					strcat(filepath,"/");
					strcat(filepath,dir->d_name);
				}
				else
	            {
	            	fprintf(stderr, "%s : %s : %s\n", execfile, strerror(errno), realpath(folder,fullpath));
					return errno;
	            }
            	findindir(filepath, filename, file_exist);
			}
		}
		temperrno = errno;
	}
	if (errno != temperrno)
	{
		fprintf(stderr, "%s : %s : %s!\n", execfile, strerror(errno), realpath(folder,fullpath));
		return errno;
	}
	
	closedir(dfd);
	free(fullpath);
	free(filepath);
}

char *getexecfilename(char *arg)
{
	int len = strlen(arg);
	int i = len-1;

	while(arg[i] != '/')
		i--;
	
	int lentemp = len - i;
	char *tempstr = malloc(sizeof(char)*(lentemp));
	int j,k = 0;
	
	i++;
	
	for(j = i; j < len; j++)
		tempstr[k++]=arg[j];
	tempstr[k]=0;

	return tempstr;
} 

int main(int argc, char *argv[])
{
	char *dirname, *filename;
	int c, flag = 0;
	
	execfile = getexecfilename(argv[0]);
	
	filename=argv[2];		
	dirname=argv[1];
	findindir(dirname,filename, &flag);
		
	if (flag == 1)
	{
		if ((file = fopen("result.txt", "r")) != NULL)
		{
			while(1)
			{
				c = fgetc(file);
				if ( feof(file) )
					break;
				printf("%c", c);
			}
			fclose(file);
		}
		else
			fprintf(stderr, "%s : %s : %s\n", execfile, strerror(errno),
				"/home/oleguchok/osisp/laboratory/second/result.txt");//!
	}
	else 
		fprintf(stderr, "%s : File doesn't exist\n", execfile);
	
	printf("Directories: %d; Files: %d\n", count_dir, count_files);
	return 0;
}
