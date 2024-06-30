/**
 * W zadaniu mamy cztery wątki: producenta i konsumenta.
 * Każdy producent może wyprodukować zadaną z góry liczbę wartości liczby typu int, tj. 3
 * Każdy wątek producenta produkuje wartości i przechowuje je we współdzielonym buforze o długości 2, 
 * podczas gdy wątek konsumenta pobiera wartości z bufora.
 * 
 * Używamy muteksu i dwóch semaforów (full, empty) do synchronizacji dostępu do buforów. 
 * Muteks służy do zarządzania dostępem do sekcji krytycznej. 
 * Semafory umożliwiają wątkom sygnalizować sobie nawzajem, czy można wykonać operacje zapisu/odczytu.
 * 
 * Każdy wątek producenta generuje liczbą losowa, a następnie wykonuje operację na semaforze.  
 * Jeśli zapis jest możliwy to zajmuje muteks, wstawia wcześniej wygenerowaną liczbę do bufora o indeksie (na pozycji) in
 * wyświetla komunikat na ekranie 
 * (który producent wykonał operację na buforze, zawartość bufora o indeksie (na pozycji) in, wartość indeksu in)
 * inkrementuje wartość indeksu in (operacja modulo), zwalnia mutex
 * a następnie wykonuje operacje na semaforze.
 * 
 * Każdy wątek konsumenta wykonuje operacje na semaforze.
 * Jeśli odczyt jest możliwy to zajmuje muteks
 * wczytuje zawartość bufora o indeksie (z pozycji) out
 * wyświetla komunikat na ekranie
 * (który spośród trzech konsumentów wykonał operację na buforze, pobrana zawartość bufora, wartość indeksu out)
 * modyfikuje wartość indeksu out
 * Zwalnia muteks, a następnie wykonuje operacje na semaforze.
 * 
 * Funkcja main tworzy wątki producenta i konsumenta
*/



#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>


#define MaxItems 3		// liczba wartosci, ktora producent produkuje, zas konsument konsumuje
#define BufferSize 2		// wielkosc bufora

						
sem_t empty;           
sem_t full;               
int in = 0;          
int out = 0;             
int buffer[BufferSize];   
pthread_mutex_t mutex;		
							

void* producer(void* pno) {   
    int item;
    int producer_num = *((int*)pno);
    for(int i = 0; i < MaxItems; i++) {
        item = rand(); 
        sem_wait(&empty);             
        pthread_mutex_lock(&mutex);   
        buffer[in] = item;            
        printf("Producer %d: Insert Item %d at %d\n", producer_num, buffer[in], in);
        in = (in + 1) % BufferSize;   
        pthread_mutex_unlock(&mutex); 
        sem_post(&full);              
    }
    return NULL;
}

void* consumer(void* cno) {   
    int consumer_num = *((int*)cno);
    for(int i = 0; i < MaxItems; i++) {
        sem_wait(&full);             
        pthread_mutex_lock(&mutex); 
        int item = buffer[out];    
        printf("Consumer %d: Remove Item %d from %d\n", consumer_num, item, out);
        out = (out + 1) % BufferSize; 
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);            
    }
    return NULL;
}

int main()
{   
    pthread_t pro[4], con[4];
    pthread_mutex_init(&mutex, NULL); 
    sem_init(&empty, 0, BufferSize);  
    sem_init(&full, 0, 0);            

    int a[4] = {1,2,3,4};							// etykiety (numery) producenta i konsumenta

    for(int i = 0; i < 4; i++) {
        pthread_create(&pro[i], NULL, (void*)producer, (void*)&a[i]);
    }
    for(int i = 0; i < 4; i++) {
        pthread_create(&con[i], NULL, (void*)consumer, (void*)&a[i]);
    }

    for(int i = 0; i < 4; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < 4; i++) {
        pthread_join(con[i], NULL);
    }


    pthread_mutex_destroy(&mutex); 
    sem_destroy(&empty);           
    sem_destroy(&full); 

    return 0;
}
