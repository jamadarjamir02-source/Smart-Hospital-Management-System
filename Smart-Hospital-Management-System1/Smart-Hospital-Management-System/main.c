/* ============================================================
 * FILE        : main.c
 * PROJECT     : Smart Hospital Management System
 * SUBJECT     : Data Structures Using C (DSC)
 * TEAM        : Siddharth | Nishant | Kunal | Jamir
 * DESCRIPTION : Entry point of the program. Displays a
 *               menu-driven interface and routes every user
 *               choice to the appropriate module function.
 *               All four modules are integrated here:
 *                 • Patient Management    (patient.h)
 *                 • Doctor  Management    (doctor.h)
 *                 • Appointment / Queue   (appointment.h)
 *                 • Billing               (billing.h)
 * ============================================================ */

#include <stdio.h>        /* printf, scanf                    */
#include "patient.h"      /* Patient module — Siddharth       */
#include "doctor.h"       /* Doctor  module — Nishant         */
#include "appointment.h"  /* Appointment / Queue  — Kunal     */
#include "billing.h"      /* Billing module       — Kunal     */

/* ── Internal Helper Prototypes ─────────────────────────────── */
static void clearMainInputBuffer(void);
static void displayWelcomeBanner(void);
static void displayMenu(void);
static void displayExitBanner(void);

/* ============================================================
 *  MAIN FUNCTION
 * ============================================================ */

int main(void)
{
    int choice;   /* Stores the menu option entered by the user */

    /* ── Step 1: Welcome Screen ──────────────────────────────── */
    displayWelcomeBanner();

    /* ── Step 2: Initialise Appointment Queue ────────────────── */
    /*
     * initializeQueue() MUST be called once before any other
     * appointment function is used. It sets front = 0, rear = -1,
     * and reloads any previously saved appointments from disk so
     * that queue data survives program restarts.
     */
    initializeQueue();

    /* ── Step 3: Main Menu Loop ──────────────────────────────── */
    /*
     * do-while loop: ensures the menu is shown at least once.
     * The loop continues until the user enters 0 (Exit).
     *
     * switch-case: maps each valid integer choice to the correct
     * module function. The default case handles any invalid input.
     */
    do
    {
        displayMenu(); /* Print the full menu each iteration */

        printf("  Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            /*
             * Non-integer input (e.g. letters) causes scanf()
             * to fail and leave garbage in stdin.
             * Clear the buffer and reset choice to an invalid
             * value so the default case fires.
             */
            if (feof(stdin) || ferror(stdin))
            {
                /* Force program exit to prevent infinite loop on EOF */
                choice = 0;
            }
            else
            {
                clearMainInputBuffer();
                choice = -1;
            }
        }
        else
        {
            clearMainInputBuffer(); /* Flush trailing '\n' */
        }

        /* ── Route choice to the appropriate module ─────────── */
        switch (choice)
        {
            /* ════════════ PATIENT MANAGEMENT ════════════ */

            case 1:
                addPatient();
                break;

            case 2:
                viewPatients();
                break;

            case 3:
                searchPatient();
                break;

            case 4:
                deletePatient();
                break;

            /* ════════════ DOCTOR MANAGEMENT ═════════════ */

            case 5:
                addDoctor();
                break;

            case 6:
                viewDoctors();
                break;

            case 7:
                searchDoctor();
                break;

            case 8:
                deleteDoctor();
                break;

            /* ══════════ APPOINTMENT MANAGEMENT ══════════ */

            case 9:
                bookAppointment();
                break;

            case 10:
                viewAppointments();
                break;

            case 11:
                serveAppointment();
                break;

            /* ════════════ BILLING MANAGEMENT ════════════ */

            case 12:
                generateBill();
                break;

            case 13:
                viewBills();
                break;

            /* ═══════════════════ EXIT ════════════════════ */

            case 0:
                displayExitBanner();
                break;

            /* ══════════════ INVALID CHOICE ═══════════════ */

            default:
                printf("\n");
                printf("  +--------------------------------------+\n");
                printf("  |           INVALID CHOICE             |\n");
                printf("  +--------------------------------------+\n");
                printf("  Please enter a number between 0 and 13.\n\n");
                break;
        }

        /* ── Pause between operations ─────────────────────────
         * After each operation (except Exit), prompt the user
         * to press Enter before redrawing the menu. This keeps
         * the output readable — they can review results before
         * the screen scrolls to a fresh menu.
         */
        if (choice != 0)
        {
            printf("\n  Press Enter to return to the main menu...");
            getchar(); /* Wait for the user to press Enter */
        }

    } while (choice != 0); /* Loop exits when user selects Exit */

    return 0; /* Program completed successfully */
}

/* ============================================================
 *  SECTION 2 — DISPLAY HELPERS (static)
 * ============================================================ */

/* ──────────────────────────────────────────────────────────── */
/*  displayWelcomeBanner()                                      */
/*  Printed once at startup before the first menu appears.     */
/*  Shows the project title, team info, and module list.       */
/* ──────────────────────────────────────────────────────────── */
static void displayWelcomeBanner(void)
{
    printf("\n");
    printf("  +------------------------------------------------------+\n");
    printf("  |                                                      |\n");
    printf("  |        SMART HOSPITAL MANAGEMENT SYSTEM              |\n");
    printf("  |                                                      |\n");
    printf("  |          Subject  : Data Structures Using C          |\n");
    printf("  |          Language : C  (Binary File Handling)        |\n");
    printf("  |                                                      |\n");
    printf("  +------------------------------------------------------+\n");
    printf("  |  TEAM MEMBERS                                        |\n");
    printf("  |  -------------------------------------------------   |\n");
    printf("  |  Siddharth  ->  Team Leader & Patient Management     |\n");
    printf("  |  Nishant    ->  Doctor  Management                   |\n");
    printf("  |  Kunal      ->  Appointment Management & Billing     |\n");
    printf("  |  Jamir      ->  Integration, Testing & Documentation |\n");
    printf("  +------------------------------------------------------+\n");
    printf("  |  MODULES                                             |\n");
    printf("  |  -------------------------------------------------   |\n");
    printf("  |  [1] Patient Management    [2] Doctor  Management    |\n");
    printf("  |  [3] Appointment Queue     [4] Billing System        |\n");
    printf("  +------------------------------------------------------+\n");
    printf("\n");
}

/* ──────────────────────────────────────────────────────────── */
/*  displayMenu()                                               */
/*  Printed at the start of every loop iteration.              */
/*  Groups all 13 operations under four labelled sections.     */
/* ──────────────────────────────────────────────────────────── */
static void displayMenu(void)
{
    printf("\n");
    printf("  +------------------------------------------------------+\n");
    printf("  |               MAIN MENU                              |\n");
    printf("  +------------------------------------------------------+\n");
    printf("  |                                                      |\n");
    printf("  |   -- PATIENT MANAGEMENT --------------------------   |\n");
    printf("  |    1.  Add    Patient                                |\n");
    printf("  |    2.  View   Patients                               |\n");
    printf("  |    3.  Search Patient                                |\n");
    printf("  |    4.  Delete Patient                                |\n");
    printf("  |                                                      |\n");
    printf("  |   -- DOCTOR MANAGEMENT ---------------------------   |\n");
    printf("  |    5.  Add    Doctor                                 |\n");
    printf("  |    6.  View   Doctors                                |\n");
    printf("  |    7.  Search Doctor                                 |\n");
    printf("  |    8.  Delete Doctor                                 |\n");
    printf("  |                                                      |\n");
    printf("  |   -- APPOINTMENT MANAGEMENT (QUEUE) --------------   |\n");
    printf("  |    9.  Book   Appointment                            |\n");
    printf("  |   10.  View   Appointments                           |\n");
    printf("  |   11.  Serve  Appointment  (FIFO)                    |\n");
    printf("  |                                                      |\n");
    printf("  |   -- BILLING -------------------------------------   |\n");
    printf("  |   12.  Generate Bill                                 |\n");
    printf("  |   13.  View    Bills                                 |\n");
    printf("  |                                                      |\n");
    printf("  |   -- EXIT ----------------------------------------   |\n");
    printf("  |    0.  Exit  Program                                 |\n");
    printf("  |                                                      |\n");
    printf("  +------------------------------------------------------+\n");
}

/* ──────────────────────────────────────────────────────────── */
/*  displayExitBanner()                                         */
/*  Shown once when the user selects option 0.                 */
/*  Provides a clean, professional program termination message.*/
/* ──────────────────────────────────────────────────────────── */
static void displayExitBanner(void)
{
    printf("\n");
    printf("  +------------------------------------------------------+\n");
    printf("  |                                                      |\n");
    printf("  |   Thank you for using Smart Hospital Management      |\n");
    printf("  |   System. All records have been saved successfully.  |\n");
    printf("  |                                                      |\n");
    printf("  |   Developed by:                                      |\n");
    printf("  |     Siddharth | Nishant | Kunal | Jamir              |\n");
    printf("  |                                                      |\n");
    printf("  |   Subject : Data Structures Using C  (DSC)           |\n");
    printf("  |                                                      |\n");
    printf("  +------------------------------------------------------+\n");
    printf("\n");
}

/* ──────────────────────────────────────────────────────────── */
/*  clearMainInputBuffer()                                      */
/*  Discards all characters in stdin up to and including '\n'. */
/*  Called after every scanf() to prevent leftover characters  */
/*  from polluting the next input operation.                   */
/* ──────────────────────────────────────────────────────────── */
static void clearMainInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* Intentionally empty — consuming leftover characters */
    }
}