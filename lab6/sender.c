#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	double a, b;
	printf("Podaj a: ");
    	scanf("%lf", &a);
    	printf("Podaj b: ");
    	scanf("%lf", &b);
	int fd_write = open("potok1", O_WRONLY);
	if(fd_write == -1) {
		perror("potok1");
		exit(1);
	}
	write(fd_write, &a, sizeof(double));
	write(fd_write, &b, sizeof(double));
	close(fd_write);
	int fd_read = open("potok2", O_RDONLY);
	if(fd_read == -1){
		exit(1);
	}
	double res;
	read(fd_read, &res, sizeof(double));
	close(fd_read);

	printf("Wynik calki:  %lf\n", res);
	return 0;
}
