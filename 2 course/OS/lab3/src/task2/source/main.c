#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>

void createDir(char* name) {
	if (mkdir(name, 0744) == -1) {
		perror("error");
		exit(1);
	}

	printf("Каталог - %s успешно создан\n", name);
}

void printDir(char* name) {
	DIR* dp;
	struct dirent* entry;

	if ((dp = opendir(name)) != NULL) {
		while ((entry = readdir(dp)) != NULL) {
			printf("%s\n", entry->d_name);
		}
		closedir(dp);
	}
	else {
		perror("error");
		exit(1);
	}
}

void removeDir(const char* name) {
	if ((rmdir(name)) == -1) {
		//printf("Ошибка удаления директории - %s\n", name);
		perror("error");
		exit(1);
	}
	
	printf("Директория %s удалена успешна\n", name);
}

void createFile(char* name) {
	int fd = creat(name, 0644);
	if (fd == -1) {
		perror("error");
		exit(1);
	}

	printf("Файл %s успешно создан\n", name);
	close(fd);
}

void printFile(char* name) {
	int fd = open(name, O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(1);
	}
	char buffer[1024];
	ssize_t size = 0;
	while ((size = read(fd, buffer, sizeof(buffer))) > 0) {
		if (write(1, buffer, size) != size) {
			perror("error");
			exit(1);
		}
	}
	if (size == -1) {
		perror("error");
		exit(1);
	}

	printf("Файл %s успешно прочитан и напечатан\n", name);
	close(fd);
}

void removeFile(char* name) {
	if (remove(name) == -1) {
		perror("error");
		exit(1);
	}

	printf("Файл %s успешно удалён\n", name);
}

void createSymbLink(char* srcPath) {
	if (symlink(srcPath, "softlink1") == -1) {
		perror("error");
		exit(1);
	}
}

void printSymbLink(char* name) {
	char buffer[4096];
	ssize_t size = 0;
	if ((size = readlink(name, buffer, sizeof(buffer))) == -1) {
		perror("error");
		exit(1);
	}
	buffer[size] = '\0';

	printf("Содержание символьной ссылки:\n%s\n", buffer);
}

void printLinkFile(char* name) {
	char buffer[4096];
	ssize_t size = 0;
	if ((size = readlink(name, buffer, sizeof(buffer))) == -1) {
		perror("error");
		exit(1);
	}
	buffer[size] = '\0';

	printFile(buffer);
}

void removeSymbLink(char* name) {
	if (unlink(name) == -1) {
		perror("error");
		exit(1);
	}

	printf("Ссылка %s удалена\n", name);
}

void createHardLink(char* name) {
	if (link(name, "hardlink1") == -1) {
		perror("error");
		exit(1);
	}

	printf("Жёсткая ссылка %s успешно создана\n", name);
}

void removeHardLink(char* name) {
	if(unlink(name) == -1) {
		perror("error");
		exit(1);
	}

	printf("Ссылка %s удалена\n", name);
}

void showInfo(char* name) {
	struct stat statbuf;

	if (stat(name, &statbuf) == -1) {
		perror("error");
		exit(1);
	}

	printf("Права доступа к файлу %s: %o\n", name, statbuf.st_mode);
	printf("Количество жёстких ссылок на файл %s: %ld\n", name, statbuf.st_nlink);
}

void changeMod(char* name) {
	if (chmod(name, 0600) == -1) {
		perror("error");
		exit(1);
	}

	printf("Права файла %s успешно изменены\n", name);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Укажите аргумент\n");
		exit(1);
	}
	
	char* func = basename(argv[0]);
	//printf("%s\n", func);
	if (strcmp(func, "createDir") == 0) {
		createDir(argv[1]);
	} else if (strcmp(func, "printDir") == 0) {
		printDir(argv[1]);
	} else if (strcmp(func, "removeDir") == 0) {
		removeDir(argv[1]);
	} else if (strcmp(func, "createFile") == 0) {
		createFile(argv[1]);
	} else if (strcmp(func, "printFile") == 0) {
		printFile(argv[1]);
	} else if (strcmp(func, "removeFile") == 0) {
		removeFile(argv[1]);
	} else if (strcmp(func, "createSymbLink") == 0) {
		createSymbLink(argv[1]);
	} else if (strcmp(func, "printSymbLink") == 0) {
		printSymbLink(argv[1]);
	} else if (strcmp(func, "printLinkFile") == 0) {
		printLinkFile(argv[1]);
	} else if (strcmp(func, "removeSymbLink") == 0) {
		removeSymbLink(argv[1]);
	} else if (strcmp(func, "createHardLink") == 0) {
		createHardLink(argv[1]);
	} else if (strcmp(func, "removeHardLink") == 0) {
		removeHardLink(argv[1]);
	} else if (strcmp(func, "showInfo") == 0) {
		showInfo(argv[1]);
	} else if (strcmp(func, "changeMod") == 0) {
		changeMod(argv[1]);
	} else {
		printf("Что-то пошло не так, я не нашёл %s функции\n", func);
	}

	return 0;
}
