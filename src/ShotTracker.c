/*******************************************************************************
 * @file        ShotTracker.c
 * @brief       Tracks shot distances and scores across an 18-hole round.
 *
 * @details     Handles shot and hole button events, calculates distances,
 *              and stores per-hole shot counts for the round.
 * @author      Jared Bawden
 * @date        2026-10-05
 * @version     1.0.0
 ******************************************************************************/

/* Includes */
#include "ShotTracker.h"
#include "LCD.h"
#include "Buttons.h"
#include "GPS.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Local defines */
#define HOLE_COUNT 18
#define MAX_SHOTS_PER_HOLE 10

/* Local type definitions */
typedef enum
{
    STATE_IDLE,
    STATE_TRACKING,
    STATE_BETWEEN_HOLES,
    STATE_SHOT_LIMIT_REACHED,
    STATE_ROUND_COMPLETE
} ShotTrackerState_t;

typedef struct
{
    uint16_t distancesMetres[MAX_SHOTS_PER_HOLE];
    uint8_t shotCount;
} Hole_t;

/* Global Variables */
static ShotTrackerState_t state = STATE_IDLE;
static Hole_t holes[HOLE_COUNT];
static uint8_t currentHole = 0;
static Point_t shotStartPoint;
static Point_t lastDisplayedPoint = {.lat = POINT_INVALID, .lon = POINT_INVALID};
static uint16_t currentDistance = 0;
static uint8_t roundCompleteDisplayed = 0;

/* Private Prototypes */
static uint8_t pointCompare(Point_t point1, Point_t point2);
static int16_t wrappedDifference(uint16_t start, uint16_t end);
static uint16_t calculateDistance(Point_t point1, Point_t point2);
static void displayTracking(uint16_t distance);
static void displayHoleReady(void);
static void storeCurrentShot(void);
static void advanceHole(void);
static uint16_t calculateRoundScore(void);

/* Public Functions */
/**
 * @brief Reset the round state and display the first-hole prompt.
 * @return None.
 */
void ShotTracker_Init(void)
{
    memset(holes, 0, sizeof(holes));
    currentHole = 0;
    currentDistance = 0;
    lastDisplayedPoint.lat = POINT_INVALID;
    lastDisplayedPoint.lon = POINT_INVALID;
    roundCompleteDisplayed = 0;
    state = STATE_IDLE;
    displayHoleReady();
}

/**
 * @brief Process button events and update the active shot-tracking state.
 * @return None.
 */
void ShotTracker_Main(void)
{
    Point_t currentPoint;

    switch (state)
    {
        case STATE_IDLE:
            if (holeBtnFlag)
            {
                holeBtnFlag = 0;
                advanceHole();
                break;
            }

            if (shotBtnFlag)
            {
                shotBtnFlag = 0;
                if (!GPS_HasValidPoint())
                {
                    LCD_WriteString("Waiting for GPS", TOP);
                    break;
                }

                shotStartPoint = GPS_GetCurrentPoint();
                currentDistance = 0;
                lastDisplayedPoint.lat = POINT_INVALID;
                lastDisplayedPoint.lon = POINT_INVALID;
                state = STATE_TRACKING;
            }
            break;

        case STATE_TRACKING:
            if (holeBtnFlag)
            {
                holeBtnFlag = 0;
                currentPoint = GPS_GetCurrentPoint();
                currentDistance = calculateDistance(shotStartPoint, currentPoint);
                storeCurrentShot();
                advanceHole();
                break;
            }

            currentPoint = GPS_GetCurrentPoint();
            if (!pointCompare(currentPoint, lastDisplayedPoint))
            {
                currentDistance = calculateDistance(shotStartPoint, currentPoint);
                displayTracking(currentDistance);
                lastDisplayedPoint = currentPoint;
            }

            if (shotBtnFlag)
            {
                shotBtnFlag = 0;

                if (holes[currentHole].shotCount >= MAX_SHOTS_PER_HOLE)
                {
                    LCD_WriteString("Pick it up mate", BOTTOM);
                    state = STATE_SHOT_LIMIT_REACHED;
                    break;
                }

                storeCurrentShot();
                shotStartPoint = currentPoint;
                lastDisplayedPoint.lat = POINT_INVALID;
                lastDisplayedPoint.lon = POINT_INVALID;
                displayTracking(0);
            }
            break;

        case STATE_BETWEEN_HOLES:
            displayHoleReady();
            state = STATE_IDLE;
            break;

        case STATE_SHOT_LIMIT_REACHED:
            shotBtnFlag = 0;
            if (holeBtnFlag)
            {
                holeBtnFlag = 0;
                advanceHole();
            }
            break;

        case STATE_ROUND_COMPLETE:
            if (!roundCompleteDisplayed)
            {
                char scoreLine[17];
                LCD_WriteString("Round complete", TOP);
                snprintf(scoreLine, sizeof(scoreLine), "Total score: %u",
                         (unsigned int)calculateRoundScore());
                LCD_WriteString(scoreLine, BOTTOM);
                roundCompleteDisplayed = 1;
            }
            break;

        default:
            state = STATE_IDLE;
            break;
    }
}

/* Private Functions */
static uint8_t pointCompare(Point_t point1, Point_t point2)
{
    return point1.lat == point2.lat && point1.lon == point2.lon;
}

static int16_t wrappedDifference(uint16_t start, uint16_t end)
{
    int16_t difference = (int16_t)end - (int16_t)start;

    if (difference > 5000)
    {
        difference -= 10000;
    }
    else if (difference < -5000)
    {
        difference += 10000;
    }

    return difference;
}

static uint16_t calculateDistance(Point_t point1, Point_t point2)
{
    int16_t latitudeDifference = wrappedDifference(point1.lat, point2.lat);
    int16_t longitudeDifference = wrappedDifference(point1.lon, point2.lon);
    
    float scaledLongitudeDifference =
        ((float)longitudeDifference * GPS_GetLonScaleFactor()) / 255.0f;
    float distanceUnits = sqrtf(
        (float)latitudeDifference * latitudeDifference +
        scaledLongitudeDifference * scaledLongitudeDifference);

    return (uint16_t)(distanceUnits * 0.1855f + 0.5f);
}

static void displayTracking(uint16_t distance)
{
    char line[17];

    snprintf(line, sizeof(line), "H%02u SHOT %02u",
             (unsigned int)(currentHole + 1),
             (unsigned int)(holes[currentHole].shotCount + 1));
    LCD_WriteString(line, TOP);

    snprintf(line, sizeof(line), "DIST %um", (unsigned int)distance);
    LCD_WriteString(line, BOTTOM);
}

static void displayHoleReady(void)
{
    char line[17];

    snprintf(line, sizeof(line), "HOLE %02u READY",
             (unsigned int)(currentHole + 1));
    LCD_WriteString(line, TOP);
    LCD_WriteString("Press shot btn", BOTTOM);
}

static void storeCurrentShot(void)
{
    Hole_t *hole = &holes[currentHole];

    if (hole->shotCount < MAX_SHOTS_PER_HOLE)
    {
        hole->distancesMetres[hole->shotCount++] = currentDistance;
    }
}

static void advanceHole(void)
{
    if (currentHole + 1 < HOLE_COUNT)
    {
        currentHole++;
        state = STATE_BETWEEN_HOLES;
    }
    else
    {
        state = STATE_ROUND_COMPLETE;
    }
}

static uint16_t calculateRoundScore(void)
{
    uint16_t totalScore = 0;

    for (uint8_t holeIndex = 0; holeIndex < HOLE_COUNT; holeIndex++)
    {
        totalScore += holes[holeIndex].shotCount;
    }

    return totalScore;
}