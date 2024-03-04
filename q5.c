#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#define MASSAGE_LENGTH 80
#define SECRET_KEY_LENGTH 4
#define AMOUNT_OF_BITS 32
#define NUM_OF_THREADS 14
#define RC4_ARR_STATE_LENGTH 256

typedef struct
{
    int ID_thread;

    char cipher[MASSAGE_LENGTH + 1];
    
    uint64_t start_index_key;
    uint64_t num_keys;
    
} ThreadStract;

void swap(unsigned char *a, unsigned char *b) {
    unsigned char temp = *a;
    *a = *b;
    *b = temp;
}
void fill_arr_state(unsigned char *arr_state){

    int i =0;
    while(i < RC4_ARR_STATE_LENGTH){
        arr_state[i] = i;
        i++;
    }
}

void creat_state_init(unsigned char *secret_key, int key_length, unsigned char *arr_state)
{
    fill_arr_state(arr_state);
    for (int i = 0, j=0; i < RC4_ARR_STATE_LENGTH; i++)
    {
        j = (j + arr_state[i] + secret_key[i % key_length]) % RC4_ARR_STATE_LENGTH;
         swap(&arr_state[i], &arr_state[j]);
    }
}

void encryption_and_decryption(unsigned char *massage, unsigned char *chiper, int massege_len, unsigned char *arr_state)
{
    for (int a = 0, i =0, j =0; a < massege_len; a++)
    {
        i = (i + 1) % RC4_ARR_STATE_LENGTH;
        j = (j + arr_state[i]) % RC4_ARR_STATE_LENGTH;
        swap(&arr_state[i], &arr_state[j]);
         int t = (arr_state[i] + arr_state[j]) % RC4_ARR_STATE_LENGTH;
        chiper[a] = massage[a] ^ arr_state[t]; // xor
    }
}

void *run_in_thread(void *thread_data_ptr)
{
    ThreadStract *thread_data = (ThreadStract *)thread_data_ptr;

    unsigned char decrypted_text[MASSAGE_LENGTH + 1];
    memset(decrypted_text, 0, MASSAGE_LENGTH + 1);

    unsigned char arr_state[RC4_ARR_STATE_LENGTH];
    memset(arr_state, 0, RC4_ARR_STATE_LENGTH);

    unsigned char current_key[SECRET_KEY_LENGTH];
    memset(current_key, 0, SECRET_KEY_LENGTH);

    unsigned char cipher[MASSAGE_LENGTH + 1];
    memset(cipher, 0, MASSAGE_LENGTH + 1);

    uint64_t start_index_key = thread_data->start_index_key;
    uint64_t num_key = thread_data->num_keys;
    int ID_thread = thread_data->ID_thread;

    uint64_t keys_checked = 0;
    uint64_t keyspace_per_thread = (1ULL << AMOUNT_OF_BITS) / NUM_OF_THREADS;

    memcpy(cipher, thread_data->cipher, MASSAGE_LENGTH + 1);

     printf("Thread number %d - started running\n", thread_data->ID_thread);

    for (uint64_t key = start_index_key; key < key+num_key; key++)
    {
        

        for (int i = 0; i < SECRET_KEY_LENGTH; i++)
            current_key[i] = (key >> (8 * i)) & 0xFF;

        creat_state_init(current_key, SECRET_KEY_LENGTH, arr_state);

        encryption_and_decryption(cipher, decrypted_text, MASSAGE_LENGTH, arr_state);

        int isOK = 1;

        for (int i = 0; i < MASSAGE_LENGTH; i++)
        {
            if (decrypted_text[i] < 32 || decrypted_text[i] > 126)
            {
                isOK = 0;
                break;
            }
        }

        if (isOK)
        {
  
        printf("The secret Key found: ");

           int i = 0;
            while (i < SECRET_KEY_LENGTH) {
                printf("%02X", current_key[i]);
                i++;
            }

         printf("\nThe Massage is: %s\n", decrypted_text);

        exit(0);
        }    
    }

    printf("Thread %d - Finished\n", ID_thread);

    return NULL;
}

void init_thread(unsigned char *cipher){
    
    pthread_t threads[NUM_OF_THREADS];
    ThreadStract thread_data[NUM_OF_THREADS];

    uint64_t key_per_thread = (1ULL << AMOUNT_OF_BITS) / NUM_OF_THREADS;
    uint64_t start_index_key = 0;

    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        thread_data[i].start_index_key = i*key_per_thread;
        thread_data[i].num_keys = key_per_thread;
        thread_data[i].ID_thread = (i + 1);
        memcpy(thread_data[i].cipher, cipher, MASSAGE_LENGTH + 1);
        
        pthread_create(&threads[i], NULL, run_in_thread, (void *)&thread_data[i]);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++)
        pthread_join(threads[i], NULL);

}

void gerate_key(unsigned char *key){
    
    int i =0;

    while (i < SECRET_KEY_LENGTH)
    {
       key[i] = rand() % 256;
       i++;
    }

}

void print_key(unsigned char *key){
    
    int i = 0;
        while (i < SECRET_KEY_LENGTH) {
            printf("%02X", key[i]);
            i++;
        }
}

void print_massage(unsigned char *cipher){
    

    for (int i = 0; i < MASSAGE_LENGTH; i++)
        printf("%02X", cipher[i]);

}

int main(void) {

    unsigned char arr_state[RC4_ARR_STATE_LENGTH];
    memset(arr_state, 0, RC4_ARR_STATE_LENGTH);

    unsigned char cipher[MASSAGE_LENGTH + 1];
    memset(cipher, 0, MASSAGE_LENGTH+1);

    unsigned char plaintext[MASSAGE_LENGTH + 1] = "that the whole world knew that Israel was going to win the war and wipe out Gaza";

    srand(time(NULL));

    unsigned char key[SECRET_KEY_LENGTH];
    memset(key, 0, SECRET_KEY_LENGTH);
    
    gerate_key(key);

    printf("The secret Key: \n");

    print_key(key);

    printf("\n");

    creat_state_init(key, SECRET_KEY_LENGTH, arr_state);

    encryption_and_decryption(plaintext, cipher, MASSAGE_LENGTH, arr_state);

    printf("Cipher: ");
    
    print_massage(cipher);

    printf("\n Starting attack!! \n");

    init_thread(cipher);
    return 0;
}