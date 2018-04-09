#include "fs.h"
#include <dirent.h>
#include <string.h>

esp_err_t dumpDir(char *path)
{
	DIR *pDir = opendir(path);
	if (!pDir) {
		return ESP_FAIL;
	}

	struct dirent *pDirent;
	printf("\nCONTENTS OF %s\n", path);

	while((pDirent = readdir(pDir)) != NULL) {
		char type[10];
		switch(pDirent->d_type) {
			case DT_UNKNOWN:
				strcpy(type, "Unknown");
				break;
			case DT_REG:
				strcpy(type, "Regular");
				break;
			case DT_DIR:
				strcpy(type, "Directory");
				break;
		}
		printf("| %s (%d, %s)\n", pDirent->d_name, pDirent->d_ino, type);
	}
	
	printf("\n");
	closedir(pDir);

	return ESP_OK;
}

esp_err_t readFile(char *path, char *buffer, size_t *length)
{
	printf("Opening file %s\n", path);
	FILE *fp = fopen(path, "r");
	if (fp != NULL) {
		printf("Reading file\n");
		size_t newLen = fread(buffer, sizeof(char), FILE_MAX_SIZE, fp);
		printf("read %d\n", newLen);
		if (length) {
			*length = newLen;
		}
		if (ferror(fp) != 0) {
			return ESP_FAIL;
		} else {
			buffer[newLen++] = '\0'; /* Just to be safe. */

			
		}

		fclose(fp);

		return ESP_OK;
	}

	return ESP_FAIL;
}