#include <stdio.h>

#include "ship.h"
#include "battle.h"


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
            "1. Setup Battlefield\n"
        );

        printf(
            "2. Show Battlefield\n"
        );

        printf(
            "3. Run Part 1-A Simulation\n"
        );

        printf(
            "4. Exit\n"
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

                setupBattlefield(
                    &battlefield
                );

                battlefieldReady = 1;

                break;


            case 2:

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


            case 3:

                if (battlefieldReady)
                {
                    runPart1A(
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

                printf(
                    "\nExiting simulator.\n"
                );

                break;


            default:

                printf(
                    "\nInvalid option. Please try again.\n"
                );
        }

    }
    while (choice != 4);


    return 0;
}