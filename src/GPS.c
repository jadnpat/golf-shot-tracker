#include "GPS.h"
#include "LCD.h"
#include <string.h>
#include <stdio.h>

#define HEADER_SIZE 8
#define LAT_LON_SIZE 16
#define GPS_SENTENCE_SIZE 82

/* Local type definitions */
typedef enum
{
    GPS_STATE_IDLE,
    GPS_STATE_READING
} GPS_State_t;

/* Local function prototypes */
static void parseNmeaSentence(const char *sentence);
static void displayParsedData(void);

/* Local variables*/
static char header[HEADER_SIZE];
static char lat[LAT_LON_SIZE];
static char lon[LAT_LON_SIZE];
static GPS_State_t gpsState = GPS_STATE_IDLE;
static char gpsSentence[GPS_SENTENCE_SIZE];
static uint8_t gpsSentenceIndex = 0;

/* *****************************************************************
Name:		GPS_Main
Inputs:		none
Outputs:	none
Description:Main function for handling GPS data parsing
******************************************************************** */
void GPS_Main(void)
{
    uint8_t received;
    char c;

    if (!USART_ReceiveByte(&received))
    {
        return;
    }
    
    c = (char)received;

    switch (gpsState)
    {
        case GPS_STATE_IDLE:
            if (c == '$')
            {
                gpsSentenceIndex = 0;
                gpsSentence[gpsSentenceIndex++] = c;
                gpsState = GPS_STATE_READING;
            }
            break;

        case GPS_STATE_READING:
            gpsSentence[gpsSentenceIndex++] = c;

            if (c == '\n' || c == '\r')
            {
                gpsSentence[gpsSentenceIndex] = '\0';
                parseNmeaSentence(gpsSentence);
                gpsState = GPS_STATE_IDLE;
            }
            else if (gpsSentenceIndex >= (sizeof(gpsSentence) - 1))
            {
                gpsState = GPS_STATE_IDLE;
            }
            break;

        default:
            gpsState = GPS_STATE_IDLE;
            break;
    }
}

static void displayParsedData(void)
{
    char latLine[17];
    char lonLine[17];

    if (lat[0] == '\0' || lon[0] == '\0')
    {
        return;
    }

    snprintf(latLine, sizeof(latLine), "LAT %s", lat);
    snprintf(lonLine, sizeof(lonLine), "LON %s", lon);

    LCD_WriteString(latLine, TOP);
    LCD_WriteString(lonLine, BOTTOM);
}

static void parseNmeaSentence(const char *sentence)
{
    char sentenceCopy[GPS_SENTENCE_SIZE];
    char *fields[15];
    char *token;
    int fieldCount = 0;

    strncpy(sentenceCopy, sentence, sizeof(sentenceCopy) - 1);
    sentenceCopy[sizeof(sentenceCopy) - 1] = '\0';

    token = strtok(sentenceCopy, ",");
    while (token != NULL && fieldCount < 15)
    {
        fields[fieldCount++] = token;
        token = strtok(NULL, ",");
    }

    if (fieldCount < 7)
    {
        return;
    }

    if (strcmp(fields[2], "A") != 0)
    {
        return;
    }

    snprintf(header, sizeof(header), "%s", fields[0] + 1);
    snprintf(lat, sizeof(lat), "%s", fields[3]);
    snprintf(lon, sizeof(lon), "%s", fields[5]);

    displayParsedData();
}