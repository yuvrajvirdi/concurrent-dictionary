#include <stdio.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

void write_to_disk(const char *filename, const char *key, const char *value) {
    pthread_mutex_lock(&file_mutex);
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        fprintf(file, "%s %s\n", key, value);
        fclose(file);
    }
    pthread_mutex_unlock(&file_mutex);
}

void read_from_disk(const char *filename, const char *key, char *value, size_t value_size) {
    pthread_mutex_lock(&file_mutex);
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char file_key[256];
        char file_value[256];
        while (fscanf(file, "%255s %255s", file_key, file_value) == 2) {
            if (strcmp(key, file_key) == 0) {
                strncpy(value, file_value, value_size - 1);
                value[value_size - 1] = '\0';
                fclose(file);
                pthread_mutex_unlock(&file_mutex);
                return;
            }
        }
        fclose(file);
    }
    pthread_mutex_unlock(&file_mutex);
    value[0] = '\0'; // key not found
}
