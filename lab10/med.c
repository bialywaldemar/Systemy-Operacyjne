#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

int waiting_patients = 0;
int patients_remaining = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t doctor_sleep_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consultation_done_cond = PTHREAD_COND_INITIALIZER;


typedef struct {
    int id;
} PatientData;

void print_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("[%ld.%06ld] - ", tv.tv_sec, tv.tv_usec);
}


void* patient_fun(void* arg) {
    PatientData* data = (PatientData*)arg;
    int id = data->id;
    print_time();
    int delay = rand() % 4 + 2;
    printf("Pacjent (%d): Ide do szpitala, bede za %d s\n", id, delay);
    sleep(delay);
    while (1) {
        pthread_mutex_lock(&mutex);
        if (waiting_patients >= 3) {
            // idzie na spacer
            pthread_mutex_unlock(&mutex);
            int walk = rand() % 3 + 1;
            print_time();
            printf("Pacjent (%d): za duzo pacjentow, wracam pozniej za %d s\n", id, walk);
            sleep(walk);
            continue;
        }

        else if (waiting_patients == 2) {
            // jest trzeci - konsultacja lekarza
            print_time();
            printf("Pacjent (%d): czeka %d pacjentow na lekarza\n", id, waiting_patients);
            print_time();
            printf("Pacjent (%d): budze lekarza\n", id);
            waiting_patients += 1;
            pthread_cond_signal(&doctor_sleep_cond);
            pthread_cond_wait(&consultation_done_cond, &mutex);
            pthread_mutex_unlock(&mutex);
            break;
        }

        else {
            // dolacza do poczekalni
            print_time();
            waiting_patients += 1;
            printf("Pacjent (%d): czeka %d pacjentow na lekarza\n", id, waiting_patients);
            pthread_cond_wait(&consultation_done_cond, &mutex);
            pthread_mutex_unlock(&mutex);
            break;
        }
    }   

    return NULL;
}

void* medic_fun(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // czeka na pacjentow
        while(waiting_patients < 3 && patients_remaining > 0) {
            print_time();
            printf("Lekarz: zasypiam\n");
            pthread_cond_wait(&doctor_sleep_cond, &mutex);
        }
        if (patients_remaining == 0) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        print_time();
        printf("Lekarz: budze sie\n");
        print_time();
        printf("Lekarz: konsultuje %d pacjentow\n", waiting_patients);
        pthread_mutex_unlock(&mutex);
        int consulting = rand() % 3 + 2;
        sleep(consulting);

        pthread_mutex_lock(&mutex);
        patients_remaining -= 3;
        waiting_patients = 0;

        pthread_cond_broadcast(&consultation_done_cond);
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    int patients = atoi(argv[1]);
    patients_remaining = patients;
    pthread_t tid[patients];
    PatientData data[patients];
    srand(time(NULL));

    pthread_t doctor_tid;
    pthread_create(&doctor_tid, NULL, medic_fun, NULL);

    for (int i = 0; i < patients; i++) {
        data[i].id = i + 1;
        pthread_create(&tid[i], NULL, patient_fun, &data[i]);
    }
    


    for (int i = 0; i < patients; i++) {
        pthread_join(tid[i], NULL);
    }
    pthread_join(doctor_tid, NULL);

    
}