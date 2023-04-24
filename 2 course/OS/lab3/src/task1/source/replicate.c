#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>


void reverse(char* str) {
	char tmp;
	size_t len = strlen(str) - 1;
	for (size_t i = 0, j = len; i < j; ++i, --j) {
		tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}
}

void copyDir(const char* oldDir) {
	char* newDir = (char*)malloc(strlen(oldDir) + 1);
	strcpy(newDir, oldDir);
	reverse(newDir);

	DIR* dp;
	struct dirent* entry;
	struct stat statbuf;
	if ((dp = opendir(oldDir)) == NULL) {
		//printf("Ошибка открытия исходного каталога - %s\n", oldDir);
		perror("Ошибка открытия исходного каталога");
		exit(1);
	}
	if (mkdir(newDir, 0777) == -1) {
		perror("error");
		exit(1);					// создали наш новый каталог
	}

	char srcPath[512], dstPath[512];
	while ((entry = readdir(dp)) != NULL) {
		sprintf(srcPath, "%s/%s", oldDir, entry->d_name); // путь к исходному файлу
		if (lstat(srcPath, &statbuf) == -1) {
			perror("error");
			exit(1);
		} // используем lstat, т.к. иначе софтлинк вернет инфу о себе
		
		if (S_ISREG(statbuf.st_mode)) { // проверка файлы на регулярность
			//printf("Внутри S_ISREG - %s\n", entry->d_name);
			char* dstFile = (char*)malloc(strlen(entry->d_name) + 1);
			strcpy(dstFile, entry->d_name);
			reverse(dstFile);

			// копирование содержимого в обратном порядке
			sprintf(dstPath, "%s/%s", newDir, dstFile);		  // путь к обратному файлу
			int srcFD = open(srcPath, O_RDONLY);
			int dstFD = open(dstPath, O_WRONLY | O_CREAT, 0644);
			if (srcFD == -1 || dstFD == -1) {
				perror("error");
				exit(1);
			}
			
			char buffer[2];
			lseek(srcFD, 0, SEEK_END);
			off_t size = lseek(srcFD, 0, SEEK_CUR);
			//printf("size = %ld\n", size);
			lseek(srcFD, 0, SEEK_SET);
			for (int i = size - 1, j = 0; i >= 0; --i, ++j) {
				lseek(srcFD, i, SEEK_SET);
				read(srcFD, buffer, 1);
				//lseek(dstFD, j, SEEK_SET);
				write(dstFD, buffer, 1);
			}
			printf("Файл - %s успешно скопирован\n", srcPath);

			close(srcFD);
			close(dstFD);
			free(dstFile);
		}
	}
	///
	
	closedir(dp);
	free(newDir);

	printf("Каталог %s успешно скопирован <задом наперед>\n", oldDir);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("В аргументах программы укажите название каталога\n");
		exit(1);
	}

	copyDir(argv[1]);

	return 0;
}
