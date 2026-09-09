#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "battle.h"
#include "strategy.h"


#define GRAVITY 9.81
#define PI 3.14159265358979323846

#define MAX_PATH_POINTS 50
#define PART2B_MAX_EVENTS 10000
#define PART2C_MAX_EVENTS 10000


/* =========================================================
   COMMON PROJECTILE FUNCTIONS
   ========================================================= */

double calculateDistance(
    double x1,
    double y1,
    double x2,
    double y2
)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(
        (dx * dx) +
        (dy * dy)
    );
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


        radians =
            angle * PI / 180.0;


        sinTwoTheta =
            sin(
                2.0 * radians
            );


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
                (
                    2.0 *
                    requiredVelocity *
                    sin(radians)
                ) /
                GRAVITY;


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


/* =========================================================
   PART 1-A FILE HANDLING
   ========================================================= */

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


/* =========================================================
   PART 1-A
   ========================================================= */

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


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
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


        return;
    }


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
}


/* =========================================================
   PART 1-B
   ========================================================= */

typedef struct
{
    double x;
    double y;

} PathPoint;


static double randomPathCoordinate(
    double maximum
)
{
    return
        ((double)rand() / RAND_MAX) *
        maximum;
}


static int countAliveEscorts(
    const Battlefield *battlefield
)
{
    int i;
    int count = 0;


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        if (
            battlefield->escorts[i].alive
        )
        {
            count++;
        }
    }


    return count;
}


static void resetPart1BState(
    Battlefield *battlefield
)
{
    int i;


    battlefield->battleship.alive = 1;


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        battlefield->escorts[i].alive = 1;

        battlefield->escorts[i].hasFired = 0;
    }
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
            "E%d [%s] - %s\n",
            escort->id,
            escort->type,
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


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
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


    if (sinkingEscortIndex != -1)
    {
        EscortShip *attacker =
            &battlefield
                ->escorts[sinkingEscortIndex];


        battlefield->battleship.alive =
            0;


        fprintf(
            file,
            "Battleship destroyed by E%d [%s]\n",
            attacker->id,
            attacker->type
        );


        return 1;
    }


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
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


            fprintf(
                file,
                "B destroyed E%d [%s]\n",
                escort->id,
                escort->type
            );
        }
    }


    fprintf(
        file,
        "Destroyed this iteration: %d\n",
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


        if (
            simulationNumber == 2 &&
            i >= jamAfterIterations
        )
        {
            minimumAngle =
                jamAngle;
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
            continue;
        }


        fprintf(
            file,
            "PART 1-B SIMULATION %d\n",
            simulationNumber
        );


        fprintf(
            file,
            "Iteration: %d\n\n",
            i + 1
        );


        if (simulationNumber == 2)
        {
            fprintf(
                file,
                "Gun Status: %s\n",
                i >= jamAfterIterations
                    ? "JAMMED"
                    : "NORMAL"
            );


            fprintf(
                file,
                "Battleship Firing Angle: %.2f - 90.00\n\n",
                minimumAngle
            );
        }


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


        fclose(file);


        completedIterations++;
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
        return;
    }


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
        countAliveEscorts(
            battlefield
        )
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


    simulation1 =
        *initialBattlefield;

    simulation2 =
        *initialBattlefield;


    resetPart1BState(
        &simulation1
    );

    resetPart1BState(
        &simulation2
    );


    printf(
        "\n========== PART 1-B ==========\n"
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


    for (
        i = 0;
        i < pathCount;
        i++
    )
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
            "Enter t for gun jam (1-%d): ",
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
            "Enter jammed minimum angle: "
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
        "Part 1-B completed.\n"
    );
}


/* =========================================================
   PART 1-C
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


    battlefield->battleship.alive =
        1;

    battlefield->battleship.health =
        1.0;

    battlefield->battleship.cumulativeImpact =
        0.0;


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        battlefield->escorts[i].alive =
            1;

        battlefield->escorts[i].hasFired =
            0;

        battlefield->escorts[i].health =
            1.0;
    }
}


static void sortEscortAttackEvents(
    EscortAttackEvent events[],
    int count
)
{
    int i;
    int j;


    for (
        i = 0;
        i < count - 1;
        i++
    )
    {
        for (
            j = 0;
            j < count - i - 1;
            j++
        )
        {
            if (
                events[j].shot.time >
                events[j + 1].shot.time
            )
            {
                EscortAttackEvent temp =
                    events[j];

                events[j] =
                    events[j + 1];

                events[j + 1] =
                    temp;
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


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution shot;

        double distance;


        if (
            !escort->alive ||
            escort->hasFired
        )
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
            events[eventCount].escortIndex =
                i;

            events[eventCount].shot =
                shot;

            eventCount++;
        }
    }


    sortEscortAttackEvents(
        events,
        eventCount
    );


    for (
        i = 0;
        i < eventCount;
        i++
    )
    {
        EscortShip *escort =
            &battlefield
                ->escorts[
                    events[i].escortIndex
                ];


        escort->hasFired =
            1;


        battlefield->battleship.health -=
            escort->impactPower;


        battlefield->battleship.cumulativeImpact +=
            escort->impactPower;


        if (
            battlefield->battleship.health <
            0.0
        )
        {
            battlefield->battleship.health =
                0.0;
        }


        fprintf(
            file,
            "E%d [%s] damage %.2f\n",
            escort->id,
            escort->type,
            escort->impactPower
        );


        if (
            battlefield->battleship.health <=
            0.0
        )
        {
            battlefield->battleship.alive =
                0;

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


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
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
            escort->alive =
                0;

            destroyedCount++;


            fprintf(
                file,
                "B destroyed E%d [%s]\n",
                escort->id,
                escort->type
            );
        }
    }


    return destroyedCount;
}


static void simulatePart1CIteration(
    Battlefield *battlefield,
    double minimumAngle,
    double maximumAngle,
    FILE *file
)
{
    if (
        processPart1CEscortAttacks(
            battlefield,
            file
        )
    )
    {
        return;
    }


    processPart1CBattleshipAttacks(
        battlefield,
        minimumAngle,
        maximumAngle,
        file
    );
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
        return;
    }


    simulatePart1CIteration(
        &simulation,
        0.1,
        89.9,
        file
    );


    fprintf(
        file,
        "\nBattleship Status: %s\n",
        simulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    fprintf(
        file,
        "Remaining Health: %.2f%%\n",
        simulation.battleship.health *
            100.0
    );


    fprintf(
        file,
        "Cumulative Impact: %.2f\n",
        simulation.battleship.cumulativeImpact
    );


    fclose(file);


    printf(
        "Stationary Part 1-C Health: %.2f%%\n",
        simulation.battleship.health *
            100.0
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

        double minimumAngle =
            0.1;


        if (
            simulationNumber == 2 &&
            i >= jamAfterIterations
        )
        {
            minimumAngle =
                jamAngle;
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
            continue;
        }


        simulatePart1CIteration(
            battlefield,
            minimumAngle,
            89.9,
            file
        );


        fprintf(
            file,
            "Health: %.2f%%\n",
            battlefield->battleship.health *
                100.0
        );


        fclose(file);


        completedIterations++;
    }


    return completedIterations;
}


void runPart1C(
    const Battlefield *initialBattlefield
)
{
    Battlefield normalSimulation;
    Battlefield jamSimulation;

    PathPoint path[MAX_PATH_POINTS];

    int pathCount;
    int jamAfterIterations;
    int i;

    double jamAngle;

    unsigned int seed;


    printf(
        "\n========== PART 1-C ==========\n"
    );


    runPart1CStationary(
        initialBattlefield
    );


    normalSimulation =
        *initialBattlefield;

    jamSimulation =
        *initialBattlefield;


    resetPart1CState(
        &normalSimulation
    );

    resetPart1CState(
        &jamSimulation
    );


    do
    {
        printf(
            "Enter path points k: "
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
        "Enter path seed: "
    );

    scanf(
        "%u",
        &seed
    );


    srand(
        seed
    );


    for (
        i = 0;
        i < pathCount;
        i++
    )
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
            "Enter gun jam iteration t: "
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
            "Enter jammed minimum angle: "
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


    runPart1CPath(
        &normalSimulation,
        path,
        pathCount,
        1,
        0,
        0.0
    );


    runPart1CPath(
        &jamSimulation,
        path,
        pathCount,
        2,
        jamAfterIterations,
        jamAngle
    );


    printf(
        "Part 1-C completed.\n"
    );
}


/* =========================================================
   PART 2-A
   ========================================================= */

static void resetPart2AState(
    Battlefield *battlefield
)
{
    int i;


    battlefield->battleship.alive =
        1;

    battlefield->battleship.health =
        1.0;

    battlefield->battleship.cumulativeImpact =
        0.0;

    battlefield->battleship.nextFireTime =
        0.0;

    battlefield->battleship.firingCount =
        0;

    battlefield->battleship.currentImpactPower =
        1.0;


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        battlefield->escorts[i].alive =
            1;

        battlefield->escorts[i].hasFired =
            0;

        battlefield->escorts[i].health =
            1.0;

        battlefield->escorts[i].firingCount =
            0;

        battlefield->escorts[i].nextFireTime =
            0.0;

        battlefield->escorts[i].currentImpactPower =
            battlefield->escorts[i].impactPower;
    }
}


static void processPart2AEscortAttacks(
    Battlefield *battlefield,
    double currentTime,
    FILE *file
)
{
    int i;


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution shot;

        double distance;


        if (
            !escort->alive ||
            escort->hasFired
        )
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
                shot.time <=
                currentTime
            )
            {
                escort->hasFired =
                    1;


                escort->firingCount++;


                battlefield->battleship.health -=
                    escort->impactPower;


                battlefield->battleship.cumulativeImpact +=
                    escort->impactPower;


                if (
                    battlefield->battleship.health <
                    0.0
                )
                {
                    battlefield->battleship.health =
                        0.0;
                }


                fprintf(
                    file,
                    "E%d hit B at %.2f seconds\n",
                    escort->id,
                    shot.time
                );


                if (
                    battlefield->battleship.health <=
                    0.0
                )
                {
                    battlefield->battleship.alive =
                        0;

                    return;
                }
            }
        }
    }
}


void runPart2A(
    const Battlefield *initialBattlefield
)
{
    Battlefield simulation;

    int attackOrder[MAX_ESCORTS];

    int targetCount;

    int i;

    double currentTime =
        0.0;

    FILE *file;


    simulation =
        *initialBattlefield;


    resetPart2AState(
        &simulation
    );


    printf(
        "\n========== PART 2-A ==========\n"
    );


    do
    {
        printf(
            "Enter Battleship firing interval TB: "
        );

        scanf(
            "%lf",
            &simulation.battleship.fireInterval
        );

    }
    while (
        simulation.battleship.fireInterval <=
        0.0
    );


    targetCount =
        createAttackOrder(
            &simulation,
            attackOrder
        );


    file = fopen(
        "part2a_result.txt",
        "w"
    );


    if (file == NULL)
    {
        return;
    }


    fprintf(
        file,
        "PART 2-A\n\n"
    );


    printf(
        "\nBattleship Attack Order\n"
    );


    for (
        i = 0;
        i < targetCount;
        i++
    )
    {
        EscortShip *escort =
            &simulation
                .escorts[
                    attackOrder[i]
                ];


        printf(
            "%d. E%d [%s] - Impact %.2f\n",
            i + 1,
            escort->id,
            escort->type,
            escort->impactPower
        );


        fprintf(
            file,
            "%d. E%d [%s]\n",
            i + 1,
            escort->id,
            escort->type
        );
    }


    for (
        i = 0;
        i < targetCount;
        i++
    )
    {
        EscortShip *target =
            &simulation
                .escorts[
                    attackOrder[i]
                ];

        ShotSolution shot;

        double distance;


        if (
            !simulation.battleship.alive
        )
        {
            break;
        }


        processPart2AEscortAttacks(
            &simulation,
            currentTime,
            file
        );


        if (
            !simulation.battleship.alive
        )
        {
            break;
        }


        if (!target->alive)
        {
            continue;
        }


        distance =
            calculateDistance(
                simulation.battleship.x,
                simulation.battleship.y,
                target->x,
                target->y
            );


        if (
            findShotSolution(
                distance,
                0.0,
                simulation.battleship.maxVelocity,
                0.1,
                89.9,
                &shot
            )
        )
        {
            target->alive =
                0;


            simulation.battleship.firingCount++;


            printf(
                "Time %.2f: B attacked E%d [%s]\n",
                currentTime,
                target->id,
                target->type
            );


            fprintf(
                file,
                "Time %.2f: B fired at E%d\n",
                currentTime,
                target->id
            );


            currentTime +=
                simulation.battleship.fireInterval;
        }
    }


    fprintf(
        file,
        "\nBattleship Status: %s\n",
        simulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    fprintf(
        file,
        "Health: %.2f%%\n",
        simulation.battleship.health *
            100.0
    );


    fclose(file);


    printf(
        "Part 2-A completed.\n"
    );
}


/* =========================================================
   PART 2-B
   CONTINUOUS ESCORT FIRING
   ========================================================= */

static double getEscortInterval(
    const EscortShip *escort,
    double eaInterval,
    double ebInterval,
    double ecInterval,
    double edInterval,
    double eeInterval
)
{
    if (
        strcmp(
            escort->type,
            "EA"
        ) == 0
    )
    {
        return eaInterval;
    }


    if (
        strcmp(
            escort->type,
            "EB"
        ) == 0
    )
    {
        return ebInterval;
    }


    if (
        strcmp(
            escort->type,
            "EC"
        ) == 0
    )
    {
        return ecInterval;
    }


    if (
        strcmp(
            escort->type,
            "ED"
        ) == 0
    )
    {
        return edInterval;
    }


    return eeInterval;
}


static void resetPart2BState(
    Battlefield *battlefield
)
{
    int i;


    battlefield->battleship.alive =
        1;

    battlefield->battleship.health =
        1.0;

    battlefield->battleship.cumulativeImpact =
        0.0;

    battlefield->battleship.nextFireTime =
        0.0;

    battlefield->battleship.firingCount =
        0;

    battlefield->battleship.currentImpactPower =
        1.0;


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        EscortShip *escort =
            &battlefield->escorts[i];


        escort->alive =
            1;

        escort->health =
            1.0;

        escort->hasFired =
            0;

        escort->nextFireTime =
            0.0;

        escort->firingCount =
            0;

        escort->currentImpactPower =
            escort->impactPower;
    }
}


static int allEscortsDestroyed(
    const Battlefield *battlefield
)
{
    int i;


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        if (
            battlefield->escorts[i].alive
        )
        {
            return 0;
        }
    }


    return 1;
}


static int selectPart2BTarget(
    const Battlefield *battlefield
)
{
    int order[MAX_ESCORTS];

    int count;

    int i;


    count =
        createAttackOrder(
            battlefield,
            order
        );


    for (
        i = 0;
        i < count;
        i++
    )
    {
        int index =
            order[i];

        const EscortShip *escort =
            &battlefield->escorts[index];

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
                0.1,
                89.9,
                &shot
            )
        )
        {
            return index;
        }
    }


    return -1;
}


void runPart2B(
    const Battlefield *initialBattlefield
)
{
    Battlefield simulation;

    FILE *file;

    double eaInterval;
    double ebInterval;
    double ecInterval;
    double edInterval;
    double eeInterval;

    double currentTime =
        0.0;

    int eventCount =
        0;

    int i;


    simulation =
        *initialBattlefield;


    resetPart2BState(
        &simulation
    );


    printf(
        "\n====================================\n"
    );

    printf(
        "            PART 2-B\n"
    );

    printf(
        "====================================\n"
    );


    do
    {
        printf(
            "Enter Battleship firing interval TB: "
        );

        scanf(
            "%lf",
            &simulation.battleship.fireInterval
        );

    }
    while (
        simulation.battleship.fireInterval <=
        0.0
    );


    do
    {
        printf(
            "Enter EA firing interval TE: "
        );

        scanf(
            "%lf",
            &eaInterval
        );

    }
    while (
        eaInterval <= 0.0
    );


    do
    {
        printf(
            "Enter EB firing interval TE: "
        );

        scanf(
            "%lf",
            &ebInterval
        );

    }
    while (
        ebInterval <= 0.0
    );


    do
    {
        printf(
            "Enter EC firing interval TE: "
        );

        scanf(
            "%lf",
            &ecInterval
        );

    }
    while (
        ecInterval <= 0.0
    );


    do
    {
        printf(
            "Enter ED firing interval TE: "
        );

        scanf(
            "%lf",
            &edInterval
        );

    }
    while (
        edInterval <= 0.0
    );


    do
    {
        printf(
            "Enter EE firing interval TE: "
        );

        scanf(
            "%lf",
            &eeInterval
        );

    }
    while (
        eeInterval <= 0.0
    );


    for (
        i = 0;
        i < simulation.escortCount;
        i++
    )
    {
        simulation
            .escorts[i]
            .fireInterval =
            getEscortInterval(
                &simulation.escorts[i],
                eaInterval,
                ebInterval,
                ecInterval,
                edInterval,
                eeInterval
            );


        simulation
            .escorts[i]
            .nextFireTime =
            0.0;
    }


    file = fopen(
        "part2b_result.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Could not create Part 2-B result file.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 2-B SIMULATION\n"
    );

    fprintf(
        file,
        "===================\n\n"
    );


    fprintf(
        file,
        "Battleship TB: %.2f seconds\n",
        simulation.battleship.fireInterval
    );

    fprintf(
        file,
        "EA TE: %.2f\n",
        eaInterval
    );

    fprintf(
        file,
        "EB TE: %.2f\n",
        ebInterval
    );

    fprintf(
        file,
        "EC TE: %.2f\n",
        ecInterval
    );

    fprintf(
        file,
        "ED TE: %.2f\n",
        edInterval
    );

    fprintf(
        file,
        "EE TE: %.2f\n\n",
        eeInterval
    );


    fprintf(
        file,
        "BATTLE EVENTS\n"
    );

    fprintf(
        file,
        "-------------\n"
    );


    while (
        simulation.battleship.alive &&
        !allEscortsDestroyed(
            &simulation
        ) &&
        eventCount <
            PART2B_MAX_EVENTS
    )
    {
        double nextEventTime =
            simulation.battleship.nextFireTime;


        for (
            i = 0;
            i < simulation.escortCount;
            i++
        )
        {
            EscortShip *escort =
                &simulation.escorts[i];


            if (!escort->alive)
            {
                continue;
            }


            if (
                escort->nextFireTime <
                nextEventTime
            )
            {
                nextEventTime =
                    escort->nextFireTime;
            }
        }


        currentTime =
            nextEventTime;


        /* =========================
           ESCORT FIRING EVENTS
           ========================= */

        for (
            i = 0;
            i < simulation.escortCount;
            i++
        )
        {
            EscortShip *escort =
                &simulation.escorts[i];

            ShotSolution shot;

            double distance;


            if (!escort->alive)
            {
                continue;
            }


            if (
                fabs(
                    escort->nextFireTime -
                    currentTime
                ) >
                0.000001
            )
            {
                continue;
            }


            distance =
                calculateDistance(
                    escort->x,
                    escort->y,
                    simulation.battleship.x,
                    simulation.battleship.y
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
                double impactTime =
                    currentTime +
                    shot.time;


                escort->firingCount++;


                simulation.battleship.health -=
                    escort->impactPower;


                simulation.battleship.cumulativeImpact +=
                    escort->impactPower;


                if (
                    simulation.battleship.health <
                    0.0
                )
                {
                    simulation.battleship.health =
                        0.0;
                }


                printf(
                    "Time %.2f: E%d [%s] fired at B\n",
                    currentTime,
                    escort->id,
                    escort->type
                );


                fprintf(
                    file,
                    "Time %.2f: E%d [%s] fired at B\n",
                    currentTime,
                    escort->id,
                    escort->type
                );


                fprintf(
                    file,
                    "  Shell Impact Time: %.2f\n",
                    impactTime
                );


                fprintf(
                    file,
                    "  Damage: %.2f\n",
                    escort->impactPower
                );


                fprintf(
                    file,
                    "  B Health: %.2f%%\n",
                    simulation.battleship.health *
                        100.0
                );


                if (
                    simulation.battleship.health <=
                    0.0
                )
                {
                    simulation.battleship.alive =
                        0;


                    fprintf(
                        file,
                        "Battleship destroyed.\n"
                    );


                    break;
                }
            }


            /*
               Escort can fire again after TE.
            */

            escort->nextFireTime +=
                escort->fireInterval;
        }


        if (
            !simulation.battleship.alive
        )
        {
            break;
        }


        /* =========================
           BATTLESHIP FIRING EVENT
           ========================= */

        if (
            fabs(
                simulation.battleship.nextFireTime -
                currentTime
            ) <=
            0.000001
        )
        {
            int targetIndex;


            targetIndex =
                selectPart2BTarget(
                    &simulation
                );


            if (
                targetIndex != -1
            )
            {
                EscortShip *target =
                    &simulation
                        .escorts[
                            targetIndex
                        ];

                ShotSolution shot;

                double distance;


                distance =
                    calculateDistance(
                        simulation.battleship.x,
                        simulation.battleship.y,
                        target->x,
                        target->y
                    );


                if (
                    findShotSolution(
                        distance,
                        0.0,
                        simulation.battleship.maxVelocity,
                        0.1,
                        89.9,
                        &shot
                    )
                )
                {
                    double impactTime =
                        currentTime +
                        shot.time;


                    simulation
                        .battleship
                        .firingCount++;


                    target->alive =
                        0;


                    printf(
                        "Time %.2f: B fired at E%d [%s]\n",
                        currentTime,
                        target->id,
                        target->type
                    );


                    fprintf(
                        file,
                        "Time %.2f: B fired at E%d [%s]\n",
                        currentTime,
                        target->id,
                        target->type
                    );


                    fprintf(
                        file,
                        "  Shell Impact Time: %.2f\n",
                        impactTime
                    );


                    fprintf(
                        file,
                        "  Target destroyed.\n"
                    );
                }
            }


            simulation.battleship.nextFireTime +=
                simulation.battleship.fireInterval;
        }


        eventCount++;
    }


    fprintf(
        file,
        "\nFINAL RESULT\n"
    );

    fprintf(
        file,
        "------------\n"
    );


    fprintf(
        file,
        "Battleship Status: %s\n",
        simulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    fprintf(
        file,
        "Battleship Health: %.2f%%\n",
        simulation.battleship.health *
            100.0
    );


    fprintf(
        file,
        "Cumulative Impact: %.2f\n",
        simulation.battleship.cumulativeImpact
    );


    fprintf(
        file,
        "Battleship Shots Fired: %d\n",
        simulation.battleship.firingCount
    );


    fprintf(
        file,
        "Simulation End Time: %.2f seconds\n",
        currentTime
    );


    fprintf(
        file,
        "\nEscort Firing Counts\n"
    );


    for (
        i = 0;
        i < simulation.escortCount;
        i++
    )
    {
        fprintf(
            file,
            "E%d [%s] - Shots: %d - Status: %s\n",
            simulation.escorts[i].id,
            simulation.escorts[i].type,
            simulation.escorts[i].firingCount,
            simulation.escorts[i].alive
                ? "ALIVE"
                : "DESTROYED"
        );
    }


    fclose(file);


    printf(
        "\nPart 2-B completed.\n"
    );


    printf(
        "Battleship Status: %s\n",
        simulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    printf(
        "Remaining Health: %.2f%%\n",
        simulation.battleship.health *
            100.0
    );


    printf(
        "Result saved to part2b_result.txt\n"
    );
}

/* =========================================================
   PART 2-C
   IMPACT POWER DEGRADATION
   ========================================================= */


static int sameGamma(
    double a,
    double b
)
{
    return fabs(a - b) < 0.000001;
}


static double getEscortGamma(
    const EscortShip *escort,
    double eaGamma,
    double ebGamma,
    double ecGamma,
    double edGamma,
    double eeGamma
)
{
    if (strcmp(escort->type, "EA") == 0)
    {
        return eaGamma;
    }

    if (strcmp(escort->type, "EB") == 0)
    {
        return ebGamma;
    }

    if (strcmp(escort->type, "EC") == 0)
    {
        return ecGamma;
    }

    if (strcmp(escort->type, "ED") == 0)
    {
        return edGamma;
    }

    return eeGamma;
}


static void resetPart2CState(
    Battlefield *battlefield
)
{
    int i;


    /* Battleship */

    battlefield->battleship.alive = 1;

    battlefield->battleship.health = 1.0;

    battlefield->battleship.cumulativeImpact = 0.0;

    battlefield->battleship.nextFireTime = 0.0;

    battlefield->battleship.firingCount = 0;

    battlefield->battleship.initialImpactPower = 1.0;

    battlefield->battleship.currentImpactPower = 1.0;


    /* Escort Ships */

    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        EscortShip *escort =
            &battlefield->escorts[i];


        escort->alive = 1;

        escort->health = 1.0;

        escort->hasFired = 0;

        escort->nextFireTime = 0.0;

        escort->firingCount = 0;

        escort->currentImpactPower =
            escort->impactPower;
    }
}


/* =========================================================
   PART 2-C SIMULATION
   ========================================================= */

void runPart2C(
    const Battlefield *initialBattlefield
)
{
    Battlefield simulation;

    FILE *file;

    double eaInterval;
    double ebInterval;
    double ecInterval;
    double edInterval;
    double eeInterval;

    double battleshipGamma;

    double eaGamma;
    double ebGamma;
    double ecGamma;
    double edGamma;
    double eeGamma;

    double currentTime = 0.0;

    int eventCount = 0;

    int i;


    simulation =
        *initialBattlefield;


    resetPart2CState(
        &simulation
    );


    printf(
        "\n====================================\n"
    );

    printf(
        "            PART 2-C\n"
    );

    printf(
        "     IMPACT POWER DEGRADATION\n"
    );

    printf(
        "====================================\n"
    );


    /* =====================================================
       FIRING INTERVALS
       ===================================================== */

    do
    {
        printf(
            "Enter Battleship firing interval TB: "
        );

        scanf(
            "%lf",
            &simulation.battleship.fireInterval
        );

    }
    while (
        simulation.battleship.fireInterval <= 0.0
    );


    do
    {
        printf(
            "Enter EA firing interval TE: "
        );

        scanf(
            "%lf",
            &eaInterval
        );

    }
    while (eaInterval <= 0.0);


    do
    {
        printf(
            "Enter EB firing interval TE: "
        );

        scanf(
            "%lf",
            &ebInterval
        );

    }
    while (ebInterval <= 0.0);


    do
    {
        printf(
            "Enter EC firing interval TE: "
        );

        scanf(
            "%lf",
            &ecInterval
        );

    }
    while (ecInterval <= 0.0);


    do
    {
        printf(
            "Enter ED firing interval TE: "
        );

        scanf(
            "%lf",
            &edInterval
        );

    }
    while (edInterval <= 0.0);


    do
    {
        printf(
            "Enter EE firing interval TE: "
        );

        scanf(
            "%lf",
            &eeInterval
        );

    }
    while (eeInterval <= 0.0);


    /* =====================================================
       GAMMA INPUTS
       ===================================================== */

    printf(
        "\n--- Gamma Values ---\n"
    );


    do
    {
        printf(
            "Enter gamma for Battleship %c: ",
            simulation.battleship.type
        );

        scanf(
            "%lf",
            &battleshipGamma
        );

    }
    while (
        battleshipGamma <= 0.0
    );


    /*
       Each Escort type uses a unique gamma value.
    */

    do
    {
        printf(
            "Enter gamma for EA: "
        );

        scanf(
            "%lf",
            &eaGamma
        );

    }
    while (
        eaGamma <= 0.0
    );


    do
    {
        printf(
            "Enter gamma for EB (different from EA): "
        );

        scanf(
            "%lf",
            &ebGamma
        );

    }
    while (
        ebGamma <= 0.0 ||
        sameGamma(
            ebGamma,
            eaGamma
        )
    );


    do
    {
        printf(
            "Enter gamma for EC: "
        );

        scanf(
            "%lf",
            &ecGamma
        );

    }
    while (
        ecGamma <= 0.0 ||
        sameGamma(ecGamma, eaGamma) ||
        sameGamma(ecGamma, ebGamma)
    );


    do
    {
        printf(
            "Enter gamma for ED: "
        );

        scanf(
            "%lf",
            &edGamma
        );

    }
    while (
        edGamma <= 0.0 ||
        sameGamma(edGamma, eaGamma) ||
        sameGamma(edGamma, ebGamma) ||
        sameGamma(edGamma, ecGamma)
    );


    do
    {
        printf(
            "Enter gamma for EE: "
        );

        scanf(
            "%lf",
            &eeGamma
        );

    }
    while (
        eeGamma <= 0.0 ||
        sameGamma(eeGamma, eaGamma) ||
        sameGamma(eeGamma, ebGamma) ||
        sameGamma(eeGamma, ecGamma) ||
        sameGamma(eeGamma, edGamma)
    );


    simulation.battleship.gamma =
        battleshipGamma;


    /* =====================================================
       ASSIGN E INTERVALS AND GAMMA VALUES
       ===================================================== */

    for (
        i = 0;
        i < simulation.escortCount;
        i++
    )
    {
        EscortShip *escort =
            &simulation.escorts[i];


        escort->fireInterval =
            getEscortInterval(
                escort,
                eaInterval,
                ebInterval,
                ecInterval,
                edInterval,
                eeInterval
            );


        escort->gamma =
            getEscortGamma(
                escort,
                eaGamma,
                ebGamma,
                ecGamma,
                edGamma,
                eeGamma
            );


        escort->nextFireTime =
            0.0;
    }


    /* =====================================================
       OUTPUT FILE
       ===================================================== */

    file = fopen(
        "part2c_result.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Could not create Part 2-C result file.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 2-C IMPACT POWER DEGRADATION\n"
    );

    fprintf(
        file,
        "=================================\n\n"
    );


    fprintf(
        file,
        "Battleship Type: %c\n",
        simulation.battleship.type
    );


    fprintf(
        file,
        "Battleship TB: %.2f\n",
        simulation.battleship.fireInterval
    );


    fprintf(
        file,
        "Battleship Gamma: %.4f\n",
        simulation.battleship.gamma
    );


    fprintf(
        file,
        "Battleship Initial Impact Power: %.2f\n\n",
        simulation.battleship.initialImpactPower
    );


    fprintf(
        file,
        "Escort Gamma Values\n"
    );

    fprintf(
        file,
        "EA: %.4f\n",
        eaGamma
    );

    fprintf(
        file,
        "EB: %.4f\n",
        ebGamma
    );

    fprintf(
        file,
        "EC: %.4f\n",
        ecGamma
    );

    fprintf(
        file,
        "ED: %.4f\n",
        edGamma
    );

    fprintf(
        file,
        "EE: %.4f\n\n",
        eeGamma
    );


    fprintf(
        file,
        "BATTLE EVENTS\n"
    );

    fprintf(
        file,
        "-------------\n"
    );


    /* =====================================================
       EVENT SIMULATION
       ===================================================== */

    while (
        simulation.battleship.alive &&
        !allEscortsDestroyed(
            &simulation
        ) &&
        eventCount <
            PART2C_MAX_EVENTS
    )
    {
        double nextEventTime =
            simulation.battleship.nextFireTime;


        /*
           Find earliest Escort firing event.
        */

        for (
            i = 0;
            i < simulation.escortCount;
            i++
        )
        {
            EscortShip *escort =
                &simulation.escorts[i];


            if (!escort->alive)
            {
                continue;
            }


            if (
                escort->nextFireTime <
                nextEventTime
            )
            {
                nextEventTime =
                    escort->nextFireTime;
            }
        }


        currentTime =
            nextEventTime;


        /* =================================================
           ESCORT FIRING EVENTS
           ================================================= */

        for (
            i = 0;
            i < simulation.escortCount;
            i++
        )
        {
            EscortShip *escort =
                &simulation.escorts[i];

            ShotSolution shot;

            double distance;


            if (!escort->alive)
            {
                continue;
            }


            if (
                fabs(
                    escort->nextFireTime -
                    currentTime
                ) >
                0.000001
            )
            {
                continue;
            }


            distance =
                calculateDistance(
                    escort->x,
                    escort->y,
                    simulation.battleship.x,
                    simulation.battleship.y
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
                double impactTime;

                double currentImpact;


                /*
                   IP_n = IP_0 * exp(-gamma * n)

                   First firing:
                   n = 0
                   Therefore initial impact power is used.
                */

                currentImpact =
                    escort->impactPower *
                    exp(
                        -escort->gamma *
                        escort->firingCount
                    );


                escort->currentImpactPower =
                    currentImpact;


                impactTime =
                    currentTime +
                    shot.time;


                escort->firingCount++;


                simulation.battleship.health -=
                    currentImpact;


                simulation.battleship.cumulativeImpact +=
                    currentImpact;


                if (
                    simulation.battleship.health <
                    0.0
                )
                {
                    simulation.battleship.health =
                        0.0;
                }


                printf(
                    "Time %.2f: E%d [%s] -> B | IP %.4f | B Health %.2f%%\n",
                    currentTime,
                    escort->id,
                    escort->type,
                    currentImpact,
                    simulation.battleship.health *
                    100.0
                );


                fprintf(
                    file,
                    "\nTime %.2f: E%d [%s] fired at B\n",
                    currentTime,
                    escort->id,
                    escort->type
                );


                fprintf(
                    file,
                    "Firing Number n: %d\n",
                    escort->firingCount - 1
                );


                fprintf(
                    file,
                    "Gamma: %.4f\n",
                    escort->gamma
                );


                fprintf(
                    file,
                    "Current Impact Power: %.4f\n",
                    currentImpact
                );


                fprintf(
                    file,
                    "Shell Impact Time: %.2f\n",
                    impactTime
                );


                fprintf(
                    file,
                    "Battleship Health: %.2f%%\n",
                    simulation.battleship.health *
                    100.0
                );


                if (
                    simulation.battleship.health <=
                    0.0
                )
                {
                    simulation.battleship.alive =
                        0;


                    fprintf(
                        file,
                        "Battleship DESTROYED.\n"
                    );


                    break;
                }
            }


            escort->nextFireTime +=
                escort->fireInterval;
        }


        if (
            !simulation.battleship.alive
        )
        {
            break;
        }


        /* =================================================
           BATTLESHIP FIRING EVENT
           ================================================= */

        if (
            fabs(
                simulation.battleship.nextFireTime -
                currentTime
            ) <=
            0.000001
        )
        {
            int targetIndex;


            targetIndex =
                selectPart2BTarget(
                    &simulation
                );


            if (
                targetIndex != -1
            )
            {
                EscortShip *target =
                    &simulation
                        .escorts[
                            targetIndex
                        ];

                ShotSolution shot;

                double distance;


                distance =
                    calculateDistance(
                        simulation.battleship.x,
                        simulation.battleship.y,
                        target->x,
                        target->y
                    );


                if (
                    findShotSolution(
                        distance,
                        0.0,
                        simulation.battleship.maxVelocity,
                        0.1,
                        89.9,
                        &shot
                    )
                )
                {
                    double currentImpact;

                    double impactTime;


                    currentImpact =
                        simulation
                            .battleship
                            .initialImpactPower *
                        exp(
                            -simulation
                                .battleship
                                .gamma *
                            simulation
                                .battleship
                                .firingCount
                        );


                    simulation
                        .battleship
                        .currentImpactPower =
                        currentImpact;


                    impactTime =
                        currentTime +
                        shot.time;


                    simulation
                        .battleship
                        .firingCount++;


                    target->health -=
                        currentImpact;


                    if (
                        target->health <
                        0.0
                    )
                    {
                        target->health =
                            0.0;
                    }


                    printf(
                        "Time %.2f: B -> E%d [%s] | IP %.4f | E Health %.2f%%\n",
                        currentTime,
                        target->id,
                        target->type,
                        currentImpact,
                        target->health *
                        100.0
                    );


                    fprintf(
                        file,
                        "\nTime %.2f: Battleship fired at E%d [%s]\n",
                        currentTime,
                        target->id,
                        target->type
                    );


                    fprintf(
                        file,
                        "Firing Number n: %d\n",
                        simulation
                            .battleship
                            .firingCount -
                        1
                    );


                    fprintf(
                        file,
                        "Gamma: %.4f\n",
                        simulation
                            .battleship
                            .gamma
                    );


                    fprintf(
                        file,
                        "Current Impact Power: %.4f\n",
                        currentImpact
                    );


                    fprintf(
                        file,
                        "Shell Impact Time: %.2f\n",
                        impactTime
                    );


                    fprintf(
                        file,
                        "E%d Health: %.2f%%\n",
                        target->id,
                        target->health *
                        100.0
                    );


                    /*
                       Unlike previous stages,
                       an Escort is only destroyed
                       when its health reaches zero.
                    */

                    if (
                        target->health <=
                        0.0
                    )
                    {
                        target->alive =
                            0;


                        printf(
                            "E%d [%s] DESTROYED\n",
                            target->id,
                            target->type
                        );


                        fprintf(
                            file,
                            "E%d DESTROYED.\n",
                            target->id
                        );
                    }
                    else
                    {
                        fprintf(
                            file,
                            "E%d survived this hit.\n",
                            target->id
                        );
                    }
                }
            }


            simulation.battleship.nextFireTime +=
                simulation.battleship.fireInterval;
        }


        eventCount++;
    }


    /* =====================================================
       FINAL RESULTS
       ===================================================== */

    fprintf(
        file,
        "\n\nFINAL RESULT\n"
    );

    fprintf(
        file,
        "============\n"
    );


    fprintf(
        file,
        "Battleship Status: %s\n",
        simulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    fprintf(
        file,
        "Battleship Health: %.2f%%\n",
        simulation.battleship.health *
        100.0
    );


    fprintf(
        file,
        "Battleship Gamma: %.4f\n",
        simulation.battleship.gamma
    );


    fprintf(
        file,
        "Battleship Shots Fired: %d\n",
        simulation.battleship.firingCount
    );


    fprintf(
        file,
        "Battleship Current Impact Power: %.4f\n",
        simulation.battleship.currentImpactPower
    );


    fprintf(
        file,
        "Cumulative Escort Impact on B: %.4f\n",
        simulation.battleship.cumulativeImpact
    );


    fprintf(
        file,
        "Simulation Time: %.2f seconds\n",
        currentTime
    );


    fprintf(
        file,
        "\nFINAL ESCORT CONDITIONS\n"
    );

    fprintf(
        file,
        "-----------------------\n"
    );


    for (
        i = 0;
        i < simulation.escortCount;
        i++
    )
    {
        EscortShip *escort =
            &simulation.escorts[i];


        fprintf(
            file,
            "\nE%d [%s]\n",
            escort->id,
            escort->type
        );


        fprintf(
            file,
            "Status: %s\n",
            escort->alive
                ? "ALIVE"
                : "DESTROYED"
        );


        fprintf(
            file,
            "Health: %.2f%%\n",
            escort->health *
            100.0
        );


        fprintf(
            file,
            "Gamma: %.4f\n",
            escort->gamma
        );


        fprintf(
            file,
            "Shots Fired: %d\n",
            escort->firingCount
        );


        fprintf(
            file,
            "Current Impact Power: %.4f\n",
            escort->currentImpactPower
        );
    }


    fclose(file);


    printf(
        "\n====================================\n"
    );

    printf(
        "Part 2-C completed.\n"
    );


    printf(
        "Battleship Status: %s\n",
        simulation.battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    printf(
        "Battleship Health: %.2f%%\n",
        simulation.battleship.health *
        100.0
    );


    printf(
        "Battleship Shots Fired: %d\n",
        simulation.battleship.firingCount
    );


    printf(
        "Final B Impact Power: %.4f\n",
        simulation.battleship.currentImpactPower
    );


    printf(
        "Results saved to part2c_result.txt\n"
    );
}