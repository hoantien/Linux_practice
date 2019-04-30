#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FILE	"/dev/led_dev"

int main(void)
{
	char *buff ;
	buff = malloc(2);
	//memset(buff, 0, 20);
	int fd = open(FILE, O_RDONLY);

	if (fd < 0) {
		perror("open\n");
		exit(1);
	}
	read(fd, buff, 2);

	printf("%s\n", buff);
	free(buff);
	close(fd);
	return 0;
}
