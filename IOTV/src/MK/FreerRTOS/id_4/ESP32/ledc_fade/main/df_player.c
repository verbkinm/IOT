#include "df_player.h"

extern QueueHandle_t xQueueDF;
extern struct IOTV_Server_embedded iot;

static const char *TAG = "DF_PLAYER";

static uint8_t sendDataBuffer[DF_BUFFER_SIZE], recDataBuffer[DF_BUFFER_SIZE], recDataBufferSize = 0;

static bool *isPlay, *repeate;
static int8_t *curr_mode, *curr_vol, *curr_eq ;

static int8_t *Led_RGB_scriptNumber;

static uint8_t  num_USB_tracks,
				num_SD_tracks,
				curr_USB_track,
				curr_SD_track,
				num_folders;

// количесвто треков для каждого режима
static uint8_t mode_tracks[DF_MODE_SIZE];
// текущий трек на режиме n. Отсчет с 1
static uint8_t mode_curr_trak[DF_MODE_SIZE] = {1, 1, 1};

static void initUART(void);

void DF_Task(void *pvParameters)
{
	while(iot.state == 0)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	isPlay = (bool *)iot.readChannel[CH_PLAY].data;
	curr_mode = (int8_t *)iot.readChannel[CH_PLAY_MODE].data;
	*curr_mode = 1;
	repeate = (bool *)iot.readChannel[CH_REPEATE].data;
	curr_vol = (int8_t *)iot.readChannel[CH_VOLUME].data;
	curr_eq = (int8_t *)iot.readChannel[CH_EQ].data;
	Led_RGB_scriptNumber = (int8_t *)iot.readChannel[CH_LED_MODE].data;

	gpio_reset_pin(DF_BUSY);
	gpio_set_direction(DF_BUSY, GPIO_MODE_INPUT);
	gpio_set_pull_mode(DF_BUSY, GPIO_PULLDOWN_ONLY);

	initUART();

	ESP_LOGW(TAG, "DF_Task created");
	xTaskCreate(DF_dataRecive_Task, "DF_dataRecive_Task", 2048, NULL, UART_PRIORITY, NULL);

	DF_updateData();

	int8_t cmd;
	while (1)
	{
		if (xQueueReceive(xQueueDF, (void *)&cmd, 10 / portTICK_PERIOD_MS) == pdPASS)
		{
			switch (cmd)
			{
			case DF_CMD_PS:
				*isPlay = inRange(*isPlay, 0, 1);
				if (*isPlay)
				{
					DF_playLargeFolder(*curr_mode, mode_curr_trak[*curr_mode - 1]);
					mode_curr_trak[*curr_mode - 1] = inc(mode_curr_trak[*curr_mode - 1], 1, mode_tracks[*curr_mode - 1]);
					*Led_RGB_scriptNumber = inc(*Led_RGB_scriptNumber, LED_MODE_MIN, LED_MODE_MAX);
				}
				else
				{
					*repeate = false;
					DF_stop();
					*Led_RGB_scriptNumber = 0;
				}
				break;
			case DF_CMD_MODE:
				*curr_mode = inRange(*curr_mode, DF_MODE_1, DF_MODE_3);
				DF_playLargeFolder(*curr_mode, mode_curr_trak[*curr_mode - 1]);
				mode_curr_trak[*curr_mode - 1] = inc(mode_curr_trak[*curr_mode - 1], 1, mode_tracks[*curr_mode - 1]);
				*Led_RGB_scriptNumber = inc(*Led_RGB_scriptNumber, LED_MODE_MIN, LED_MODE_MAX);
				break;
			case DF_CMD_REPEATE:
				*repeate = inRange(*repeate, DF_MODE_1, DF_MODE_3);
				if (*repeate == 0)
					DF_stopRepeat();
				break;
			case DF_CMD_VOLUME:
				*curr_vol = inRange(*curr_vol, VOLUME_MIN, VOLUME_MAX);
				DF_volume(*curr_vol);
				break;
			case DF_CMD_EQ:
				*curr_eq = inRange(*curr_eq, EQ_NORMAL, EQ_BASE);
				DF_EQSelect(*curr_eq);
				break;
			default:
				break;
			}
		}

		*isPlay = !gpio_get_level(DF_BUSY);

		// Проверка включения повтора текущего режима
		if (!*isPlay && *repeate)
		{
			static int counter = 0;
			if (++counter > 10)
			{
				DF_playLargeFolder(*curr_mode, mode_curr_trak[*curr_mode - 1]);
				mode_curr_trak[*curr_mode - 1] = inc(mode_curr_trak[*curr_mode - 1], 1, mode_tracks[*curr_mode - 1]);
				*Led_RGB_scriptNumber = inc(*Led_RGB_scriptNumber, LED_MODE_MIN, LED_MODE_MAX);
				counter = 0;
			}
		}
	}
}

void DF_dataRecive_Task(void *pvParameters)
{
	uint8_t byte;
	while (1)
	{
		const int rxBytes = uart_read_bytes(UART_NUM_1, &byte, 1, 1000 / portTICK_PERIOD_MS);
		if (rxBytes > 0)
		{
			recDataBuffer[recDataBufferSize] = byte;
			recDataBufferSize++;
			if (recDataBufferSize == DF_BUFFER_SIZE)
				DF_parseData();
		}
	}
}
/**************************************************************************/
/*!
     @brief  Play the next song in chronological order.
 */
/**************************************************************************/
void DF_playNext()
{
	sendDataBuffer[CMD_VALUE] = CTRL_NEXT;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Play the previous song in chronological order.
 */
/**************************************************************************/
void DF_playPrevious()
{
	sendDataBuffer[CMD_VALUE] = CTRL_PREV;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Play a specific track.
     @param    trackNum
               The track number to play.
 */
/**************************************************************************/
void DF_play(uint16_t trackNum)
{
	sendDataBuffer[CMD_VALUE] = CTRL_PLAY;
	sendDataBuffer[PARAM_MSB] = (trackNum >> 8);
	sendDataBuffer[PARAM_LSB] = trackNum;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Stop the current playback
 */
/**************************************************************************/
void DF_stop()
{
	sendDataBuffer[CMD_VALUE] = CTRL_STOP;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 0;
	DF_sendData();
}

void DF_playFromRootFolder(uint16_t trackNum)
{
	sendDataBuffer[CMD_VALUE] = CTRL_SPEC_PLAY;
	sendDataBuffer[PARAM_MSB] = (trackNum >> 8);
	sendDataBuffer[PARAM_LSB] = trackNum;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Play a specific track in the folder named "MP3".
     @param    trackNum
               The track number to play.
 */
/**************************************************************************/
void DF_playFromMP3Folder(uint16_t trackNum)
{
	sendDataBuffer[CMD_VALUE] = CTRL_SPEC_PLAY_MP3;
	sendDataBuffer[PARAM_MSB] = (trackNum >> 8);
	sendDataBuffer[PARAM_LSB] = trackNum;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Interrupt the current track with a new track.
     @param    trackNum
               The track number to play.
 */
/**************************************************************************/
void DF_playAdvertisement(uint16_t trackNum)
{
	sendDataBuffer[CMD_VALUE] = CTRL_INSERT_ADVERT;
	sendDataBuffer[PARAM_MSB] = (trackNum >> 8);
	sendDataBuffer[PARAM_LSB] = trackNum;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Stop the interrupting track.
 */
/**************************************************************************/
void DF_stopAdvertisement()
{
	sendDataBuffer[CMD_VALUE] = CTRL_STOP_ADVERT;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 0;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Increment the volume by 1 out of 30.
 */
/**************************************************************************/
void DF_incVolume()
{
	sendDataBuffer[CMD_VALUE] = CTRL_INC_VOL;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Decrement the volume by 1 out of 30.
 */
/**************************************************************************/
void DF_decVolume()
{
	sendDataBuffer[CMD_VALUE] = CTRL_DEC_VOL;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Set the volume to a specific value out of 30.
     @param    volume
               The volume level (0 - 30).
 */
/**************************************************************************/
void DF_volume(uint8_t value)
{
	if (value > 30)
		return;

	sendDataBuffer[CMD_VALUE] = CTRL_VOLUME;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = value;
	DF_sendData();
	DF_query_currentVolume();
}

/**************************************************************************/
/*!
     @brief  Set the EQ mode.
     @param    setting
               The desired EQ ID.
 */
/**************************************************************************/
void DF_EQSelect(uint8_t setting)
{
	if (setting > 5)
		return;

	sendDataBuffer[CMD_VALUE] = CTRL_EQ;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = setting;
	DF_sendData();
	DF_query_currentEQ();
}

/**************************************************************************/
/*!
     @brief  Loop a specific track.
     @param    trackNum
               The track number to play.
 */
/**************************************************************************/
void DF_loop(uint16_t trackNum)
{
	sendDataBuffer[CMD_VALUE] = CTRL_PLAYBACK_MODE;
	sendDataBuffer[PARAM_MSB] = (trackNum >> 8);
	sendDataBuffer[PARAM_LSB] = trackNum;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Specify the playback source.
     @param    source
               The playback source ID.
               1 - USB
               2 - SD
 */
/**************************************************************************/
void DF_playbackSource(uint8_t source)
{
	if (source < 1 || source > 2)
		return;

	sendDataBuffer[CMD_VALUE] = CTRL_PLAYBACK_SRC;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = source;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Reset all settings to factory default.
 */
/**************************************************************************/
void DF_reset()
{
	sendDataBuffer[CMD_VALUE] = CTRL_RESET;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Pause playing current track.
 */
/**************************************************************************/
void DF_pause()
{
	sendDataBuffer[CMD_VALUE] = CTRL_PAUSE;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Play a specific track from a specific folder.
     @param    folderNum
               The folder number.
     @param    trackNum
               The track number to play.
 */
/**************************************************************************/
void DF_playFolder(uint8_t folderNum, uint8_t trackNum)
{
	sendDataBuffer[CMD_VALUE] = CTRL_SPEC_FOLDER;
	sendDataBuffer[PARAM_MSB] = folderNum;
	sendDataBuffer[PARAM_LSB] = trackNum;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Play a specific track from a specific folder, where the track
             names are numbered 4 digit (e.g. 1234-mysong.mp3) and can be
             up to 3000. Only 15 folders ("01" to "15") are supported in this
             mode.
     @param    folderNum
               The folder number.
     @param    trackNum
               The track number to play.
 */
/**************************************************************************/
void DF_playLargeFolder(uint8_t folderNum, uint16_t trackNum)
{
	const uint16_t arg = (((uint16_t)folderNum) << 12) | (trackNum & 0x0FFFF);
	sendDataBuffer[CMD_VALUE] = CTRL_SPEC_TRACK_3000;
	sendDataBuffer[PARAM_MSB] = arg >> 8;
	sendDataBuffer[PARAM_LSB] = arg;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Specify volume gain.
     @param    gain
               The specified volume gain.
               on - 1
               off - 2
 */
/**************************************************************************/
void DF_volumeAdjustSet(uint8_t on_off, uint8_t gain)
{
	if (gain > 31 || on_off > 1)
		return;

	sendDataBuffer[CMD_VALUE] = CTRL_AUDIO_AMPL;
	sendDataBuffer[PARAM_MSB] = on_off;
	sendDataBuffer[PARAM_LSB] = gain;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Play all tracks.
 */
/**************************************************************************/
void DF_startRepeatPlay()
{
	sendDataBuffer[CMD_VALUE] = CTRL_REPEAT_PLAY;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Stop repeat play.
 */
/**************************************************************************/
void DF_stopRepeatPlay()
{
	sendDataBuffer[CMD_VALUE] = CTRL_REPEAT_PLAY;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 0;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Play all tracks in a given folder.
     @param    folderNum
               The folder number.
 */
/**************************************************************************/
void DF_repeatFolder(uint16_t folder)
{
	if (folder > 99)
		return;

	sendDataBuffer[CMD_VALUE] = CTRL_REPEAT_FOLDER;
	sendDataBuffer[PARAM_MSB] = (folder >> 8);
	sendDataBuffer[PARAM_LSB] = folder;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Play all tracks in a random order.
 */
/**************************************************************************/
void DF_randomAll()
{
	sendDataBuffer[CMD_VALUE] = CTRL_RANDOM_ALL;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 0;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Repeat the current track.
 */
/**************************************************************************/
void DF_startRepeat()
{
	sendDataBuffer[CMD_VALUE] = CTRL_REPEAT_CURRENT;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 0;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Stop repeat play of the current track.
 */
/**************************************************************************/
void DF_stopRepeat()
{
	sendDataBuffer[CMD_VALUE] = CTRL_REPEAT_CURRENT;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Turn on DAC.
 */
/**************************************************************************/
void DF_startDAC()
{
	sendDataBuffer[CMD_VALUE] = CTRL_SET_DAC;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 0;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Turn off DAC.
 */
/**************************************************************************/
void DF_stopDAC()
{
	sendDataBuffer[CMD_VALUE] = CTRL_SET_DAC;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 1;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Put the MP3 player into sleep mode.
 */
/**************************************************************************/
void DF_sleep()
{
	sendDataBuffer[CMD_VALUE] = CTRL_SLEEP;
	sendDataBuffer[PARAM_MSB] = 0;
	sendDataBuffer[PARAM_LSB] = 0;
	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Determine if a track is currently playing.
 */
/**************************************************************************/
void DF_query_isPlaying()
{
	DF_query(CMD_STATUS, 0, 0);
}

/**************************************************************************/
/*!
     @brief  Determine the current volume setting.
 */

/**************************************************************************/
void DF_query_currentVolume()
{
	DF_query(CMD_VOLUME, 0, 0);
}

/**************************************************************************/
/*!
     @brief  Determine the current EQ setting.
 */
/**************************************************************************/
void DF_query_currentEQ()
{
	DF_query(CMD_EQ, 0, 0);
}

/**************************************************************************/
/*!
     @brief  Determine the number of tracks accessible via USB.
 */
/**************************************************************************/
void DF_query_numUsbTracks()
{
	DF_query(CMD_USB_FILES, 0, 0);
}

/**************************************************************************/
/*!
     @brief  Determine the number of tracks accessible via SD card.
 */
/**************************************************************************/
void DF_query_numSdTracks()
{
	DF_query(CMD_SD_FILES, 0, 0);
}

/**************************************************************************/
/*!
     @brief  Determine the current track played via USB.
 */
/**************************************************************************/
void DF_query_currentUsbTrack()
{
	DF_query(CMD_USB_TRACK, 0, 0);
}

/**************************************************************************/
/*!
     @brief  Determine the current track played via SD card.
 */
/**************************************************************************/
void DF_query_currentSdTrack()
{
	return DF_query(CMD_SD_TRACK, 0, 0);
}

/**************************************************************************/
/*!
     @brief  Determine the number of tracks in the specified folder.
     @param    folder
               The folder number.
 */
/**************************************************************************/
void DF_query_numTracksInFolder(uint16_t folder)
{
	DF_query(CMD_FOLDER_FILES, folder >> 8, folder);
}

/**************************************************************************/
/*!
     @brief  Determine the number of folders available.
 */
/**************************************************************************/
void DF_query_numFolders()
{
	DF_query(CMD_FOLDERS, 0, 0);
}

/**************************************************************************/
/*!
     @brief  Determine and insert the checksum of a given config/command
             packet into that same packet struct.
     @param    _stack
               Reference to a struct containing the config/command packet
               to calculate the checksum over.
 */
/**************************************************************************/
void DF_calculateChecksum()
{
	uint16_t checksum = (~(sendDataBuffer[VERSION] + sendDataBuffer[LENGTH] + sendDataBuffer[CMD_VALUE] + sendDataBuffer[FEEDBAC_VALUE] + sendDataBuffer[PARAM_MSB] + sendDataBuffer[PARAM_LSB])) + 1;

	sendDataBuffer[CHECKSUM_MSB] = checksum >> 8;
	sendDataBuffer[CHECKSUM_LSB] = checksum;
}

/**************************************************************************/
/*!
     @brief  Send a config/command packet to the MP3 player.
 */
/**************************************************************************/
void DF_sendData()
{
	vTaskDelay(50 / portTICK_PERIOD_MS);

	sendDataBuffer[START_BYTE] = SB;
	sendDataBuffer[VERSION] = VER;
	sendDataBuffer[LENGTH] = LEN;
	sendDataBuffer[FEEDBAC_VALUE] = NO_FEEDBACK;
	DF_calculateChecksum();
	sendDataBuffer[END_BYTE] = EB;

	uart_write_bytes(UART_NUM_1, sendDataBuffer, DF_BUFFER_SIZE);

	printf("Data send ->   ");
	DF_printBuff(sendDataBuffer, DF_BUFFER_SIZE);
	printf("\n");
	fflush(stdout);
}

/**************************************************************************/
/*!
     @brief  Query the MP3 player for specific information.
     @param    cmd
               The command/query ID.
     @param    msb
               The payload/parameter MSB.
     @param    lsb
               The payload/parameter LSB.
 */
/**************************************************************************/
void DF_query(uint8_t cmd, uint8_t msb, uint8_t lsb)
{
	sendDataBuffer[START_BYTE] = SB;
	sendDataBuffer[VERSION] = VER;
	sendDataBuffer[LENGTH] = LEN;
	sendDataBuffer[CMD_VALUE] = cmd;
	sendDataBuffer[FEEDBAC_VALUE] = NO_FEEDBACK;
	sendDataBuffer[PARAM_MSB] = msb;
	sendDataBuffer[PARAM_LSB] = lsb;
	DF_calculateChecksum();
	sendDataBuffer[END_BYTE] = EB;

	DF_sendData();
}

/**************************************************************************/
/*!
     @brief  Parse MP3 player query responses.
 */
/**************************************************************************/

void DF_parseData()
{
	if (recDataBufferSize < DF_BUFFER_SIZE)
		return;

	if (recDataBuffer[START_BYTE] != SB && recDataBuffer[END_BYTE] != EB)
	{
		printf("Recive unknow data. Data in rec buffer: ");
		DF_printBuff(recDataBuffer, recDataBufferSize);
		printf("\n");

		// Смещаем буфер влево, отбрасывая первый байт
		memmove(recDataBuffer, &recDataBuffer[1], DF_BUFFER_SIZE - 1);
		recDataBufferSize--;
		return;
	}

	printf("Data recive <- ");
	DF_printBuff(recDataBuffer, recDataBufferSize);
	printf(" - ");

#define MSB recDataBuffer[PARAM_MSB]
#define LSB recDataBuffer[PARAM_LSB]

	switch (recDataBuffer[CMD_VALUE])
	{
	case CMD_CUR_DEV_ONLINE:
		if (LSB == 0x01)
			printf("USB flash drive online\n");
		else if (LSB == 0x02)
			printf("SD card online\n");
		else if (LSB == 0x03)
			printf("PC online\n");
		else if (LSB == 0x04)
			printf("USB flash drive and SD card online\n");
		break;

	case CMD_ERROR:
		DF_printError();
		break;

	case CMD_FEEDBACK:
		if (MSB == 0 && LSB == 0)
			printf("Module has successfully received the command\n");
		break;

	case CMD_STATUS:
		if (MSB == 0 || MSB == 1)  // 1 || 2
		{
			printf("A track in %s is %s\n",
					(MSB == 1) ? "USB" : "SD",
							(LSB == 0) ? "stopped" : (LSB == 1) ? "playing" : "paused");
		}
		else if (MSB == 0x10)
			printf("Module in sleep mode\n");
		break;

	case CMD_VOLUME:
		printf("Volume is %d\n", LSB);
		*curr_vol = (int8_t)LSB;
		break;

	case CMD_EQ:
		printf("EQ is %d\n", LSB);
		*curr_eq = LSB;
		break;

	case CMD_USB_FILES:
		num_USB_tracks = ((uint16_t)MSB << 8) | LSB;
		printf("Files in USB: %d\n", num_USB_tracks);
		break;
	case CMD_SD_FILES:
		num_SD_tracks = ((uint16_t)MSB << 8) | LSB;
		printf("Files in SD: %d\n", num_SD_tracks);
		break;

	case CMD_USB_TRACK:
		curr_USB_track = ((uint16_t)MSB << 8) | LSB;
		printf("The track %d in USB being played\n", curr_USB_track);
		break;

	case CMD_SD_TRACK:
		curr_SD_track = ((uint16_t)MSB << 8) | LSB;
		printf("The track %d in SD being played\n", curr_SD_track);
		break;

	case CMD_FOLDER_FILES:
		uint8_t tracks = ((uint16_t)MSB << 8) | LSB;
		printf("%d track in folder\n", tracks);
		if (mode_tracks[0] == 0)
			mode_tracks[0] = tracks;
		else if (mode_tracks[1] == 0)
			mode_tracks[1] = tracks;
		else if (mode_tracks[2] == 0)
			mode_tracks[2] = tracks;
		break;

	case CMD_FOLDERS:
		num_folders = ((uint16_t)MSB << 8) | LSB;
		printf("%d folders in current device\n", num_folders);
		break;

	case CMD_DEV_PLUGGED:
		if (LSB == 1)
			printf("USB flash drive is plugged in\n");
		else if (LSB == 2)
			printf("SD card is plugged in\n");
		else if (LSB == 4)
			printf("USB cable connected to PC is plugged in\n");
		break;
	case CMD_DEV_PULL_OUT:
		if (LSB == 1)
			printf("USB flash drive is pulled out\n");
		else if (LSB == 2)
			printf("SD card is pulled out\n");
		else if (LSB == 4)
			printf("USB cable connected to PC is pulled out\n");
		break;
	case CMD_TRACK_FINSH_USB:
		printf("%d track is finished playing in USB flash drive\n", ((uint16_t)MSB << 8) | LSB);
		break;
	case CMD_TRACK_FINSH_SD:
		printf("%d track is finished playing in SD card\n", ((uint16_t)MSB << 8) | LSB);
		break;
	default:
		printf("In recive unknow data.\n");
	}

	fflush(stdout);

	recDataBufferSize = 0;

#undef MSB
#undef LSB
}

/**************************************************************************/
/*!
     @brief  Print the error description if an error has been received
 */
/**************************************************************************/
void DF_printError()
{
	if (recDataBuffer[CMD_VALUE] == CMD_ERROR)
	{
		switch (recDataBuffer[PARAM_LSB])
		{
		case 0x1:
		{
			printf("Module busy (this info is returned when the initialization is not done)");
			break;
		}
		case 0x2:
		{
			printf("Currently sleep mode(supports only specified device in sleep mode)");
			break;
		}
		case 0x3:
		{
			printf("Serial receiving error(a frame has not been received completely yet)");
			break;
		}
		case 0x4:
		{
			printf("Checksum incorrect");
			break;
		}
		case 0x5:
		{
			printf("Specified track is out of current track scope");
			break;
		}
		case 0x6:
		{
			printf("Specified track is not found");
			break;
		}
		case 0x7:
		{
			printf("Insertion error(an inserting operation only can be done when a track is being played)");
			break;
		}
		case 0x8:
		{
			printf("SD card reading failed(SD card pulled out or damaged)");
			break;
		}
		case 0xA:
		{
			printf("Entered into sleep mode");
			break;
		}
		default:
		{
			printf("Unknown error: %d", recDataBuffer[PARAM_LSB]);
			break;
		}
		}
	}
	else
		printf("No error");

	printf("\n");
	fflush(stdout);
}

void DF_printBuff(uint8_t *data, uint8_t size)
{
	for (int i = 0; i < size - 1; ++i)
		printf("%02X:", data[i]);
	printf("%02X", data[size - 1]);
}

void DF_updateData()
{
//	DF_volumeAdjustSet(0, 0);

//	DF_query_isPlaying();
	DF_query_currentVolume();
	DF_query_currentEQ();
//	DF_query_numUsbTracks();
	DF_query_numSdTracks();
//	DF_query_currentUsbTrack();
	DF_query_currentSdTrack();
	DF_query_numFolders();

	vTaskDelay(100 / portTICK_PERIOD_MS);

	for (int i = 1; i <= num_folders; ++i)
		DF_query_numTracksInFolder(i);
}

static void initUART(void)
{
	const uart_config_t uart_config = {
			.baud_rate = 9600,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.source_clk = UART_SCLK_DEFAULT,
	};

	uart_driver_install(UART_NUM_1, UART_BUF_SIZE, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_1, &uart_config);
	uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
