/*******************************************************************************
 * @file        GPS.c
 * @brief       Parses NMEA sentences and retains the latest valid GPS point.
 *
 * @details     Reads received UART bytes, parses active RMC fixes, and derives
 *              the longitude scale factor from a cosine lookup table.
 * @author      Jared Bawden
 * @date        2026-10-03
 * @version     1.0.0
 ******************************************************************************/

/* Includes */
#include "GPS.h"
#include "LCD.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

/* Macros & Constants */
#define GPS_SENTENCE_SIZE 82 /* Max allowed size of NMEA sentence*/

/* Local type definitions */
typedef enum
{
    GPS_STATE_IDLE,
    GPS_STATE_READING
} GPS_State_t;

/* Global Variables */
static GPS_State_t gpsState = GPS_STATE_IDLE;
static char gpsSentence[GPS_SENTENCE_SIZE];
static uint8_t gpsSentenceIndex = 0;
static Point_t currentPoint = {.lon = POINT_INVALID, .lat = POINT_INVALID};
static uint8_t hasValidPoint = 0;
static uint8_t lonScaleFactor = 0;
static uint8_t lonScaleInitialized = 0;
static const uint8_t cosineByLatitudeDegree[91] PROGMEM = {
    255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 249,
    248, 247, 246, 245, 244, 243, 241, 240, 238, 236, 235, 233, 231,
    229, 227, 225, 223, 221, 219, 216, 214, 211, 209, 206, 204, 201,
    198, 195, 192, 190, 186, 183, 180, 177, 174, 171, 167, 164, 160,
    157, 153, 150, 146, 143, 139, 135, 131, 128, 124, 120, 116, 112,
    108, 104, 100, 96, 91, 87, 83, 79, 75, 70, 66, 62, 57, 53, 49,
    44, 40, 35, 31, 27, 22, 18, 13, 9, 4, 0};

/* Private Prototypes */
static Point_t parseNmeaSentence(const char *sentence);

/* Public Functions */
/**
 * @brief Consume one received byte and update the NMEA sentence parser.
 * @return None.
 */
void GPS_Main(void)
{
    uint8_t received;
    char c;
    uint8_t sentenceParsed = 0;

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
                sentenceParsed = 1;
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

    if (sentenceParsed)
    {
        if (strncmp(gpsSentence, "$GPRMC,", 7) == 0 ||
            strncmp(gpsSentence, "$GNRMC,", 7) == 0)
        {
            Point_t parsedPoint = parseNmeaSentence(gpsSentence);
            if (parsedPoint.lat != POINT_INVALID && parsedPoint.lon != POINT_INVALID)
            {
                currentPoint = parsedPoint;
                hasValidPoint = 1;
            }
        }
        sentenceParsed = 0;
    }
}

/**
 * @brief Get the most recently accepted GPS point.
 * @return The latest valid point, or POINT_INVALID before the first valid fix.
 */
Point_t GPS_GetCurrentPoint(void)
{
    return currentPoint;
}

/**
 * @brief Check whether the GPS has accepted a valid point.
 * @return Nonzero after the first valid fix; otherwise zero.
 */
uint8_t GPS_HasValidPoint(void)
{
    return hasValidPoint;
}

/**
 * @brief Get the fixed-point longitude scale derived from latitude.
 * @return The cosine scale encoded from 0 to 255.
 */
uint8_t GPS_GetLonScaleFactor(void)
{
    return lonScaleFactor;
}

/* Private Functions */
static Point_t parseNmeaSentence(const char *sentence)
{
    char sentenceCopy[GPS_SENTENCE_SIZE];
    char *fields[15];
    char *token;
    int fieldCount = 0;
    char latPoint[5];
    char lonPoint[5];
    Point_t point;
    uint8_t latitudeDegrees;

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
        point.lat = POINT_INVALID;
        point.lon = POINT_INVALID;
        return point;
    }

    if (strcmp(fields[2], "A") != 0)
    {
        point.lat = POINT_INVALID;
        point.lon = POINT_INVALID;
        return point;
    }

    
    if (strlen(fields[3]) < 9 || strlen(fields[5]) < 10 ||
        fields[3][0] < '0' || fields[3][0] > '9' ||
        fields[3][1] < '0' || fields[3][1] > '9')
    {
        point.lat = POINT_INVALID;
        point.lon = POINT_INVALID;
        return point;
    }

    latitudeDegrees = (uint8_t)((fields[3][0] - '0') * 10 + (fields[3][1] - '0'));
    if (latitudeDegrees > 90)
    {
        point.lat = POINT_INVALID;
        point.lon = POINT_INVALID;
        return point;
    }

    /* ******************************************************
    Extract latitude and longitude from format: ddmm.mmmm for 
    latitude and dddmm.mmmm for longitude into the arcminute
    / 10000 format used in Point_t.
    ****************************************************** */
    strncpy(latPoint, fields[3] + 5, 4);
    latPoint[4] = '\0';

    strncpy(lonPoint, fields[5] + 6, 4);
    lonPoint[4] = '\0';


    point.lat = (uint16_t)atoi(latPoint);
    point.lon = (uint16_t)atoi(lonPoint);

    if (!lonScaleInitialized)
    {
        lonScaleFactor = pgm_read_byte(&cosineByLatitudeDegree[latitudeDegrees]);
        lonScaleInitialized = 1;
    }

    return point;
}