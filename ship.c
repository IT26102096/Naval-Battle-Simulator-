#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ship.h"


/* =========================================================
   RANDOM NUMBER HELPER
   ========================================================= */

static double randomBetween(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}


/* =========================================================
   BATTLESHIP TYPE VALIDATION
   ========================================================= */

static int validBattleshipType(char type)
{
    type = (char)toupper((unsigned char)type);

    return type == 'U' ||
           type == 'M' ||
           type == 'R' ||
           type == 'S';
}


/* =========================================================
   ESCORT SHIP TYPE PROPERTIES
   ========================================================= */

static void setEscortType(EscortShip *escort, int randomType)
{
    switch (randomType)
    {
        case 0:
            strcpy(escort->type, "EA");
            escort->impactPower = 0.08;
            escort->angleRange = 20.0;
            break;

        case 1:
            strcpy(escort->type, "EB");
            escort->impactPower = 0.06;
            escort->angleRange = 30.0;
            break;

        case 2:
            strcpy(escort->type, "EC");
            escort->impactPower = 0.07;
            escort->angleRange = 25.0;
            break;

        case 3:
            strcpy(escort->type, "ED");
            escort->impactPower = 0.05;
            escort->angleRange = 50.0;
            break;

        default:
            strcpy(escort->type, "EE");
            escort->impactPower = 0.04;
            escort->angleRange = 70.0;
            break;
    }
}


/* =========================================================
   ESCORT SHIP GENERATION
   ========================================================= */

static void generateEscortShips(Battlefield *battlefield)
{
    int i;

    for (i = 0; i < battlefield->escortCount; i++)
    {
        EscortShip *escort = &battlefield->escorts[i];

        /* Unique ID */
        escort->id = i + 1;


        /* Random position inside the canvas */
        escort->x = randomBetween(
            0.0,
            battlefield->canvasSize
        );

        escort->y = randomBetween(
            0.0,
            battlefield->canvasSize
        );


        /* Random Escort type */
        setEscortType(
            escort,
            rand() % 5
        );


        /* Random valid minimum angle */
        escort->minAngle = randomBetween(
            0.0,
            90.0 - escort->angleRange
        );

        escort->maxAngle =
            escort->minAngle +
            escort->angleRange;


        /*
           EA maximum velocity is
           1.2 * Battleship maximum velocity.

           Other Escort maximum velocities
           are randomly generated below
           Battleship maximum velocity.
        */

        if (strcmp(escort->type, "EA") == 0)
        {
            escort->maxVelocity =
                1.2 *
                battlefield->battleship.maxVelocity;
        }
        else
        {
            escort->maxVelocity =
                randomBetween(
                    0.55 *
                    battlefield->battleship.maxVelocity,

                    0.95 *
                    battlefield->battleship.maxVelocity
                );
        }


        /* Random minimum velocity */
        escort->minVelocity =
            randomBetween(
                0.20 *
                escort->maxVelocity,

                0.60 *
                escort->maxVelocity
            );


        /* Ship starts alive */
        escort->alive = 1;


        /*
           Part 1-C:
           Every Escort ship initially
           has not fired.
        */
        escort->hasFired = 0;
    }
}


/* =========================================================
   BATTLEFIELD SETUP
   ========================================================= */

void setupBattlefield(Battlefield *battlefield)
{
    char type;
    unsigned int seed;


    printf("\n--- Battlefield Setup ---\n");


    /* Canvas size */

    do
    {
        printf("Enter square canvas size D: ");

        scanf(
            "%lf",
            &battlefield->canvasSize
        );

    }
    while (battlefield->canvasSize <= 0.0);


    /* Number of Escort ships */

    do
    {
        printf(
            "Enter number of escort ships (1-%d): ",
            MAX_ESCORTS
        );

        scanf(
            "%d",
            &battlefield->escortCount
        );

    }
    while (
        battlefield->escortCount < 1 ||
        battlefield->escortCount > MAX_ESCORTS
    );


    /* Battleship type */

    do
    {
        printf(
            "Battleship type [U/M/R/S]: "
        );

        scanf(
            " %c",
            &type
        );

        type =
            (char)toupper(
                (unsigned char)type
            );

    }
    while (!validBattleshipType(type));


    battlefield->battleship.type = type;


    /* Battleship X position */

    do
    {
        printf(
            "Battleship X position (0 to %.2f): ",
            battlefield->canvasSize
        );

        scanf(
            "%lf",
            &battlefield->battleship.x
        );

    }
    while (
        battlefield->battleship.x < 0.0 ||
        battlefield->battleship.x >
        battlefield->canvasSize
    );


    /* Battleship Y position */

    do
    {
        printf(
            "Battleship Y position (0 to %.2f): ",
            battlefield->canvasSize
        );

        scanf(
            "%lf",
            &battlefield->battleship.y
        );

    }
    while (
        battlefield->battleship.y < 0.0 ||
        battlefield->battleship.y >
        battlefield->canvasSize
    );


    /* Battleship maximum shell velocity */

    do
    {
        printf(
            "Battleship maximum shell velocity: "
        );

        scanf(
            "%lf",
            &battlefield->battleship.maxVelocity
        );

    }
    while (
        battlefield->battleship.maxVelocity <= 0.0
    );


    /* Random seed */

    printf(
        "Enter random seed value: "
    );

    scanf(
        "%u",
        &seed
    );


    srand(seed);


    /* =====================================================
       Battleship initial status
       ===================================================== */

    battlefield->battleship.alive = 1;


    /*
       Part 1-C:
       1.0 represents 100% health.
    */

    battlefield->battleship.health = 1.0;


    /*
       At the beginning, Battleship has
       received no damage.
    */

    battlefield->battleship.cumulativeImpact = 0.0;


    /* Generate Escort ships */

    generateEscortShips(
        battlefield
    );


    printf(
        "\nBattlefield setup completed.\n"
    );
}


/* =========================================================
   DISPLAY BATTLEFIELD
   ========================================================= */

void printBattlefield(
    const Battlefield *battlefield
)
{
    int i;


    printf(
        "\n========== BATTLEFIELD ==========\n"
    );


    printf(
        "Canvas: (0,0) to (%.2f, %.2f)\n",
        battlefield->canvasSize,
        battlefield->canvasSize
    );


    /* Battleship details */

    printf(
        "\nBattleship\n"
    );


    printf(
        "Type: %c\n",
        battlefield->battleship.type
    );


    printf(
        "Position: (%.2f, %.2f)\n",
        battlefield->battleship.x,
        battlefield->battleship.y
    );


    printf(
        "Maximum shell velocity: %.2f\n",
        battlefield->battleship.maxVelocity
    );


    printf(
        "Status: %s\n",
        battlefield->battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    /*
       Part 1-C values
    */

    printf(
        "Health: %.2f%%\n",
        battlefield->battleship.health * 100.0
    );


    printf(
        "Cumulative Impact: %.2f\n",
        battlefield->battleship.cumulativeImpact
    );


    /* Escort Ship details */

    printf(
        "\nEscort Ships\n"
    );


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        const EscortShip *escort =
            &battlefield->escorts[i];


        printf(
            "\nE%d [%s]\n",
            escort->id,
            escort->type
        );


        printf(
            "Position: (%.2f, %.2f)\n",
            escort->x,
            escort->y
        );


        printf(
            "Velocity: %.2f - %.2f\n",
            escort->minVelocity,
            escort->maxVelocity
        );


        printf(
            "Angle: %.2f - %.2f degrees\n",
            escort->minAngle,
            escort->maxAngle
        );


        printf(
            "Impact Power: %.2f\n",
            escort->impactPower
        );


        printf(
            "Status: %s\n",
            escort->alive
                ? "ALIVE"
                : "DESTROYED"
        );


        printf(
            "Has Fired: %s\n",
            escort->hasFired
                ? "YES"
                : "NO"
        );
    }
}