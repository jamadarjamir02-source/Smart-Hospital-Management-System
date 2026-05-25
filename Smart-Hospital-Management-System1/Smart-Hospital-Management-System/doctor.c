/* ============================================================
 * FILE        : doctor.c
 * MODULE      : Doctor Management Module
 * PROJECT     : Smart Hospital Management System
 * SUBJECT     : Data Structures Using C (DSC)
 * AUTHOR      : Nishant (Doctor Management)
 * DESCRIPTION : Implementation of all doctor management
 *               functions — add, view, search, and delete.
 *               Uses binary file handling (fread / fwrite) for
 *               persistent storage in doctors.dat.
 * ============================================================ */

#include <stdio.h>      /* printf, scanf, fopen, fclose, fread, fwrite, remove, rename */
#include <stdlib.h>     /* exit()                                                       */
#include <string.h>     /* strcspn() — strips trailing newline from fgets input         */
#include "doctor.h"     /* Doctor struct and function prototypes                        */

/* ── Internal Helper Prototypes ─────────────────────────────── */
/*
 * Declared 'static' — these functions are private to this file.
 * They cannot be called from main.c or any other module.
 */

static void  printDoctorDivider(void);
static void  printDoctorTableHeader(void);
static void  printDoctorRow(const Doctor *d);
static void  clearDoctorInputBuffer(void);

/* ============================================================
 *  SECTION 1 — ADD DOCTOR
 * ============================================================ */

/*
 * addDoctor()
 * -----------
 * Collects doctor details from the user, validates that the
 * chosen ID is not already in use, then appends the new record
 * to the binary file in append-binary ("ab") mode.
 *
 * Binary file mode "ab":
 *   - Opens the file for writing in binary mode.
 *   - Creates the file automatically if it does not exist.
 *   - New records are always appended at the end — existing
 *     records are never overwritten.
 */
void addDoctor(void)
{
    Doctor  newDoctor;   /* Local variable to hold the new record */
    FILE   *fp;          /* File pointer for doctors.dat          */

    printf("  +----------------------------------+\n");
    printf("  |       ADD NEW DOCTOR RECORD      |\n");
    printf("  +----------------------------------+\n");

    /* ── Step 1: Doctor ID ───────────────────────────────────── */
    printf("\n  Enter Doctor ID       : ");
    if (scanf("%d", &newDoctor.doctorId) != 1)
    {
        printf("  [ERROR] Invalid input. Doctor ID must be a number.\n");
        clearDoctorInputBuffer();
        return;
    }
    clearDoctorInputBuffer(); /* Flush leftover '\n' from the input buffer */

    /*
     * Duplicate ID check:
     *   Scan the existing file before accepting the new record.
     *   Every doctor must have a unique ID in the system.
     */
    if (doctorExists(newDoctor.doctorId))
    {
        printf("  [ERROR] Doctor ID %d already exists. "
               "Please use a unique ID.\n", newDoctor.doctorId);
        return;
    }

    /* ── Step 2: Doctor Name ─────────────────────────────────── */
    printf("  Enter Doctor Name     : ");
    fgets(newDoctor.name, MAX_DOCTOR_NAME_LEN, stdin);
    /*
     * fgets() keeps the trailing '\n' character in the buffer.
     * strcspn() locates that '\n' so we can replace it with '\0',
     * giving us a clean, null-terminated string.
     */
    newDoctor.name[strcspn(newDoctor.name, "\n")] = '\0';

    if (newDoctor.name[0] == '\0')
    {
        printf("  [ERROR] Doctor name cannot be empty.\n");
        return;
    }

    /* ── Step 3: Specialization ──────────────────────────────── */
    printf("  Enter Specialization  : ");
    fgets(newDoctor.specialization, MAX_SPECIALIZATION_LEN, stdin);
    newDoctor.specialization[strcspn(newDoctor.specialization, "\n")] = '\0';

    if (newDoctor.specialization[0] == '\0')
    {
        printf("  [ERROR] Specialization cannot be empty.\n");
        return;
    }

    /* ── Step 4: Write to Binary File ────────────────────────── */
    /*
     * "ab" = append binary
     *   We open in append mode so each new doctor is written at
     *   the end of the file without affecting previous records.
     */
    fp = fopen(DOCTOR_FILE, "ab");
    if (fp == NULL)
    {
        printf("  [ERROR] Could not open '%s' for writing.\n", DOCTOR_FILE);
        printf("  Ensure the 'data/' directory exists.\n");
        return;
    }

    /*
     * fwrite(&newDoctor, sizeof(Doctor), 1, fp)
     *   - &newDoctor    : pointer to the struct to write
     *   - sizeof(Doctor): number of bytes for one record
     *   - 1             : write exactly 1 record
     *   - fp            : the open file pointer
     * Returns the number of records written — should be 1.
     */
    if (fwrite(&newDoctor, sizeof(Doctor), 1, fp) != 1)
    {
        printf("  [ERROR] Failed to save doctor record. Please try again.\n");
        fclose(fp);
        return;
    }

    fclose(fp); /* Always close the file after use */

    printf("\n  [SUCCESS] Dr. %s (ID: %d) added successfully!\n",
           newDoctor.name, newDoctor.doctorId);
    printf("  Record saved to '%s'.\n", DOCTOR_FILE);
}

/* ============================================================
 *  SECTION 2 — VIEW ALL DOCTORS
 * ============================================================ */

/*
 * viewDoctors()
 * -------------
 * Opens doctors.dat in read-binary ("rb") mode and reads every
 * Doctor record one by one using fread() inside a while loop.
 * Each record is printed as a formatted table row.
 *
 * fread() loop pattern:
 *
 *   while (fread(&d, sizeof(Doctor), 1, fp) == 1)
 *   {
 *       // process record d
 *   }
 *
 * fread() returns 1 on a successful read, and 0 at EOF or on
 * error — making it a clean, natural loop condition.
 */
void viewDoctors(void)
{
    Doctor  d;         /* Temporary variable to hold each record read */
    FILE   *fp;        /* File pointer                                 */
    int     count = 0; /* Counter to detect an empty file             */

    printf("  +----------------------------------+\n");
    printf("  |        ALL DOCTOR RECORDS        |\n");
    printf("  +----------------------------------+\n\n");

    /* "rb" = read binary — the file must already exist */
    fp = fopen(DOCTOR_FILE, "rb");
    if (fp == NULL)
    {
        printf("  [INFO] No doctor records found. "
               "The file '%s' does not exist yet.\n", DOCTOR_FILE);
        printf("  Add a doctor first to create the data file.\n");
        return;
    }

    printDoctorTableHeader(); /* Print column headings and top border */

    /*
     * Read records sequentially until fread() signals EOF
     * by returning 0 (fewer than 1 record read).
     */
    while (fread(&d, sizeof(Doctor), 1, fp) == 1)
    {
        printDoctorRow(&d); /* Print this record as one table row */
        count++;
    }

    printDoctorDivider(); /* Bottom border of the table */
    fclose(fp);

    if (count == 0)
    {
        printf("  [INFO] No doctor records found in '%s'.\n", DOCTOR_FILE);
    }
    else
    {
        printf("\n  Total Doctors on Record: %d\n", count);
    }
}

/* ============================================================
 *  SECTION 3 — SEARCH DOCTOR BY ID
 * ============================================================ */

/*
 * searchDoctor()
 * --------------
 * Prompts the user for a Doctor ID, then performs a sequential
 * scan of doctors.dat until a matching record is found.
 * Displays the complete record in a card layout if found.
 *
 * Why sequential search?
 *   Records are written back-to-back in a flat binary file with
 *   no index or sorted order. We must read every record in turn
 *   until we find a match or exhaust the file.
 */
void searchDoctor(void)
{
    Doctor  d;           /* Holds each record during the scan  */
    FILE   *fp;          /* File pointer                        */
    int     searchId;    /* ID entered by the user to find      */
    int     found = 0;   /* Flag: 1 = record located, 0 = not  */

    printf("  +----------------------------------+\n");
    printf("  |          SEARCH DOCTOR           |\n");
    printf("  +----------------------------------+\n");

    printf("\n  Enter Doctor ID to search: ");
    if (scanf("%d", &searchId) != 1)
    {
        printf("  [ERROR] Invalid input. Please enter a numeric ID.\n");
        clearDoctorInputBuffer();
        return;
    }
    clearDoctorInputBuffer();

    fp = fopen(DOCTOR_FILE, "rb");
    if (fp == NULL)
    {
        printf("  [INFO] No records found. File '%s' does not exist.\n",
               DOCTOR_FILE);
        return;
    }

    /*
     * Sequential scan:
     *   Compare each record's doctorId with the search target.
     *   Break immediately on a match to avoid reading more data.
     */
    while (fread(&d, sizeof(Doctor), 1, fp) == 1)
    {
        if (d.doctorId == searchId)
        {
            found = 1; /* Record located */
            break;     /* Stop reading — no need to continue */
        }
    }

    fclose(fp);

    if (found)
    {
        /* Display the full record in a structured card format */
        printf("\n  [SUCCESS] Doctor record found:\n");
        printf("  +----------------------------------+\n");
        printf("  |  DOCTOR DETAILS                  |\n");
        printf("  +----------------------------------+\n");
        printf("  |  Doctor ID      : %-14d |\n", d.doctorId);
        printf("  |  Name           : %-14s |\n", d.name);
        printf("  |  Specialization : %-14s |\n", d.specialization);
        printf("  +----------------------------------+\n");
    }
    else
    {
        printf("\n  [NOT FOUND] No doctor with ID %d exists in the system.\n",
               searchId);
    }
}

/* ============================================================
 *  SECTION 4 — DELETE DOCTOR BY ID
 * ============================================================ */

/*
 * deleteDoctor()
 * --------------
 * Removes a doctor record identified by Doctor ID using the
 * TEMPORARY FILE METHOD — the standard safe approach for
 * deleting a record from a binary file in C.
 *
 * Algorithm:
 *   1. Open the original file (doctors.dat) for reading.
 *   2. Open a temporary file (doctors_temp.dat) for writing.
 *   3. Copy every record into the temp file EXCEPT the target.
 *   4. Close both files.
 *   5. Delete the original file with remove().
 *   6. Rename the temp file to the original name with rename().
 *
 * Why this approach?
 *   C has no built-in way to remove bytes from the middle of a
 *   file. The temp-file swap is the universally accepted pattern
 *   for binary file record deletion.
 */
void deleteDoctor(void)
{
    Doctor  d;             /* Holds each record during the copy loop  */
    FILE   *fp;            /* File pointer for the original file      */
    FILE   *tempFp;        /* File pointer for the temporary file     */
    int     deleteId;      /* Doctor ID the user wants to remove      */
    int     found = 0;     /* Flag: 1 if the record was located       */

    const char *tempFile = "data/doctors_temp.dat"; /* Temporary file path */

    printf("  +----------------------------------+\n");
    printf("  |          DELETE DOCTOR           |\n");
    printf("  +----------------------------------+\n");

    printf("\n  Enter Doctor ID to delete: ");
    if (scanf("%d", &deleteId) != 1)
    {
        printf("  [ERROR] Invalid input. Please enter a numeric ID.\n");
        clearDoctorInputBuffer();
        return;
    }
    clearDoctorInputBuffer();

    /* ── Step 1: Open original file for reading ──────────────── */
    fp = fopen(DOCTOR_FILE, "rb");
    if (fp == NULL)
    {
        printf("  [INFO] No records found. File '%s' does not exist.\n",
               DOCTOR_FILE);
        return;
    }

    /* ── Step 2: Open temp file for writing ──────────────────── */
    tempFp = fopen(tempFile, "wb");
    if (tempFp == NULL)
    {
        printf("  [ERROR] Could not create temporary file for deletion.\n");
        fclose(fp);
        return;
    }

    /* ── Step 3: Copy every record EXCEPT the one to delete ───── */
    while (fread(&d, sizeof(Doctor), 1, fp) == 1)
    {
        if (d.doctorId == deleteId)
        {
            found = 1; /* Mark as found — do NOT write to temp file */
            continue;  /* Skip this record, effectively deleting it */
        }
        /* Write all other records into the temp file unchanged */
        fwrite(&d, sizeof(Doctor), 1, tempFp);
    }

    /* ── Step 4: Close both files ────────────────────────────── */
    fclose(fp);
    fclose(tempFp);

    if (!found)
    {
        /*
         * The target ID was not found anywhere in the file.
         * Remove the empty temp file and leave the original intact.
         */
        remove(tempFile);
        printf("\n  [NOT FOUND] No doctor with ID %d exists in the system.\n",
               deleteId);
        return;
    }

    /* ── Step 5: Delete the original file ────────────────────── */
    if (remove(DOCTOR_FILE) != 0)
    {
        printf("  [ERROR] Could not remove the original file during deletion.\n");
        remove(tempFile); /* Clean up the temp file to avoid leftover data */
        return;
    }

    /* ── Step 6: Rename temp file → original filename ────────── */
    if (rename(tempFile, DOCTOR_FILE) != 0)
    {
        printf("  [ERROR] Could not rename temporary file. "
               "Remaining data is in '%s'.\n", tempFile);
        return;
    }

    printf("\n  [SUCCESS] Doctor with ID %d has been deleted from the system.\n",
           deleteId);
}

/* ============================================================
 *  SECTION 5 — INTERNAL HELPER FUNCTIONS (static)
 * ============================================================
 *
 * 'static' restricts these functions to this translation unit.
 * They are private implementation details — no other .c file
 * or module can call them directly.
 */

/* ──────────────────────────────────────────────────────────── */
/*  doctorExists()                                              */
/*  Scans doctors.dat for a given ID.                          */
/*  Returns 1 (true) if found, 0 (false) if not found or      */
/*  if the data file does not yet exist.                       */
/* ──────────────────────────────────────────────────────────── */
int doctorExists(int id)
{
    Doctor  d;
    FILE   *fp;

    fp = fopen(DOCTOR_FILE, "rb");
    if (fp == NULL)
    {
        /*
         * File doesn't exist yet — no records have been saved,
         * so this ID is guaranteed to be unique. Return 0 (safe).
         */
        return 0;
    }

    while (fread(&d, sizeof(Doctor), 1, fp) == 1)
    {
        if (d.doctorId == id)
        {
            fclose(fp);
            return 1; /* Duplicate ID found */
        }
    }

    fclose(fp);
    return 0; /* ID is free — no duplicates detected */
}

/* ──────────────────────────────────────────────────────────── */
/*  printDoctorDivider()                                        */
/*  Prints a horizontal separator line for table formatting.   */
/* ──────────────────────────────────────────────────────────── */
static void printDoctorDivider(void)
{
    printf("  +----------+----------------------+----------------------+\n");
}

/* ──────────────────────────────────────────────────────────── */
/*  printDoctorTableHeader()                                    */
/*  Prints the column header row of the doctor records table.  */
/* ──────────────────────────────────────────────────────────── */
static void printDoctorTableHeader(void)
{
    printDoctorDivider();
    printf("  | %-8s | %-20s | %-20s |\n",
           "ID", "Name", "Specialization");
    printDoctorDivider();
}

/* ──────────────────────────────────────────────────────────── */
/*  printDoctorRow()                                            */
/*  Prints one Doctor record as a single formatted table row.  */
/*  Takes a const pointer — avoids copying the struct and      */
/*  signals that the data will not be modified inside.         */
/* ──────────────────────────────────────────────────────────── */
static void printDoctorRow(const Doctor *d)
{
    printf("  | %-8d | %-20s | %-20s |\n",
           d->doctorId,
           d->name,
           d->specialization);
}

/* ──────────────────────────────────────────────────────────── */
/*  clearDoctorInputBuffer()                                    */
/*  Discards all characters remaining in stdin up to and       */
/*  including the next newline '\n' or EOF.                    */
/*                                                              */
/*  Why this is necessary:                                      */
/*  After scanf() reads a number, the '\n' the user pressed    */
/*  stays in stdin. The next fgets() call would immediately    */
/*  read that '\n' and return an empty string. Calling this    */
/*  function after every scanf() prevents that bug.            */
/* ──────────────────────────────────────────────────────────── */
static void clearDoctorInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* Intentionally empty — consuming leftover characters */
    }
}