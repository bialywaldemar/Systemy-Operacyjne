#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
double f(double x){
    return 4.0/(x*x + 1);
}


int main(int argc, char *argv[]) // width, n
{
    double width = atof(argv[1]);
    int n = atoi(argv[2]);

    for(int k = 0; k <= n; k++) {
        // clock_t start = clock();
        struct timeval start, end;
        gettimeofday(&start, NULL);

        double a = 0.0;
        double b = 1.0;
        double res = 0.0;

        int pipes_num = k;
        int pipes[pipes_num][2];
        pid_t pids[pipes_num];
        int rectangles_num = (int)((b - a) / width);

        for (int i = 0; i < pipes_num; i++){
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        for (int i = 0; i < pipes_num; i++){
            pids[i] = fork();
            if(pids[i] == -1){
                perror("fork");
                exit(1);
            }
            if (pids[i] == 0){
                //potomny
                close(pipes[i][0]);
                double curr_res = 0.0;
                int start_i = i * (rectangles_num / k);
                int end_i = (i + 1) * (rectangles_num / k);
                if (i == pipes_num){
                    end_i = rectangles_num;
                }
                for(int j = start_i; j < end_i; j++){
                    double x =  a + j * width;
                    curr_res += f(x) * width;
                }
                write(pipes[i][1], &curr_res, sizeof(curr_res));
                close(pipes[i][1]);
                exit(0);
            }
            else {
                close(pipes[i][1]);
            }
        }

        for (int i = 0; i < pipes_num; i++){
            double received;
            read(pipes[i][0], &received, sizeof(received));
            res += received;
            close(pipes[i][0]);
        }

        for(int i = 0; i < pipes_num; i++) {
            waitpid(pids[i], NULL, 0);
        }
        // clock_t end = clock();
        // double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
        gettimeofday(&end, NULL);
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;
        printf("\nDLA K = %d PROCESOW:\n", k);
        printf("Wynik calki: %lf\n", res);
        printf("Czas wykonania: %lf sekund\n", elapsed);
    }
    return 0;
}