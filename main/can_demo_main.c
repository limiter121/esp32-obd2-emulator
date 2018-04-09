#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_vfs_fat.h"

#include "CAN.h"
#include "CAN_config.h"

#include "obd.h"

#include <string.h>
#include "http_server.h"

#include <dirent.h>
#include "fs.h"

#ifndef CONFIG_ESPCAN
#error for this demo you must enable and configure ESPCan in menuconfig
#endif

#ifdef CONFIG_CAN_SPEED_100KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_100KBPS
#endif

#ifdef CONFIG_CAN_SPEED_125KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_125KBPS
#endif

#ifdef CONFIG_CAN_SPEED_250KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_250KBPS
#endif

#ifdef CONFIG_CAN_SPEED_500KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_500KBPS
#endif

#ifdef CONFIG_CAN_SPEED_800KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_800KBPS
#endif

#ifdef CONFIG_CAN_SPEED_1000KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_1000KBPS
#endif

#ifdef CONFIG_CAN_SPEED_USER_KBPS
#define CONFIG_SELECTED_CAN_SPEED CONFIG_CAN_SPEED_USER_KBPS_VAL /* per menuconfig */
#endif

CAN_device_t CAN_cfg = {
	.speed = CONFIG_SELECTED_CAN_SPEED,		 // CAN Node baudrade
	.tx_pin_id = CONFIG_ESP_CAN_TXD_PIN_NUM, // CAN TX pin example menuconfig GPIO_NUM_5
	.rx_pin_id = CONFIG_ESP_CAN_RXD_PIN_NUM, // CAN RX pin example menuconfig GPIO_NUM_35 ( Olimex )
	.rx_queue = NULL,						 // FreeRTOS queue for RX frames
};

// Queue for CAN multi-frame packets
uint8_t can_flow_queue[5][8];

unsigned int vehicle_speed = 0;
float vehicle_rpm = 0;
float vehicle_throttle = 0;
char vehicle_vin[17] = "ESP32OBD2EMULATOR";

static EventGroupHandle_t wifi_event_group;

#define WIFI_SSID "ESP32-OBD2"
#define WIFI_PASS "88888888"


esp_err_t event_handler(void *ctx, system_event_t *event)
{
	return ESP_OK;
}

CAN_frame_t createOBDResponse(unsigned int mode, unsigned int pid)
{
	CAN_frame_t response;

	response.MsgID = 0x7E8;
	response.FIR.B.DLC = 8;
	response.FIR.B.FF = CAN_frame_std;
	response.FIR.B.RTR = CAN_no_RTR;
	response.data.u8[0] = 2; // Number of data bytes ()
	response.data.u8[1] = 0x40 + mode; // Mode (+ 0x40)
	response.data.u8[2] = pid; // PID

	return response;
}

int sendOBDResponse(CAN_frame_t *response)
{
	int success = CAN_write_frame(response);

	printf("Response = %d | (0x%03x) 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
		   success, response->MsgID,
		   response->data.u8[0], response->data.u8[1], response->data.u8[2], response->data.u8[3],
		   response->data.u8[4], response->data.u8[5], response->data.u8[6], response->data.u8[7]);
	
	return success;
}

void respondToOBD1(uint8_t pid)
{
	printf("Responding to Mode 1 PID 0x%02x\n", pid);

	CAN_frame_t response = createOBDResponse(1, pid);

	switch (pid)
	{
		case 0x00: // Supported PIDs
			response.data.u8[0] += 4; // Number of data bytes
			response.data.u8[3] = 0x00; // Data byte 1
			response.data.u8[4] = 0x18; // Data byte 2
			response.data.u8[5] = 0x80; // Data byte 3
			response.data.u8[6] = 0x00; // Data byte 4
			break;
		case 0x0C: // RPM
			response.data.u8[0] += 2; // Number of data bytes
			obdRevConvert_0C(vehicle_rpm, &response.data.u8[3], &response.data.u8[4], 0, 0);
			break;
		case 0x0D: // Speed
			response.data.u8[0] += 1; // Number of data bytes
			obdRevConvert_0D(vehicle_speed, &response.data.u8[3], 0, 0, 0);
			break;
		case 0x11: // Throttle position
			response.data.u8[0] += 1; // Number of data bytes
			obdRevConvert_11(vehicle_throttle, &response.data.u8[3], 0, 0, 0);
			break;
	}

	sendOBDResponse(&response);
}

void respondToOBD9(uint8_t pid)
{
	printf("Responding to Mode 9 PID 0x%02x\n", pid);

	CAN_frame_t response = createOBDResponse(9, pid);

	switch (pid)
	{
		case 0x00: // Supported PIDs
			response.data.u8[0] += 4; // Number of data bytes
			response.data.u8[3] = 0x40; // Data byte 1
			response.data.u8[4] = 0x00; // Data byte 2
			response.data.u8[5] = 0x00; // Data byte 3
			response.data.u8[6] = 0x00; // Data byte 4
			sendOBDResponse(&response);
			break;
		case 0x02: // Vehicle Identification Number (VIN)
			// Initiate multi-frame message packet
			response.data.u8[0] = 0x10; // FF (First Frame, ISO_15765-2)
			response.data.u8[1] = 0x14; // Length (20 bytes)
			response.data.u8[2] = 0x49; // Mode (+ 0x40)
			response.data.u8[3] = 0x02; // PID
			response.data.u8[4] = 0x01; // Data byte 1
			response.data.u8[5] = vehicle_vin[0]; // Data byte 2
			response.data.u8[6] = vehicle_vin[1]; // Data byte 3
			response.data.u8[7] = vehicle_vin[2]; // Data byte 4
			sendOBDResponse(&response);

			// Clear flow control queue
			// memset(can_flow_queue, 0, 40);

			// Fill flow control queue
			// Part 1
			can_flow_queue[0][0] = 0x21; // CF (Consecutive Frame, ISO_15765-2), sequence number
			can_flow_queue[0][1] = vehicle_vin[3]; // Data byte 1
			can_flow_queue[0][2] = vehicle_vin[4]; // Data byte 2
			can_flow_queue[0][3] = vehicle_vin[5]; // Data byte 3
			can_flow_queue[0][4] = vehicle_vin[6]; // Data byte 4
			can_flow_queue[0][5] = vehicle_vin[7]; // Data byte 5
			can_flow_queue[0][6] = vehicle_vin[8]; // Data byte 6
			can_flow_queue[0][7] = vehicle_vin[9]; // Data byte 7
			// Part 2
			can_flow_queue[1][0] = 0x22; // CF
			can_flow_queue[1][1] = vehicle_vin[10]; // Data byte 1
			can_flow_queue[1][2] = vehicle_vin[11]; // Data byte 2
			can_flow_queue[1][3] = vehicle_vin[12]; // Data byte 3
			can_flow_queue[1][4] = vehicle_vin[13]; // Data byte 4
			can_flow_queue[1][5] = vehicle_vin[14]; // Data byte 5
			can_flow_queue[1][6] = vehicle_vin[15]; // Data byte 6
			can_flow_queue[1][7] = vehicle_vin[16]; // Data byte 7

			break;
	}
}

void task_CAN(void *pvParameters)
{
	(void)pvParameters;

	//frame buffer
	CAN_frame_t __RX_frame;

	//create CAN RX Queue
	CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));

	//start CAN Module
	CAN_init();
	printf("CAN initialized...\n");

	while (1)
	{
		//receive next CAN frame from queue
		if (xQueueReceive(CAN_cfg.rx_queue, &__RX_frame, 3 * portTICK_PERIOD_MS) == pdTRUE)
		{
			printf("\nFrame from : 0x%08x, DLC %d, RTR %d, FF %d \n", __RX_frame.MsgID, __RX_frame.FIR.B.DLC,
				   __RX_frame.FIR.B.RTR, __RX_frame.FIR.B.FF);
			printf("D0: 0x%02x, ", __RX_frame.data.u8[0]);
			printf("D1: 0x%02x, ", __RX_frame.data.u8[1]);
			printf("D2: 0x%02x, ", __RX_frame.data.u8[2]);
			printf("D3: 0x%02x, ", __RX_frame.data.u8[3]);
			printf("D4: 0x%02x, ", __RX_frame.data.u8[4]);
			printf("D5: 0x%02x, ", __RX_frame.data.u8[5]);
			printf("D6: 0x%02x, ", __RX_frame.data.u8[6]);
			printf("D7: 0x%02x\n", __RX_frame.data.u8[7]);
			printf("==============================================================================\n");

			// Check if frame is OBD query
			if (__RX_frame.MsgID == 0x7df) {
				printf("OBD QUERY !\n");

				switch (__RX_frame.data.u8[1]) { // Mode
					case 1: // Show current data
						respondToOBD1(__RX_frame.data.u8[2]);
						break;
					case 9: // Vehicle information
						respondToOBD9(__RX_frame.data.u8[2]);
						break;
					default:
						printf("Unsupported mode %d !\n", __RX_frame.data.u8[1]);
				}
			} else if (__RX_frame.MsgID == 0x7e0) { // Check if frame is addressed to the ECU (us)
				printf("ECU MSG !\n");

				if (__RX_frame.data.u8[0] == 0x30) { // Flow control frame (continue)
					CAN_frame_t response = createOBDResponse(0, 0);

					for (int i = 0; i < 5; i++) {
						if (can_flow_queue[i][0] == 0) { continue; }

						for (int j = 0; j < 8; j++) {
							response.data.u8[j] = can_flow_queue[i][j];
						}
						sendOBDResponse(&response);
					}

					// Clear flow control queue
					memset(can_flow_queue, 0, 40);
				}
			}
		}
	}
}

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

char * get_type_for_filename_ext(char *filename)
{
	char *ext = get_filename_ext(filename);
	if (strcmp(ext, "html") == 0) {
		return "text/html";
	} else if (strcmp(ext, "css") == 0) {
		return "text/css";
	} else if (strcmp(ext, "js") == 0) {
		return "text/javascript";
	}

	return NULL;
}

static void cb_GET_file(http_context_t http_ctx, void *ctx)
{
	char *file = malloc(FILE_MAX_SIZE + 1);
	size_t response_size;
	ESP_ERROR_CHECK(readFile((char*)ctx, file, &response_size));

	char *content_type = get_type_for_filename_ext(ctx);

	http_response_begin(http_ctx, 200, content_type, response_size);
	http_buffer_t http_file = {.data = file};
	http_response_write(http_ctx, &http_file);
	free(file);
	http_response_end(http_ctx);
}

static void cb_PATCH_vehicle(http_context_t http_ctx, void* ctx)
{
	const char *name = http_request_get_arg_value(http_ctx, "name");
	const char *value = http_request_get_arg_value(http_ctx, "value");
	unsigned int code = 200;

	if (name != NULL && value != NULL) {
		printf("Received %s = %s\n", name, value);

		if (strcmp(name, "speed") == 0) {
			vehicle_speed = strtol(value, NULL, 10);
		} else if (strcmp(name, "rpm") == 0) {
			vehicle_rpm = strtol(value, NULL, 10);
		} else if (strcmp(name, "throttle") == 0) {
			vehicle_throttle = strtof(value, NULL);
		} else if (strcmp(name, "vin") == 0) {
			strncpy(vehicle_vin, value, 17);
		}
	} else {
		printf("Invalid data received !\n");
		code = 400;
	}
	
	http_response_begin(http_ctx, code, "text/plain", HTTP_RESPONSE_SIZE_UNKNOWN);
	http_buffer_t http_response = { .data = "", .data_is_persistent = true };
	http_response_write(http_ctx, &http_response);
	http_response_end(http_ctx);
}

void wifi_init_softap()
{
	wifi_event_group = xEventGroupCreate();

	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	wifi_config_t wifi_config = {
		.ap = {
			.ssid = WIFI_SSID,
			.ssid_len = strlen(WIFI_SSID),
			.password = WIFI_PASS,
			.max_connection = 2,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK},
	};
	if (strlen(WIFI_PASS) == 0)
	{
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	printf("wifi_init_softap finished.SSID:%s password:%s\n", WIFI_SSID, WIFI_PASS);
}

void app_main()
{
	// wait for IDF logs to end
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	printf("CAN RXD PIN NUM: %d\n", CONFIG_ESP_CAN_RXD_PIN_NUM);
	printf("CAN TXD PIN NUM: %d\n", CONFIG_ESP_CAN_TXD_PIN_NUM);
	printf("CAN SPEED      : %d KBit/s\n", CONFIG_SELECTED_CAN_SPEED);
#ifdef CONFIG_CAN_SPEED_USER_KBPS
	printf("kBit/s setting was done by User\n");
#endif

	//Create CAN receive task
	xTaskCreate(&task_CAN, "CAN", 2048, NULL, 5, NULL);

	///////////////// WIFI	

	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	printf("Initializing WIFI...\n");
	wifi_init_softap();

	///////////////// HTTP

	printf("Initializing HTTP server...\n");

	http_server_t server;
	http_server_options_t http_options = HTTP_SERVER_OPTIONS_DEFAULT();
	esp_err_t res;

	ESP_ERROR_CHECK(http_server_start(&http_options, &server));
	ESP_ERROR_CHECK(http_register_handler(server, "/", HTTP_GET, HTTP_HANDLE_RESPONSE, &cb_GET_file, "/spiflash/index.html"));
	ESP_ERROR_CHECK(http_register_handler(server, "/main.css", HTTP_GET, HTTP_HANDLE_RESPONSE, &cb_GET_file, "/spiflash/main.css"));
	ESP_ERROR_CHECK(http_register_handler(server, "/main.js", HTTP_GET, HTTP_HANDLE_RESPONSE, &cb_GET_file, "/spiflash/main.js"));
	ESP_ERROR_CHECK(http_register_form_handler(server, "/api/vehicle", HTTP_PATCH, HTTP_HANDLE_RESPONSE, &cb_PATCH_vehicle, NULL));

	////////////////// FAT

	esp_vfs_fat_mount_config_t mountConfig;
	wl_handle_t m_wl_handle;
	mountConfig.max_files = 4;
	mountConfig.format_if_mount_failed = false;
	ESP_ERROR_CHECK(esp_vfs_fat_spiflash_mount("/spiflash", "storage", &mountConfig, &m_wl_handle));

	ESP_ERROR_CHECK(dumpDir("/spiflash"));
}
