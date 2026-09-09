#include <stdio.h>

#include "fileio.h"


/* =========================================================
   PRINT A SAVED TEXT FILE
   ========================================================= */

static void displayTextFile(
    const char *filename
)
{
    FILE *file;

    char line[256];


    file = fopen(
        filename,
        "r"
    );


    if (file == NULL)
    {
        printf(
            "\nNo saved result found for: %s\n",
            filename
        );

        return;
    }


    printf(
        "\n====================================\n"
    );

    printf(
        "FILE: %s\n",
        filename
    );

    printf(
        "====================================\n\n"
    );


    while (
        fgets(
            line,
            sizeof(line),
            file
        ) != NULL
    )
    {
        printf(
            "%s",
            line
        );
    }


    fclose(file);


    printf(
        "\n====================================\n"
    );
}


/* =========================================================
   SAVE CURRENT SETUP
   ========================================================= */

void saveSetupSnapshot(
    const Battlefield *battlefield
)
{
    FILE *file;

    int i;


    file = fopen(
        "setup_state.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Warning: setup_state.txt could not be created.\n"
        );

        return;
    }


    fprintf(
        file,
        "ADVANCED NAVAL BATTLE SIMULATOR\n"
    );

    fprintf(
        file,
        "CURRENT SETUP\n"
    );

    fprintf(
        file,
        "====================================\n\n"
    );


    fprintf(
        file,
        "Canvas Size: %.2f\n",
        battlefield->canvasSize
    );


    fprintf(
        file,
        "Number of Escort Ships: %d\n\n",
        battlefield->escortCount
    );


    fprintf(
        file,
        "BATTLESHIP\n"
    );

    fprintf(
        file,
        "----------\n"
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
        "Health: %.2f%%\n",
        battlefield->battleship.health *
        100.0
    );


    fprintf(
        file,
        "Initial Impact Power: %.4f\n",
        battlefield->battleship.initialImpactPower
    );


    fprintf(
        file,
        "Gamma: %.4f\n\n",
        battlefield->battleship.gamma
    );


    fprintf(
        file,
        "ESCORT SHIPS\n"
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
            "\nE%d [%s]\n",
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
            "Gamma: %.4f\n",
            escort->gamma
        );
    }


    fclose(file);
}


/* =========================================================
   INSTRUCTIONS
   ========================================================= */

void showInstructions(void)
{
    printf(
        "\n====================================\n"
    );

    printf(
        "             INSTRUCTIONS\n"
    );

    printf(
        "====================================\n"
    );


    printf(
        "\nOBJECTIVE\n"
    );

    printf(
        "Simulate battles between one Battleship B and "
        "multiple Escort Ships E.\n"
    );


    printf(
        "\nBATTLEFIELD SETUP\n"
    );

    printf(
        "- Enter the square battlefield size D.\n"
    );

    printf(
        "- Enter the number of Escort Ships.\n"
    );

    printf(
        "- Select Battleship type U, M, R or S.\n"
    );

    printf(
        "- Enter Battleship position and maximum shell velocity.\n"
    );

    printf(
        "- Escort Ships are generated using the random seed.\n"
    );


    printf(
        "\nSIMULATION PARTS\n"
    );

    printf(
        "Part 1-A:\n"
        "Basic stationary battle with zero Battleship reload time.\n\n"
    );


    printf(
        "Part 1-B:\n"
        "Battleship follows a path. A second simulation includes "
        "a gun jam.\n\n"
    );


    printf(
        "Part 1-C:\n"
        "Escort damage becomes cumulative instead of one-hit "
        "Battleship destruction.\n\n"
    );


    printf(
        "Part 2-A:\n"
        "Battleship uses firing interval TB and a target priority "
        "strategy.\n\n"
    );


    printf(
        "Part 2-B:\n"
        "Escort Ships can fire repeatedly using their TE firing "
        "intervals.\n\n"
    );


    printf(
        "Part 2-C:\n"
        "Impact power decreases after repeated gun firings using "
        "IPn = IP0 * exp(-gamma * n).\n"
    );


    printf(
        "\nRESULT FILES\n"
    );

    printf(
        "Each simulation saves relevant results as text files.\n"
    );

    printf(
        "Use Simulation Statistics to view previously saved results.\n"
    );


    printf(
        "\n====================================\n"
    );
}


/* =========================================================
   PREVIOUS SIMULATION STATISTICS
   ========================================================= */

void showSimulationStatistics(void)
{
    int choice;


    do
    {
        printf(
            "\n====================================\n"
        );

        printf(
            "       SIMULATION STATISTICS\n"
        );

        printf(
            "====================================\n"
        );


        printf(
            "1. View Last Battlefield Setup\n"
        );

        printf(
            "2. View Part 1-A Initial Conditions\n"
        );

        printf(
            "3. View Part 1-A Hit Details\n"
        );

        printf(
            "4. View Part 1-A Final Conditions\n"
        );

        printf(
            "5. View Part 1-B Simulation 1 Summary\n"
        );

        printf(
            "6. View Part 1-B Simulation 2 Summary\n"
        );

        printf(
            "7. View Part 1-C Stationary Result\n"
        );

        printf(
            "8. View Part 2-A Result\n"
        );

        printf(
            "9. View Part 2-B Result\n"
        );

        printf(
            "10. View Part 2-C Result\n"
        );

        printf(
            "11. Back\n"
        );


        printf(
            "Enter option: "
        );


        scanf(
            "%d",
            &choice
        );


        switch (choice)
        {
            case 1:

                displayTextFile(
                    "setup_state.txt"
                );

                break;


            case 2:

                displayTextFile(
                    "part1a_initial.txt"
                );

                break;


            case 3:

                displayTextFile(
                    "part1a_hits.txt"
                );

                break;


            case 4:

                displayTextFile(
                    "part1a_final.txt"
                );

                break;


            case 5:

                displayTextFile(
                    "part1b_sim1_summary.txt"
                );

                break;


            case 6:

                displayTextFile(
                    "part1b_sim2_summary.txt"
                );

                break;


            case 7:

                displayTextFile(
                    "part1c_stationary.txt"
                );

                break;


            case 8:

                displayTextFile(
                    "part2a_result.txt"
                );

                break;


            case 9:

                displayTextFile(
                    "part2b_result.txt"
                );

                break;


            case 10:

                displayTextFile(
                    "part2c_result.txt"
                );

                break;


            case 11:

                break;


            default:

                printf(
                    "\nInvalid statistics option.\n"
                );
        }

    }
    while (
        choice != 11
    );
}