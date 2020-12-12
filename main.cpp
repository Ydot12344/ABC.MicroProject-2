#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <ctime>
#include <queue>

//очереди
std::queue<int> a1, a2, a3;
//мьютекс
pthread_mutex_t rwlock;

//постановка в очередь
void* funWrite(void* param){
    //начальная задержка для разных писателей
    sleep(*((int*)param));
    //номер покупателя и отдела магазина
    int num, sec;
    while(1) {
        pthread_mutex_lock(&rwlock);
        srand(time(NULL));
        num = *(int*)param;
        sec = rand() % 3 + 1;
        if (num == 1)
            a1.push(num);
        else if (num == 2)
            a2.push(num);
        else
            a3.push(num);
        fprintf(stdout, "Buyer number %d - set in %d section\n", num, sec);
        pthread_mutex_unlock(&rwlock);
        sleep(3);
    }
    return nullptr;
}

//обработка покупателя
void* funRead(void* param){
    //номер отдела
    int* num = (int*)param;
    //номер покупателя
    int num_buyer;
    while(1) {
        pthread_mutex_lock(&rwlock);
        bool fl = false;
        if (*(num) == 1 && !a1.empty()) {
            num_buyer = a1.front();
            a1.pop();
            fl = true;
        } else if (*(num) == 2 && !a2.empty()) {
            num_buyer = a2.front();
            a2.pop();
            fl = true;
        } else if (*(num) == 3 && !a3.empty()){
            num_buyer = a3.front();
            a3.pop();
            fl = true;
        }
        pthread_mutex_unlock(&rwlock);
        if(fl) {
            fprintf(stdout, "Buyer number %d - was served in %d section\n", num_buyer, *num);
            sleep(1);
        }
    }
    return nullptr;
}

int main(){
    pthread_mutex_init(&rwlock, NULL) ;

    //создание продавцов
    pthread_t threads[3];
    int nums[3];
    for (int i = 0; i < 3; i++){
        nums[i] = i + 1;
        pthread_create(&threads[i], NULL, funRead, (void*)(nums+i)) ;
    }

    //создание покупателей
    pthread_t threadW[10];
    int delays[10];
    for(int i = 0; i < 10; i++){
        delays[i] = i+1;
        pthread_create(&threadW[i], NULL, funWrite, (void*)(delays+i));
    }

    while (1){}
    return 0;
}