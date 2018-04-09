#include "esp_system.h"

#define FILE_MAX_SIZE 4096

esp_err_t dumpDir(char *path);
esp_err_t readFile(char *path, char *buffer, size_t *length);