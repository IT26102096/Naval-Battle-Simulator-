#include <stdio.h>

#include "ship.h"
#include "battle.h"
#include "fileio.h"


/* =========================================================
   START SIMULATION SUBMENU
   ========================================================= */

static void startSimulationMenu(
    Battlefield *battlefield,
    int battlefieldReady
)
{
    int choice;


    do
    {
        printf(
            "\n====================================\n"
        );

        printf(
            "        START SIMULATION\n"
        );

        printf(
            "====================================\n"
        );


        printf(
            "1. Run Part 1-A Simulation\n"
        );

        printf(
            "2. Run Part 1-B Simulations\n"
        );

        printf(
            "3. Run Part 1-C Cumulative Damage Simulation\n"
        );

        printf(
            "4. Run Part 2-A Simulation\n"
        );

        printf(
            "5. Run Part 2-B Continuous Firing Simulation\n"
        );

        printf(
            "6. Run Part 2-C Impact Degradation Simulation\n"
        );

        printf(
            "7. Back to Main Menu\n"
        );


        printf(
            "Enter option: "
        );


        scanf(
            "%d",
            &choice
        );


        if (
            choice >= 1 &&
            choice <= 6 &&
            !battlefieldReady
        )
        {
            printf(
                "\nPlease setup the battlefield first.\n"
            );

            continue;
        }


        switch (choice)
        {
            case 1:

                runPart1A(
                    battlefield
                );

                break;


            case 2:

                runPart1B(
                    battlefield
                );

                break;


            case 3:

                runPart1C(
                    battlefield
                );

                break;


            case 4:

                runPart2A(
                    battlefield
                );

                break;


            case 5:

                runPart2B(
                    battlefield
                );

                break;


            case 6:

                runPart2C(
                    battlefield
                );

                break;


            case 7:

                break;


            default:

                printf(
                    "\nInvalid simulation option.\n"
                );
        }

    }
    while (
        choice != 7
    );
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    Battlefield battlefield;

    int choice;

    int battlefieldReady = 0;


    do
    {
        printf(
            "\n====================================\n"
        );

        printf(
            "     ADVANCED NAVAL BATTLE SIMULATOR\n"
        );

        printf(
            "====================================\n"
        );


        printf(
            "1. Start Simulation\n"
        );

        printf(
            "2. Setup Battlefield\n"
        );

        printf(
            "3. Show Current Battlefield\n"
        );

        printf(
            "4. View Instructions\n"
        );

        printf(
            "5. Simulation Statistics\n"
        );

        printf(
            "6. Exit\n"
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

                startSimulationMenu(
                    &battlefield,
                    battlefieldReady
                );

                break;


            case 2:

                setupBattlefield(
                    &battlefield
                );


                battlefieldReady = 1;


                /*
                   Save current setup so that it can
                   later be displayed in Statistics.
                */

                saveSetupSnapshot(
                    &battlefield
                );


                printf(
                    "\nBattlefield setup saved successfully.\n"
                );

                break;


            case 3:

                if (battlefieldReady)
                {
                    printBattlefield(
                        &battlefield
                    );
                }
                else
                {
                    printf(
                        "\nPlease setup the battlefield first.\n"
                    );
                }

                break;


            case 4:

                showInstructions();

                break;


            case 5:

                showSimulationStatistics();

                break;


            case 6:

                printf(
                    "\nAre you sure you want to exit? (1 = Yes, 0 = No): "
                );


                {
                    int confirmExit;


                    scanf(
                        "%d",
                        &confirmExit
                    );


                    if (
                        confirmExit == 1
                    )
                    {
                        printf(
                            "\nExiting Naval Battle Simulator.\n"
                        );
                    }
                    else
                    {
                        choice = 0;


                        printf(
                            "\nExit cancelled.\n"
                        );
                    }
                }

                break;


            default:

                printf(
                    "\nInvalid option. Please try again.\n"
                );
        }

    }
    while (
        choice != 6
    );


    return 0;
}