#ifndef DF_PLAYER_H
#define DF_PLAYER_H

#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"
#include "iotv.h"

/* Packet Values */
#define DF_BUFFER_SIZE          10   // total number of bytes in a stack/packet (same for cmds and queries)
#define SB                      0x7E // start byte
#define VER                     0xFF // version
#define LEN                     0x06 // number of bytes after "LEN" (except for checksum data and EB)
#define FEEDBACK                1    // feedback requested
#define NO_FEEDBACK             0    // no feedback requested
#define EB                      0xEF // end byte

/* Control Commands */
#define CTRL_NEXT               0x01
#define CTRL_PREV               0x02
#define CTRL_SPEC_PLAY          0x03 // Specify playback of a track(in the root directory of a storage device)
#define CTRL_INC_VOL            0x04
#define CTRL_DEC_VOL            0x05
#define CTRL_VOLUME             0x06
#define CTRL_EQ                 0x07
#define CTRL_PLAYBACK_MODE      0x08 // Specify single repeat playback
#define CTRL_PLAYBACK_SRC       0x09 // Specify playback of a device(USB/SD)
#define CTRL_SLEEP              0x0A
#define CTRL_RESET              0x0C
#define CTRL_PLAY               0x0D
#define CTRL_PAUSE              0x0E
#define CTRL_SPEC_FOLDER        0x0F // Specify playback a track in a folder
#define CTRL_AUDIO_AMPL         0x10 // Audio amplification setting
#define CTRL_REPEAT_PLAY        0x11 // Set all repeat playback
#define CTRL_SPEC_PLAY_MP3      0x12 // Specify playback of folder named “MP3”
#define CTRL_INSERT_ADVERT      0x13 // Insert an advertisement
#define CTRL_SPEC_TRACK_3000    0x14 // Specify playback a track in a folder that supports 3000 tracks
#define CTRL_STOP_ADVERT        0x15
#define CTRL_STOP               0x16
#define CTRL_REPEAT_FOLDER      0x17 // Specify repeat playback of a folder
#define CTRL_RANDOM_ALL         0x18
#define CTRL_REPEAT_CURRENT     0x19
#define CTRL_SET_DAC            0x1A

/* Query Command */
#define CMD_CUR_DEV_ONLINE      0x3F // Query current online storage device
#define CMD_ERROR               0x40 // Module returns an error data with this command
#define CMD_FEEDBACK            0x41 // Module reports a feedback with this command
#define CMD_STATUS              0x42 // Query current status
#define CMD_VOLUME              0x43 // Query current volume
#define CMD_EQ                  0x44 // Query current EQ
#define CMD_USB_FILES           0x47 // Query number of tracks in the root of USB flash drive
#define CMD_SD_FILES            0x48 // Query number of tracks in the root of micro SD card
#define CMD_USB_TRACK           0x4B // Query current track in the USB flash drive
#define CMD_SD_TRACK            0x4C // Query current track in the micro SD Card
#define CMD_FOLDER_FILES        0x4E // Query number of tracks in a folder
#define CMD_FOLDERS             0x4F // Query number of folders in the current storage device

#define CMD_DEV_PLUGGED         0x3A // storage device is plugged
#define CMD_DEV_PULL_OUT        0x3B // storage device is pull out
#define CMD_TRACK_FINSH_USB     0x3C // track is finished playing in USB flash drive
#define CMD_TRACK_FINSH_SD      0x3D // track is finished playing in USB flash drive

/* EQ Values */
#define EQ_NORMAL               0
#define EQ_POP                  1
#define EQ_ROCK                 2
#define EQ_JAZZ                 3
#define EQ_CLASSIC              4
#define EQ_BASE                 5

/* Volume */
#define VOLUME_MIN				0
#define VOLUME_MAX				30

/* Specify playback of a device */
#define SRC_USB                 1
#define SRC_SD                  2

#define START_BYTE      0
#define VERSION         1
#define LENGTH          2
#define CMD_VALUE       3
#define FEEDBAC_VALUE   4
#define PARAM_MSB       5
#define PARAM_LSB       6
#define CHECKSUM_MSB    7
#define CHECKSUM_LSB    8
#define END_BYTE        9

void DF_Task(void *pvParameters);
void DF_dataRecive_Task(void *pvParameters);

void DF_playNext();
void DF_playPrevious();
void DF_play(uint16_t trackNum);
void DF_stop();
void DF_playFromRootFolder(uint16_t trackNum);
void DF_playFromMP3Folder(uint16_t trackNum);
void DF_playAdvertisement(uint16_t trackNum);
void DF_stopAdvertisement();
void DF_incVolume();
void DF_decVolume();
void DF_volume(uint8_t value);
void DF_EQSelect(uint8_t setting);
void DF_loop(uint16_t trackNum);
void DF_playbackSource(uint8_t source);
void DF_reset();
void DF_pause();
void DF_playFolder(uint8_t folderNum, uint8_t trackNum);
void DF_playLargeFolder(uint8_t folderNum, uint16_t trackNum);
void DF_volumeAdjustSet(uint8_t on_off, uint8_t gain);
void DF_startRepeatPlay();
void DF_stopRepeatPlay();
void DF_repeatFolder(uint16_t folder);
void DF_randomAll();
void DF_startRepeat();
void DF_stopRepeat();
void DF_startDAC();
void DF_stopDAC();
void DF_sleep();

void DF_query_isPlaying();
void DF_query_currentVolume();
void DF_query_currentEQ();
void DF_query_numUsbTracks();
void DF_query_numSdTracks();
void DF_query_currentUsbTrack();
void DF_query_currentSdTrack();
void DF_query_numTracksInFolder(uint16_t folder);
void DF_query_numFolders();

void DF_calculateChecksum();

void DF_sendData();

void DF_query(uint8_t cmd, uint8_t msb, uint8_t lsb);

void DF_parseData();

void DF_printError();
void DF_printBuff(uint8_t *data, uint8_t size);

void DF_updateData();

#endif // DF_PLAYER_H
