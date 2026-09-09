#include <stdio.h>
#include <stdlib.h>
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

/*part 1-B*/
#define MAX_PATH_POINTS 50

typedef struct
{
    double x;
    double y;

} PathPoint;


static double randomPathCoordinate(double maximum)
{
    return ((double)rand() / RAND_MAX) * maximum;
}


static int countAliveEscorts(
    const Battlefield *battlefield
)
{
    int i;
    int count = 0;

    for (i = 0; i < battlefield->escortCount; i++)
    {
        if (battlefield->escorts[i].alive)
        {
            count++;
        }
    }

    return count;
}


static void writeBattlefieldState(
    FILE *file,
    const Battlefield *battlefield
)
{
    int i;

    fprintf(
        file,
        "Battleship Type: %c\n",
        battlefield->battleship.type
    );

    fprintf(
        file,
        "Battleship Position: (%.2f, %.2f)\n",
        battlefield->battleship.x,
        battlefield->battleship.y
    );

    fprintf(
        file,
        "Battleship Status: %s\n\n",
        battlefield->battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    fprintf(
        file,
        "Escort Ships\n"
    );

    fprintf(
        file,
        "------------\n"
    );


    for (i = 0; i < battlefield->escortCount; i++)
    {
        const EscortShip *escort =
            &battlefield->escorts[i];

        fprintf(
            file,
            "E%d [%s]\n",
            escort->id,
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
            "Velocity: %.2f - %.2f\n",
            escort->minVelocity,
            escort->maxVelocity
        );

        fprintf(
            file,
            "Angle: %.2f - %.2f\n",
            escort->minAngle,
            escort->maxAngle
        );

        fprintf(
            file,
            "Impact Power: %.2f\n",
            escort->impactPower
        );

        fprintf(
            file,
            "Status: %s\n\n",
            escort->alive
                ? "ALIVE"
                : "DESTROYED"
        );
    }
}


static int simulatePart1BIteration(
    Battlefield *battlefield,
    double battleshipMinAngle,
    double battleshipMaxAngle,
    FILE *file
)
{
    int i;

    int sinkingEscortIndex = -1;

    double earliestImpactTime = 0.0;

    int destroyedThisIteration = 0;

    double battleEndTime = 0.0;


    /*
       First check whether any living Escort ship
       can hit the Battleship.
    */

    for (i = 0; i < battlefield->escortCount; i++)
    {
        EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution shot;
        double distance;


        if (!escort->alive)
        {
            continue;
        }


        distance =
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
       Part 1-B still uses Part 1-A damage rules.
       One Escort hit destroys B.
    */

    if (sinkingEscortIndex != -1)
    {
        EscortShip *attacker =
            &battlefield->escorts[sinkingEscortIndex];

        battlefield->battleship.alive = 0;


        printf(
            "Battleship destroyed by E%d [%s] at %.2f seconds.\n",
            attacker->id,
            attacker->type,
            earliestImpactTime
        );


        fprintf(
            file,
            "\nRESULT\n"
        );

        fprintf(
            file,
            "Battleship destroyed by E%d [%s]\n",
            attacker->id,
            attacker->type
        );

        fprintf(
            file,
            "Impact Time: %.2f seconds\n",
            earliestImpactTime
        );


        return 1;
    }


    /*
       If B survives Escort attacks,
       attack every living reachable Escort.
    */

    for (i = 0; i < battlefield->escortCount; i++)
    {
        EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution shot;
        double distance;


        if (!escort->alive)
        {
            continue;
        }


        distance =
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
                battlefield->battleship.maxVelocity,
                battleshipMinAngle,
                battleshipMaxAngle,
                &shot
            )
        )
        {
            escort->alive = 0;

            destroyedThisIteration++;


            if (shot.time > battleEndTime)
            {
                battleEndTime =
                    shot.time;
            }


            fprintf(
                file,
                "\nBattleship hit E%d [%s]\n",
                escort->id,
                escort->type
            );

            fprintf(
                file,
                "Distance: %.2f\n",
                distance
            );

            fprintf(
                file,
                "Velocity: %.2f\n",
                shot.velocity
            );

            fprintf(
                file,
                "Angle: %.2f\n",
                shot.angle
            );

            fprintf(
                file,
                "Time to Hit: %.2f seconds\n",
                shot.time
            );
        }
    }


    fprintf(
        file,
        "\nEscort ships destroyed in this iteration: %d\n",
        destroyedThisIteration
    );

    fprintf(
        file,
        "Iteration battle duration: %.2f seconds\n",
        battleEndTime
    );


    printf(
        "Battleship survived this iteration. Escorts destroyed: %d\n",
        destroyedThisIteration
    );


    return 0;
}


static int runPathSimulation(
    Battlefield *battlefield,
    const PathPoint path[],
    int pathCount,
    int simulationNumber,
    int jamAfterIterations,
    double jamAngle
)
{
    int i;
    int completedIterations = 0;


    for (
        i = 0;
        i < pathCount &&
        battlefield->battleship.alive;
        i++
    )
    {
        char filename[100];

        FILE *file;

        double minimumAngle = 0.1;

        int gunJammed = 0;


        /*
           Simulation 2:
           First t iterations are normal.
           From iteration t + 1 onward,
           the gun is jammed.
        */

        if (
            simulationNumber == 2 &&
            i >= jamAfterIterations
        )
        {
            minimumAngle = jamAngle;
            gunJammed = 1;
        }


        battlefield->battleship.x =
            path[i].x;

        battlefield->battleship.y =
            path[i].y;


        snprintf(
            filename,
            sizeof(filename),
            "part1b_sim%d_iteration_%d.txt",
            simulationNumber,
            i + 1
        );


        file = fopen(
            filename,
            "w"
        );


        if (file == NULL)
        {
            printf(
                "Could not create %s\n",
                filename
            );

            continue;
        }


        fprintf(
            file,
            "PART 1-B SIMULATION %d\n",
            simulationNumber
        );

        fprintf(
            file,
            "Iteration: %d\n",
            i + 1
        );

        fprintf(
            file,
            "============================\n\n"
        );


        fprintf(
            file,
            "Path Position: (%.2f, %.2f)\n",
            path[i].x,
            path[i].y
        );


        if (simulationNumber == 2)
        {
            fprintf(
                file,
                "Gun Status: %s\n",
                gunJammed
                    ? "JAMMED"
                    : "NORMAL"
            );

            fprintf(
                file,
                "Battleship Firing Angle: %.2f - 90.00 degrees\n\n",
                minimumAngle
            );
        }


        fprintf(
            file,
            "STATE BEFORE ITERATION\n"
        );

        fprintf(
            file,
            "----------------------\n"
        );


        writeBattlefieldState(
            file,
            battlefield
        );


        simulatePart1BIteration(
            battlefield,
            minimumAngle,
            89.9,
            file
        );


        fprintf(
            file,
            "\nSTATE AFTER ITERATION\n"
        );

        fprintf(
            file,
            "---------------------\n"
        );


        writeBattlefieldState(
            file,
            battlefield
        );


        fclose(file);


        completedIterations++;


        printf(
            "Iteration %d completed. Result saved to %s\n",
            i + 1,
            filename
        );
    }


    return completedIterations;
}


static void savePart1BSummary(
    const char *filename,
    const Battlefield *battlefield,
    int completedIterations
)
{
    FILE *file;

    file = fopen(
        filename,
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Could not create %s\n",
            filename
        );

        return;
    }


    fprintf(
        file,
        "PART 1-B SUMMARY\n"
    );

    fprintf(
        file,
        "================\n\n"
    );


    fprintf(
        file,
        "Completed Iterations: %d\n",
        completedIterations
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
        "Escort Ships Remaining: %d\n",
        countAliveEscorts(battlefield)
    );


    fclose(file);
}


void runPart1B(
    const Battlefield *initialBattlefield
)
{
    Battlefield simulation1;
    Battlefield simulation2;

    PathPoint path[MAX_PATH_POINTS];

    int pathCount;
    int jamAfterIterations;

    double jamAngle;

    unsigned int pathSeed;

    int sim1Iterations;
    int sim2Iterations;

    int i;


    /*
       Make two copies so both simulations
       start from exactly the same battlefield.
    */

    simulation1 =
        *initialBattlefield;

    simulation2 =
        *initialBattlefield;


    printf(
        "\n====================================\n"
    );

    printf(
        "          PART 1-B SIMULATIONS\n"
    );

    printf(
        "====================================\n"
    );


    do
    {
        printf(
            "Enter number of path points k (2-%d): ",
            MAX_PATH_POINTS
        );

        scanf(
            "%d",
            &pathCount
        );

    }
    while (
        pathCount < 2 ||
        pathCount > MAX_PATH_POINTS
    );


    printf(
        "Enter path random seed: "
    );

    scanf(
        "%u",
        &pathSeed
    );


    srand(
        pathSeed
    );


    /*
       Generate the path only once.
       Both simulations use the same path.
    */

    for (i = 0; i < pathCount; i++)
    {
        path[i].x =
            randomPathCoordinate(
                initialBattlefield->canvasSize
            );

        path[i].y =
            randomPathCoordinate(
                initialBattlefield->canvasSize
            );
    }


    do
    {
        printf(
            "Enter t (gun jams after t iterations, 1 to %d): ",
            pathCount - 1
        );

        scanf(
            "%d",
            &jamAfterIterations
        );

    }
    while (
        jamAfterIterations < 1 ||
        jamAfterIterations >= pathCount
    );


    do
    {
        printf(
            "Enter jammed minimum firing angle (0 < angle < 30): "
        );

        scanf(
            "%lf",
            &jamAngle
        );

    }
    while (
        jamAngle <= 0.0 ||
        jamAngle >= 30.0
    );


    printf(
        "\nGenerated Battleship Path\n"
    );

    printf(
        "-------------------------\n"
    );


    for (i = 0; i < pathCount; i++)
    {
        printf(
            "Point %d: (%.2f, %.2f)\n",
            i + 1,
            path[i].x,
            path[i].y
        );
    }


    printf(
        "\n--- Simulation 1: Normal Gun ---\n"
    );


    sim1Iterations =
        runPathSimulation(
            &simulation1,
            path,
            pathCount,
            1,
            0,
            0.0
        );


    savePart1BSummary(
        "part1b_sim1_summary.txt",
        &simulation1,
        sim1Iterations
    );


    printf(
        "\n--- Simulation 2: Gun Jam ---\n"
    );


    sim2Iterations =
        runPathSimulation(
            &simulation2,
            path,
            pathCount,
            2,
            jamAfterIterations,
            jamAngle
        );


    savePart1BSummary(
        "part1b_sim2_summary.txt",
        &simulation2,
        sim2Iterations
    );


    printf(
        "\n====================================\n"
    );

    printf(
        "      PART 1-B COMPARISON\n"
    );

    printf(
        "====================================\n"
    );


    printf(
        "Simulation 1:\n"
    );

    printf(
        "Iterations completed: %d\n",
        sim1Iterations
    );

    printf(
        "Battleship: %s\n",
        simulation1.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );

    printf(
        "Escort ships remaining: %d\n\n",
        countAliveEscorts(
            &simulation1
        )
    );


    printf(
        "Simulation 2:\n"
    );

    printf(
        "Iterations completed: %d\n",
        sim2Iterations
    );

    printf(
        "Battleship: %s\n",
        simulation2.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );

    printf(
        "Escort ships remaining: %d\n",
        countAliveEscorts(
            &simulation2
        )
    );


    printf(
        "\nPart 1-B results saved to text files.\n"
    );
}

/* =========================================================
   PART 1-C - CUMULATIVE DAMAGE SYSTEM
   ========================================================= */

typedef struct
{
    int escortIndex;
    ShotSolution shot;

} EscortAttackEvent;


static void resetPart1CState(
    Battlefield *battlefield
)
{
    int i;

    battlefield->battleship.alive = 1;
    battlefield->battleship.health = 1.0;
    battlefield->battleship.cumulativeImpact = 0.0;

    for (i = 0; i < battlefield->escortCount; i++)
    {
        battlefield->escorts[i].alive = 1;
        battlefield->escorts[i].hasFired = 0;
    }
}


static void sortEscortAttackEvents(
    EscortAttackEvent events[],
    int count
)
{
    int i;
    int j;

    for (i = 0; i < count - 1; i++)
    {
        for (j = 0; j < count - i - 1; j++)
        {
            if (events[j].shot.time > events[j + 1].shot.time)
            {
                EscortAttackEvent temp = events[j];

                events[j] = events[j + 1];
                events[j + 1] = temp;
            }
        }
    }
}


static int processPart1CEscortAttacks(
    Battlefield *battlefield,
    FILE *file
)
{
    EscortAttackEvent events[MAX_ESCORTS];

    int eventCount = 0;
    int i;


    /*
       Find all living Escort Ships that have not
       fired before and can currently hit B.
    */

    for (i = 0; i < battlefield->escortCount; i++)
    {
        EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution shot;

        double distance;


        if (!escort->alive || escort->hasFired)
        {
            continue;
        }


        distance =
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
            events[eventCount].escortIndex = i;
            events[eventCount].shot = shot;

            eventCount++;
        }
    }


    /*
       Shells are processed according to their
       impact time.
    */

    sortEscortAttackEvents(
        events,
        eventCount
    );


    for (i = 0; i < eventCount; i++)
    {
        EscortShip *escort =
            &battlefield->escorts[
                events[i].escortIndex
            ];


        escort->hasFired = 1;


        battlefield->battleship.health -=
            escort->impactPower;


        battlefield->battleship.cumulativeImpact +=
            escort->impactPower;


        if (battlefield->battleship.health < 0.0)
        {
            battlefield->battleship.health = 0.0;
        }


        printf(
            "E%d [%s] hit B: %.0f%% damage\n",
            escort->id,
            escort->type,
            escort->impactPower * 100.0
        );


        fprintf(
            file,
            "E%d [%s] hit Battleship\n",
            escort->id,
            escort->type
        );

        fprintf(
            file,
            "Impact Power: %.2f\n",
            escort->impactPower
        );

        fprintf(
            file,
            "Impact Time: %.2f seconds\n",
            events[i].shot.time
        );

        fprintf(
            file,
            "Battleship Health: %.2f\n\n",
            battlefield->battleship.health
        );


        if (battlefield->battleship.health <= 0.0)
        {
            battlefield->battleship.alive = 0;

            printf(
                "Battleship destroyed by cumulative damage.\n"
            );

            fprintf(
                file,
                "Battleship destroyed by cumulative damage.\n"
            );

            return 1;
        }
    }


    return 0;
}


static int processPart1CBattleshipAttacks(
    Battlefield *battlefield,
    double minimumAngle,
    double maximumAngle,
    FILE *file
)
{
    int i;
    int destroyedCount = 0;


    if (!battlefield->battleship.alive)
    {
        return 0;
    }


    for (i = 0; i < battlefield->escortCount; i++)
    {
        EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution shot;

        double distance;


        if (!escort->alive)
        {
            continue;
        }


        distance =
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
                battlefield->battleship.maxVelocity,
                minimumAngle,
                maximumAngle,
                &shot
            )
        )
        {
            escort->alive = 0;

            destroyedCount++;


            printf(
                "Battleship destroyed E%d [%s]\n",
                escort->id,
                escort->type
            );


            fprintf(
                file,
                "Battleship destroyed E%d [%s]\n",
                escort->id,
                escort->type
            );

            fprintf(
                file,
                "Distance: %.2f\n",
                distance
            );

            fprintf(
                file,
                "Firing Velocity: %.2f\n",
                shot.velocity
            );

            fprintf(
                file,
                "Firing Angle: %.2f\n",
                shot.angle
            );

            fprintf(
                file,
                "Time to Hit: %.2f seconds\n\n",
                shot.time
            );
        }
    }


    return destroyedCount;
}


static void writePart1CState(
    FILE *file,
    const Battlefield *battlefield
)
{
    int i;


    fprintf(
        file,
        "\nBattleship Status: %s\n",
        battlefield->battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    fprintf(
        file,
        "Battleship Health: %.2f\n",
        battlefield->battleship.health
    );


    fprintf(
        file,
        "Battleship Health Percentage: %.2f%%\n",
        battlefield->battleship.health * 100.0
    );


    fprintf(
        file,
        "Cumulative Impact: %.2f\n\n",
        battlefield->battleship.cumulativeImpact
    );


    fprintf(
        file,
        "Escort Ship Status\n"
    );

    fprintf(
        file,
        "------------------\n"
    );


    for (i = 0; i < battlefield->escortCount; i++)
    {
        const EscortShip *escort =
            &battlefield->escorts[i];


        fprintf(
            file,
            "E%d [%s] : %s | Fired: %s\n",
            escort->id,
            escort->type,
            escort->alive
                ? "ALIVE"
                : "DESTROYED",
            escort->hasFired
                ? "YES"
                : "NO"
        );
    }
}


static int simulatePart1CIteration(
    Battlefield *battlefield,
    double minimumAngle,
    double maximumAngle,
    FILE *file
)
{
    int destroyedEscortCount;


    fprintf(
        file,
        "\nESCORT ATTACKS\n"
    );

    fprintf(
        file,
        "--------------\n"
    );


    if (
        processPart1CEscortAttacks(
            battlefield,
            file
        )
    )
    {
        writePart1CState(
            file,
            battlefield
        );

        return 1;
    }


    fprintf(
        file,
        "\nBATTLESHIP ATTACKS\n"
    );

    fprintf(
        file,
        "------------------\n"
    );


    destroyedEscortCount =
        processPart1CBattleshipAttacks(
            battlefield,
            minimumAngle,
            maximumAngle,
            file
        );


    fprintf(
        file,
        "\nEscort ships destroyed by B in this iteration: %d\n",
        destroyedEscortCount
    );


    writePart1CState(
        file,
        battlefield
    );


    return 0;
}


static void runPart1CStationary(
    const Battlefield *initialBattlefield
)
{
    Battlefield simulation;

    FILE *file;


    simulation =
        *initialBattlefield;


    resetPart1CState(
        &simulation
    );


    file = fopen(
        "part1c_stationary.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Could not create Part 1-C stationary result file.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 1-C STATIONARY SIMULATION\n"
    );

    fprintf(
        file,
        "==============================\n\n"
    );


    fprintf(
        file,
        "Initial Battleship Position: (%.2f, %.2f)\n",
        simulation.battleship.x,
        simulation.battleship.y
    );


    simulatePart1CIteration(
        &simulation,
        0.1,
        89.9,
        file
    );


    fclose(file);


    printf(
        "\nStationary Part 1-C Result\n"
    );

    printf(
        "Battleship: %s\n",
        simulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );

    printf(
        "Remaining Health: %.2f%%\n",
        simulation.battleship.health * 100.0
    );

    printf(
        "Cumulative Impact: %.2f\n",
        simulation.battleship.cumulativeImpact
    );

    printf(
        "Saved to part1c_stationary.txt\n"
    );
}


static int runPart1CPath(
    Battlefield *battlefield,
    const PathPoint path[],
    int pathCount,
    int simulationNumber,
    int jamAfterIterations,
    double jamAngle
)
{
    int i;
    int completedIterations = 0;


    for (
        i = 0;
        i < pathCount &&
        battlefield->battleship.alive;
        i++
    )
    {
        char filename[120];

        FILE *file;

        double minimumAngle = 0.1;

        int gunJammed = 0;


        if (
            simulationNumber == 2 &&
            i >= jamAfterIterations
        )
        {
            minimumAngle = jamAngle;
            gunJammed = 1;
        }


        battlefield->battleship.x =
            path[i].x;

        battlefield->battleship.y =
            path[i].y;


        snprintf(
            filename,
            sizeof(filename),
            "part1c_path_sim%d_iteration_%d.txt",
            simulationNumber,
            i + 1
        );


        file = fopen(
            filename,
            "w"
        );


        if (file == NULL)
        {
            printf(
                "Could not create %s\n",
                filename
            );

            continue;
        }


        fprintf(
            file,
            "PART 1-C PATH SIMULATION %d\n",
            simulationNumber
        );

        fprintf(
            file,
            "Iteration: %d\n",
            i + 1
        );

        fprintf(
            file,
            "===============================\n\n"
        );


        fprintf(
            file,
            "Battleship Position: (%.2f, %.2f)\n",
            path[i].x,
            path[i].y
        );


        if (simulationNumber == 2)
        {
            fprintf(
                file,
                "Gun Status: %s\n",
                gunJammed
                    ? "JAMMED"
                    : "NORMAL"
            );

            fprintf(
                file,
                "Battleship Angle Range: %.2f - 90.00\n",
                minimumAngle
            );
        }


        simulatePart1CIteration(
            battlefield,
            minimumAngle,
            89.9,
            file
        );


        fclose(file);

        completedIterations++;


        printf(
            "Part 1-C path iteration %d completed.\n",
            i + 1
        );
    }


    return completedIterations;
}


static void savePart1CPathSummary(
    const char *filename,
    const Battlefield *battlefield,
    int completedIterations
)
{
    FILE *file;


    file = fopen(
        filename,
        "w"
    );


    if (file == NULL)
    {
        return;
    }


    fprintf(
        file,
        "PART 1-C PATH SUMMARY\n"
    );

    fprintf(
        file,
        "=====================\n\n"
    );


    fprintf(
        file,
        "Completed Iterations: %d\n",
        completedIterations
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
        "Remaining Health: %.2f%%\n",
        battlefield->battleship.health * 100.0
    );


    fprintf(
        file,
        "Cumulative Impact: %.2f\n",
        battlefield->battleship.cumulativeImpact
    );


    fclose(file);
}


void runPart1C(
    const Battlefield *initialBattlefield
)
{
    Battlefield normalPathSimulation;
    Battlefield jamPathSimulation;

    PathPoint path[MAX_PATH_POINTS];

    int pathCount;
    int jamAfterIterations;
    int i;

    double jamAngle;

    unsigned int pathSeed;

    int normalIterations;
    int jamIterations;


    printf(
        "\n====================================\n"
    );

    printf(
        "      PART 1-C DAMAGE SIMULATION\n"
    );

    printf(
        "====================================\n"
    );


    /*
       Redo Part 1-A using cumulative damage.
    */

    runPart1CStationary(
        initialBattlefield
    );


    /*
       Redo Part 1-B using cumulative damage.
    */

    normalPathSimulation =
        *initialBattlefield;

    jamPathSimulation =
        *initialBattlefield;


    resetPart1CState(
        &normalPathSimulation
    );

    resetPart1CState(
        &jamPathSimulation
    );


    do
    {
        printf(
            "\nEnter number of path points k (2-%d): ",
            MAX_PATH_POINTS
        );

        scanf(
            "%d",
            &pathCount
        );

    }
    while (
        pathCount < 2 ||
        pathCount > MAX_PATH_POINTS
    );


    printf(
        "Enter path random seed: "
    );

    scanf(
        "%u",
        &pathSeed
    );


    srand(
        pathSeed
    );


    /*
       Same path is used for both simulations.
    */

    for (i = 0; i < pathCount; i++)
    {
        path[i].x =
            randomPathCoordinate(
                initialBattlefield->canvasSize
            );

        path[i].y =
            randomPathCoordinate(
                initialBattlefield->canvasSize
            );
    }


    do
    {
        printf(
            "Enter t for gun jam (1 to %d): ",
            pathCount - 1
        );

        scanf(
            "%d",
            &jamAfterIterations
        );

    }
    while (
        jamAfterIterations < 1 ||
        jamAfterIterations >= pathCount
    );


    do
    {
        printf(
            "Enter jammed minimum angle (0 < angle < 30): "
        );

        scanf(
            "%lf",
            &jamAngle
        );

    }
    while (
        jamAngle <= 0.0 ||
        jamAngle >= 30.0
    );


    printf(
        "\nPart 1-C Path Simulation 1 - Normal Gun\n"
    );


    normalIterations =
        runPart1CPath(
            &normalPathSimulation,
            path,
            pathCount,
            1,
            0,
            0.0
        );


    savePart1CPathSummary(
        "part1c_path_sim1_summary.txt",
        &normalPathSimulation,
        normalIterations
    );


    printf(
        "\nPart 1-C Path Simulation 2 - Jammed Gun\n"
    );


    jamIterations =
        runPart1CPath(
            &jamPathSimulation,
            path,
            pathCount,
            2,
            jamAfterIterations,
            jamAngle
        );


    savePart1CPathSummary(
        "part1c_path_sim2_summary.txt",
        &jamPathSimulation,
        jamIterations
    );


    printf(
        "\n========== PART 1-C COMPARISON ==========\n"
    );


    printf(
        "Normal Path Simulation:\n"
    );

    printf(
        "Status: %s\n",
        normalPathSimulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );

    printf(
        "Health: %.2f%%\n",
        normalPathSimulation.battleship.health * 100.0
    );

    printf(
        "Cumulative Impact: %.2f\n\n",
        normalPathSimulation.battleship.cumulativeImpact
    );


    printf(
        "Jammed Path Simulation:\n"
    );

    printf(
        "Status: %s\n",
        jamPathSimulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );

    printf(
        "Health: %.2f%%\n",
        jamPathSimulation.battleship.health * 100.0
    );

    printf(
        "Cumulative Impact: %.2f\n",
        jamPathSimulation.battleship.cumulativeImpact
    );


    printf(
        "\nPart 1-C results saved to text files.\n"
    );
}