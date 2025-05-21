#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
    
double f(double x){
    return 4.0/(x*x + 1);
}

typedef struct {
    int id;
    int threads_num;
    double width;
    int rect_num;
    double* results;
}ThreadData;

void* calculate(void* arg) {
    double a = 0.0;
    double b = 1.0;
    double calc_res = 0.0;
    ThreadData* data = (ThreadData*)arg;
    int start = (data->rect_num / data->threads_num) * data->id;
    int end = (data->rect_num / data->threads_num) * (data->id + 1);

    for(int i = start; i < end; i++) {
        double x = i * data->width;
        calc_res += f(x) * data->width;
    }

    data->results[data->id] = calc_res;

    return NULL;
}

int main(int argc, char *argv[]) // width, n
{
    if(argc != 3) {
        return 1;
    }
    
    double width = atof(argv[1]);
    int n = atoi(argv[2]);

    double a = 0.0;
    double b = 1.0;

    int rect_num = (int)((b - a) / width);

    printf("Width = %.8f, Rectangles = %d\n", width, rect_num);
    printf("Wątki\tWynik całki\t\tCzas [s]\n");
    printf("-----\t-----------------------\t--------\n");

    for(int k = 0; k <= n; k++){
        struct timeval start, end;
        gettimeofday(&start, NULL);

        pthread_t tid[k];
        ThreadData data[k];
        double results[k];

        double integral_res = 0.0;

        for(int i = 0; i < k; i++){
            data[i].id = i;
            data[i].threads_num = k;
            data[i].width = width;
            data[i].rect_num = rect_num;
            data[i].results = results;
            pthread_create(&tid[i], NULL, calculate, &data[i]);
        }

        for (int i = 0; i < k; i++) {
            pthread_join(tid[i], NULL);
        }
        for(int i = 0; i < k; i++){
            integral_res += results[i];
        }

        gettimeofday(&end, NULL);
        double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        printf("%d\t%.15f\t%.3f\n", k, integral_res, time_spent);
    }
    return 0;
}