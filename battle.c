#include <stdio.h>
#include <math.h>

#include "battle.h"

#define GRAVITY 9.81
#define PI 3.14159265358979323846


double calculateDistance(
    double x1,
    double y1,
    double x2,
    double y2
)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt((dx * dx) + (dy * dy));
}


int findShotSolution(
    double distance,
    double minVelocity,
    double maxVelocity,
    double minAngle,
    double maxAngle,
    ShotSolution *solution
)
{
    double angle;

    solution->possible = 0;

    /*
       Try different firing angles.

       For each angle, calculate the velocity
       required to reach the target.
    */

    for (
        angle = minAngle;
        angle <= maxAngle;
        angle += 0.1
    )
    {
        double radians;
        double sinTwoTheta;
        double requiredVelocity;
        double flightTime;

        radians = angle * PI / 180.0;

        sinTwoTheta = sin(2.0 * radians);

        if (sinTwoTheta <= 0.000001)
        {
            continue;
        }

        requiredVelocity =
            sqrt(
                (distance * GRAVITY) /
                sinTwoTheta
            );

        if (
            requiredVelocity >= minVelocity &&
            requiredVelocity <= maxVelocity
        )
        {
            flightTime =
                (2.0 *
                 requiredVelocity *
                 sin(radians)) /
                GRAVITY;

            /*
               Select the fastest valid shot.
            */

            if (
                !solution->possible ||
                flightTime < solution->time
            )
            {
                solution->possible = 1;
                solution->velocity =
                    requiredVelocity;

                solution->angle =
                    angle;

                solution->time =
                    flightTime;
            }
        }
    }

    return solution->possible;
}


static void saveInitialBattlefield(
    const Battlefield *battlefield
)
{
    FILE *file;
    int i;

    file = fopen(
        "part1a_initial.txt",
        "w"
    );

    if (file == NULL)
    {
        printf(
            "Error creating initial state file.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 1-A INITIAL BATTLEFIELD\n"
    );

    fprintf(
        file,
        "============================\n\n"
    );


    fprintf(
        file,
        "Canvas Size: %.2f x %.2f\n\n",
        battlefield->canvasSize,
        battlefield->canvasSize
    );


    fprintf(
        file,
        "Battleship\n"
    );

    fprintf(
        file,
        "Type: %c\n",
        battlefield->battleship.type
    );

    fprintf(
        file,
        "Position: (%.2f, %.2f)\n",
        battlefield->battleship.x,
        battlefield->battleship.y
    );

    fprintf(
        file,
        "Maximum Velocity: %.2f\n",
        battlefield->battleship.maxVelocity
    );

    fprintf(
        file,
        "Minimum Angle: 0.00\n"
    );

    fprintf(
        file,
        "Maximum Angle: 90.00\n\n"
    );


    fprintf(
        file,
        "Escort Ships\n"
    );

    fprintf(
        file,
        "------------\n"
    );


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        const EscortShip *escort =
            &battlefield->escorts[i];

        fprintf(
            file,
            "\nEscort ID: %d\n",
            escort->id
        );

        fprintf(
            file,
            "Type: %s\n",
            escort->type
        );

        fprintf(
            file,
            "Position: (%.2f, %.2f)\n",
            escort->x,
            escort->y
        );

        fprintf(
            file,
            "Minimum Velocity: %.2f\n",
            escort->minVelocity
        );

        fprintf(
            file,
            "Maximum Velocity: %.2f\n",
            escort->maxVelocity
        );

        fprintf(
            file,
            "Minimum Angle: %.2f\n",
            escort->minAngle
        );

        fprintf(
            file,
            "Maximum Angle: %.2f\n",
            escort->maxAngle
        );

        fprintf(
            file,
            "Impact Power: %.2f\n",
            escort->impactPower
        );
    }


    fclose(file);
}


static void saveFinalBattlefield(
    const Battlefield *battlefield
)
{
    FILE *file;
    int i;

    file = fopen(
        "part1a_final.txt",
        "w"
    );

    if (file == NULL)
    {
        printf(
            "Error creating final state file.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 1-A FINAL BATTLEFIELD\n"
    );

    fprintf(
        file,
        "==========================\n\n"
    );


    fprintf(
        file,
        "Battleship Status: %s\n",
        battlefield->battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    fprintf(
        file,
        "Battleship Position: (%.2f, %.2f)\n\n",
        battlefield->battleship.x,
        battlefield->battleship.y
    );


    fprintf(
        file,
        "Escort Ship Status\n"
    );

    fprintf(
        file,
        "------------------\n"
    );


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        fprintf(
            file,
            "E%d [%s] : %s\n",
            battlefield->escorts[i].id,
            battlefield->escorts[i].type,
            battlefield->escorts[i].alive
                ? "ALIVE"
                : "DESTROYED"
        );
    }


    fclose(file);
}


void runPart1A(
    Battlefield *battlefield
)
{
    int i;

    int sinkingEscortIndex = -1;

    double earliestImpactTime = 0.0;

    int destroyedEscortCount = 0;

    double battleEndTime = 0.0;

    FILE *hitFile;


    printf(
        "\n====================================\n"
    );

    printf(
        "          PART 1-A SIMULATION\n"
    );

    printf(
        "====================================\n"
    );


    saveInitialBattlefield(
        battlefield
    );


    /*
       First determine whether any Escort ship
       can successfully hit the Battleship.
    */

    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution shot;

        double distance =
            calculateDistance(
                escort->x,
                escort->y,
                battlefield->battleship.x,
                battlefield->battleship.y
            );


        if (
            findShotSolution(
                distance,
                escort->minVelocity,
                escort->maxVelocity,
                escort->minAngle,
                escort->maxAngle,
                &shot
            )
        )
        {
            if (
                sinkingEscortIndex == -1 ||
                shot.time < earliestImpactTime
            )
            {
                sinkingEscortIndex = i;
                earliestImpactTime =
                    shot.time;
            }
        }
    }


    /*
       In Part 1-A, one shell impact is enough
       to destroy the Battleship.
    */

    if (sinkingEscortIndex != -1)
    {
        battlefield->battleship.alive = 0;

        printf(
            "\nBattleship has been destroyed.\n"
        );

        printf(
            "Escort ship E%d [%s] sank the Battleship.\n",
            battlefield
                ->escorts[sinkingEscortIndex]
                .id,

            battlefield
                ->escorts[sinkingEscortIndex]
                .type
        );

        printf(
            "Impact time: %.2f seconds\n",
            earliestImpactTime
        );


        saveFinalBattlefield(
            battlefield
        );


        printf(
            "\nResults saved to:\n"
        );

        printf(
            "part1a_initial.txt\n"
        );

        printf(
            "part1a_final.txt\n"
        );


        return;
    }


    /*
       No Escort ship can hit B.
       The Battleship now attacks every
       Escort ship within its attack range.
    */

    hitFile = fopen(
        "part1a_hits.txt",
        "w"
    );


    if (hitFile == NULL)
    {
        printf(
            "Error creating hit details file.\n"
        );

        return;
    }


    fprintf(
        hitFile,
        "PART 1-A BATTLESHIP HIT DETAILS\n"
    );

    fprintf(
        hitFile,
        "================================\n\n"
    );


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution shot;

        double distance =
            calculateDistance(
                battlefield->battleship.x,
                battlefield->battleship.y,
                escort->x,
                escort->y
            );


        if (
            findShotSolution(
                distance,
                0.0,
                battlefield
                    ->battleship
                    .maxVelocity,
                0.1,
                89.9,
                &shot
            )
        )
        {
            escort->alive = 0;

            destroyedEscortCount++;


            if (
                shot.time >
                battleEndTime
            )
            {
                battleEndTime =
                    shot.time;
            }


            fprintf(
                hitFile,
                "Escort ID: %d\n",
                escort->id
            );

            fprintf(
                hitFile,
                "Type: %s\n",
                escort->type
            );

            fprintf(
                hitFile,
                "Distance: %.2f\n",
                distance
            );

            fprintf(
                hitFile,
                "Firing Velocity: %.2f\n",
                shot.velocity
            );

            fprintf(
                hitFile,
                "Firing Angle: %.2f\n",
                shot.angle
            );

            fprintf(
                hitFile,
                "Time to Hit: %.2f seconds\n\n",
                shot.time
            );
        }
    }


    fclose(hitFile);


    printf(
        "\nBattleship survived.\n"
    );

    printf(
        "Escort ships destroyed: %d\n",
        destroyedEscortCount
    );

    printf(
        "Battle duration: %.2f seconds\n",
        battleEndTime
    );


    saveFinalBattlefield(
        battlefield
    );


    printf(
        "\nResults saved to:\n"
    );

    printf(
        "part1a_initial.txt\n"
    );

    printf(
        "part1a_hits.txt\n"
    );

    printf(
        "part1a_final.txt\n"
    );
}