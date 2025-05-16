#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

double f(double x){
    return 4.0/(x*x + 1);
}

double calculate(double a, double b, int n) {
    double width = (b - a) / n;
    double res = 0.0;
    for (int i; i < n; i++) {
        double x = a + i * width;
        res += f(x) * width;
    }
    return res;
}

int main(){
	int fd_read = open("potok1", O_RDONLY);
	if (fd_read == -1){
        perror("potok1");
		exit(1);
	}
	double a, b;
	read(fd_read, &a, sizeof(double));
	read(fd_read, &b, sizeof(double));
	close(fd_read);
	double res = calculate(a, b, 100000);
    int fd_write = open("potok2", O_WRONLY);
	if(fd_write == -1){
        perror("potok2");
		exit(1);
	}
	write(fd_write, &res, sizeof(double));
	printf("Policzono calke\n");
	close(fd_write);
	return 0;
}
