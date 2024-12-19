/**
  ******************************************************************************
  * @file    App/sensor.c
  * @author  SRA Application Team
  * @brief   Sensor init and sensor state machines
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sensor.h"
#include "gatt_db.h"
#include "cmsis_os.h"
#include "bluenrg_gap.h"
#include "bluenrg_gap_aci.h"
#include "hci_le.h"
#include "hci_const.h"
#include "bluenrg_aci_const.h"
#include "bluenrg_gatt_aci.h"
#include "lsm6dsl.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  ADV_INTERVAL_MIN_MS  1
#define  ADV_INTERVAL_MAX_MS  1
//#define ORIGINAL

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t bdaddr[BDADDR_SIZE];
extern uint8_t bnrg_expansion_board;
__IO uint8_t set_connectable = 1;
__IO uint16_t connection_handle = 0;
__IO uint8_t  notification_enabled = FALSE;
__IO uint32_t connected = FALSE;

//extern uint16_t EnvironmentalCharHandle;
extern uint16_t AccGyroMagCharHandle;
extern uint16_t RenameDeviceCharHandle;

extern uint32_t sf;
extern LSM6DSL_Object_t MotionSensor;
extern LSM6DSL_Axes_t acc_axes;

volatile uint8_t request_free_fall_notify = FALSE;

AxesRaw_t x_axes = {0, 0, 0};
AxesRaw_t g_axes = {0, 0, 0};
AxesRaw_t m_axes = {0, 0, 0};
AxesRaw_t q_axes[SEND_N_QUATERNIONS] = {{0, 0, 0}};

// Define structure for accelerometer data
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} AccAxes_t;

// Global variables
AccAxes_t cur_acc, prev_acc;
uint8_t omg_its_rocking;
int update_count = 0;
extern char device_name[18]; // Your custom device name
extern uint8_t acc_sensitivity;

/* Private function prototypes -----------------------------------------------*/
void GAP_DisconnectionComplete_CB(void);
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
 * Function Name  : Set_DeviceConnectable.
 * Description    : Puts the device in connectable mode.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Set_DeviceConnectable(void)
{

  uint8_t ret;

  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,SENSOR_DEMO_NAME};

  uint8_t manuf_data[26] = {
    2,0x0A,0x00, /* 0 dBm */  // Transmission Power
    8,0x09, SENSOR_DEMO_NAME,  // Complete Name
    13,0xFF,0x01, /* SKD version */
    0x80,
    0x00,
    0xF4, /* ACC+Gyro+Mag 0xE0 | 0x04 Temp | 0x10 Pressure */
    0x00, /*  */
    0x00, /*  */
    bdaddr[5], /* BLE MAC start -MSB first- */
    bdaddr[4],
    bdaddr[3],
    bdaddr[2],
    bdaddr[1],
    bdaddr[0]  /* BLE MAC stop */
  };


  manuf_data[18] |= 0x01; /* Sensor Fusion */


  hci_le_set_scan_resp_data(0, NULL);

  PRINTF("Set General Discoverable Mode.\r\n");

  ret = aci_gap_set_discoverable(ADV_DATA_TYPE,
                                (ADV_INTERVAL_MIN_MS*1000)/625,(ADV_INTERVAL_MAX_MS*1000)/625,
                                 STATIC_RANDOM_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), (const char *)local_name, 0, NULL, 0, 0);

  aci_gap_update_adv_data(26, manuf_data);

  if(ret != BLE_STATUS_SUCCESS)
  {
    PRINTF("aci_gap_set_discoverable() failed: 0x%02x\r\n", ret);
  }
  else
    PRINTF("aci_gap_set_discoverable() --> SUCCESS\r\n");
}

/**
 * @brief  Callback processing the ACI events.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
void user_notify(void * pData)
{
  hci_uart_pckt *hci_pckt = pData;
  /* obtain event packet */
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

  if(hci_pckt->type != HCI_EVENT_PKT)
    return;

  switch(event_pckt->evt){

  case EVT_DISCONN_COMPLETE:
    {
      GAP_DisconnectionComplete_CB();
    }
    break;

  case EVT_LE_META_EVENT:
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      switch(evt->subevent){
      case EVT_LE_CONN_COMPLETE:
        {
          evt_le_connection_complete *cc = (void *)evt->data;
          GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
        }
        break;
      }
    }
    break;

  case EVT_VENDOR:
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;
      switch(blue_evt->ecode){
      case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
		{
		  // Handle GATT write requests
		  evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
		  PRINTF("evt->conn_handle: %d\r\n", evt->conn_handle);
		  PRINTF("evt->attr_handle: %d\r\n", evt->attr_handle);
		  PRINTF("evt->data_length: %d\r\n", evt->data_length);
		  PRINTF("evt->offset: %d\r\n", evt->offset);

		  // Print the written data (i.e., the device name or other characteristics)
		  for(int i = 0; i < evt->data_length; ++i){
			  PRINTF("evt->att_data[%d]: %d\r\n", i, evt->att_data[i]);
		  }

		  // You can also handle other characteristics like the Acceleration Notification here
		  // For example, check for the AccGyroMagCharHandle if relevant
//		  PRINTF("AccGyroMagCharHandle is %d\r\n", AccGyroMagCharHandle);
		  switch(evt->attr_handle) {
		  	  case 7: // "Device Name / Device name"
			  {
				  char new_device_name[evt->data_length + 1];  // +1 for null terminator
				  for(int i = 0; i < evt->data_length; ++i) {
					  new_device_name[i] = evt->att_data[i];  // Convert each byte to ASCII character
				  }
				  new_device_name[evt->data_length] = '\0';  // Null terminate the string

				  PRINTF("Device Name is changed to %s\r\n", new_device_name);
				  if (strlen(new_device_name) <= 17) {
					  int i;
					  // Copy the new device name and pad with spaces if needed
					  for (i = 0; i < 17 && new_device_name[i] != '\0'; i++) {
						  device_name[i] = new_device_name[i];
					  }

					  // Pad remaining space with spaces if the new device name is shorter
					  for (; i < 17; i++) {
						  device_name[i] = ' ';
					  }

					  PRINTF("Device Name is now: %s\r\n", device_name);

				  } else {
					  // Print a message if the new device name is too long
					  PRINTF("Device name too long: %s\r\n", new_device_name);
				  }
				  break;
			    }
		  }
		}
		break;

      case EVT_BLUE_ATT_READ_RESP:
        {
    	  PRINTF("EVT_BLUE_ATT_READ_RESP\r\n");
    	  evt_att_read_resp *pr = (void*)blue_evt->data;

		  // Access the read data:
		  uint16_t read_value = *(uint16_t*)pr->attribute_value;
		  PRINTF("Read Value: %d\r\n", read_value);
        }
        break;


      case EVT_BLUE_GATT_READ_PERMIT_REQ:
        {
          evt_gatt_read_permit_req *pr = (void*)blue_evt->data;
          Read_Request_CB(pr->attr_handle);
        }
        break;

      }

    }
    break;
  }
}
/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  None
 * @retval None
 */
void GAP_DisconnectionComplete_CB(void)
{
  connected = FALSE;
  PRINTF("Disconnected\r\n");
  /* Make the device connectable again. */
  set_connectable = TRUE;
  notification_enabled = FALSE;
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t Address of peer device
 * @param  uint16_t Connection handle
 * @retval None
 */
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle)
{
  connected = TRUE;
  connection_handle = handle;

  PRINTF("Connected to device:");
  for(uint32_t i = 5; i > 0; i--){
    PRINTF("%02X-", addr[i]);
  }
  PRINTF("%02X\r\n", addr[0]);
}

void update_advertising_status(char status) {

    uint8_t manuf_data[26] = {
        2, 0x0A, 0x00, /* 0 dBm */
        8, 0x09, SENSOR_DEMO_NAME, /* Complete Name */
        13, 0xFF, 0x01, /* SKD version */
        0x80,
        0x00,
        0xF4, /* ACC+Gyro+Mag 0xE0 | 0x04 Temp | 0x10 Pressure */
        0x00, /* Reserved */
        0x00, /* Reserved */
        bdaddr[5], /* BLE MAC start -MSB first- */
        bdaddr[4],
        bdaddr[3],
        bdaddr[2],
        bdaddr[1],
        bdaddr[0] /* BLE MAC stop */
    };
#ifndef ORIGINAL
    manuf_data[5] = device_name[0];
    manuf_data[6] = device_name[1];
    manuf_data[7] = device_name[2];
    manuf_data[8] = device_name[3];
    manuf_data[9] = device_name[4];
    manuf_data[10] = device_name[5];
    manuf_data[11] = device_name[6];
    manuf_data[15] = device_name[7];
    manuf_data[16] = device_name[8];
    manuf_data[17] = device_name[9];
    manuf_data[18] = device_name[10];
    manuf_data[19] = device_name[11];
    manuf_data[20] = device_name[12];
    manuf_data[21] = device_name[13];
    manuf_data[22] = device_name[14];
    manuf_data[23] = device_name[15];
    manuf_data[24] = device_name[16];
    manuf_data[25] = status;
#endif

    hci_le_set_advertising_data(sizeof(manuf_data),  manuf_data);
	if (status == 'b') osDelay(1000);
}

void check_and_advertise() {
    // Check if the difference exceeds 10 for any axis
	cur_acc.x = acc_axes.x;
	cur_acc.y = acc_axes.y;
	cur_acc.z = acc_axes.z;
	PRINTF("Acceleration change detected:\r\n");
	    PRINTF("Previous acceleration: x = %d, y = %d, z = %d\r\n",
	           (int)prev_acc.x, (int)prev_acc.y, (int)prev_acc.z);
	    PRINTF("Current acceleration: x = %d, y = %d, z = %d\r\n",
	           (int)cur_acc.x, (int)cur_acc.y, (int)cur_acc.z);
	if (update_count > 10) {
		if (abs(cur_acc.x - prev_acc.x) > acc_sensitivity ||
			abs(cur_acc.y - prev_acc.y) > acc_sensitivity ||
			abs(cur_acc.z - prev_acc.z) > acc_sensitivity) {

			omg_its_rocking = 'b';
			PRINTF("ROCKING DETECTED!!!\r\n");
		} else {
			omg_its_rocking = 'a';
		}
	}
	else {

		update_count++;
	}

    // Update previous values
    prev_acc.x = cur_acc.x;
    prev_acc.y = cur_acc.y;
    prev_acc.z = cur_acc.z;

    // Update advertising data with the current status
    update_advertising_status(omg_its_rocking);
}

