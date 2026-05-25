/*
========================================================
...
Uses binary file handling (fread / fwrite) for
persistent storage in patients.dat.

Last Updated By : Siddharth Chavan
Contribution    : Patient Management Module Improvements

========================================================
*/


#include <stdio.h>      /* printf, scanf, fopen, fclose, fread, fwrite, remove, rename */
#include <stdlib.h>     /* exit()                                                       */
#include <string.h>     /* strcspn() — used to strip trailing newline from fgets input  */
#include "patient.h"    /* Patient struct and function prototypes                       */

/* ── Internal Helper Prototypes ─────────────────────────────── */
/* These helpers are used only inside this file (static = file-scoped). */

static void        printDivider(void);
static void        printTableHeader(void);
static void        printPatientRow(const Patient *p);
static void        clearInputBuffer(void);

/* ============================================================
 *  SECTION 1 — ADD PATIENT
 * ============================================================ */

/*
 * addPatient()
 * ------------
 * Collects patient details from the user, validates that the
 * chosen ID is not already in use, then appends the new record
 * to the binary file in append-binary ("ab") mode.
 *
 * Binary file mode "ab":
 *   - Opens the file for writing in binary mode.
 *   - If the file does not exist, it is created automatically.
 *   - New records are always appended at the end, so existing
 *     records are never overwritten.
 */
void addPatient(void)
{
    Patient newPatient;   /* Local variable to hold the new record */
    FILE   *fp;           /* File pointer for patients.dat         */

    printf("  +----------------------------------+\n");
    printf("  |       ADD NEW PATIENT RECORD     |\n");
    printf("  +----------------------------------+\n");

    /* ── Step 1: Patient ID ──────────────────────────────────── */
    printf("\n  Enter Patient ID   : ");
    if (scanf("%d", &newPatient.patientId) != 1)
    {
        printf("  [ERROR] Invalid input. Patient ID must be a number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer(); /* Flush leftover '\n' from the input buffer */

    /*
     * Duplicate ID check:
     *   Before accepting this record, scan the existing file to
     *   make sure no patient already has this ID.
     */
    if (patientExists(newPatient.patientId))
    {
        printf("  [ERROR] Patient ID %d already exists. "
               "Please use a unique ID.\n", newPatient.patientId);
        return;
    }

    /* ── Step 2: Patient Name ────────────────────────────────── */
    printf("  Enter Patient Name : ");
    fgets(newPatient.name, MAX_NAME_LEN, stdin);
    /*
     * fgets() keeps the trailing '\n' in the string.
     * strcspn() finds that '\n' position so we can overwrite it
     * with '\0', giving us a clean null-terminated string.
     */
    newPatient.name[strcspn(newPatient.name, "\n")] = '\0';

    if (newPatient.name[0] == '\0')
    {
        printf("  [ERROR] Name cannot be empty.\n");
        return;
    }

    /* ── Step 3: Age ─────────────────────────────────────────── */
    printf("  Enter Age          : ");
    if (scanf("%d", &newPatient.age) != 1 || newPatient.age <= 0 || newPatient.age > 150)
    {
        printf("  [ERROR] Invalid age. Please enter a value between 1 and 150.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    /* ── Step 4: Gender ──────────────────────────────────────── */
    printf("  Enter Gender       : ");
    fgets(newPatient.gender, MAX_GENDER_LEN, stdin);
    newPatient.gender[strcspn(newPatient.gender, "\n")] = '\0';

    if (newPatient.gender[0] == '\0')
    {
        printf("  [ERROR] Gender cannot be empty.\n");
        return;
    }

    /* ── Step 5: Diagnosis ───────────────────────────────────── */
    printf("  Enter Diagnosis    : ");
    fgets(newPatient.diagnosis, MAX_DIAGNOSIS_LEN, stdin);
    newPatient.diagnosis[strcspn(newPatient.diagnosis, "\n")] = '\0';

    if (newPatient.diagnosis[0] == '\0')
    {
        printf("  [ERROR] Diagnosis cannot be empty.\n");
        return;
    }

    /* ── Step 6: Write to Binary File ────────────────────────── */
    /*
     * "ab" = append binary
     *   We open in append mode so each new patient is added at
     *   the end of the file without disturbing previous records.
     */
    fp = fopen(PATIENT_FILE, "ab");
    if (fp == NULL)
    {
        printf("  [ERROR] Could not open '%s' for writing.\n", PATIENT_FILE);
        printf("  Ensure the 'data/' directory exists.\n");
        return;
    }

    /*
     * fwrite(&newPatient, sizeof(Patient), 1, fp)
     *   - &newPatient  : pointer to the data to write
     *   - sizeof(Patient) : size of one record in bytes
     *   - 1            : write exactly 1 record
     *   - fp           : file pointer
     * Returns the number of records successfully written (should be 1).
     */
    if (fwrite(&newPatient, sizeof(Patient), 1, fp) != 1)
    {
        printf("  [ERROR] Failed to save patient record. Please try again.\n");
        fclose(fp);
        return;
    }

    fclose(fp); /* Always close the file after use */

    printf("\n  [SUCCESS] Patient '%s' (ID: %d) added successfully!\n",
           newPatient.name, newPatient.patientId);
    printf("  Record saved to '%s'.\n", PATIENT_FILE);
}

/* ============================================================
 *  SECTION 2 — VIEW ALL PATIENTS
 * ============================================================ */

/*
 * viewPatients()
 * --------------
 * Opens patients.dat in read-binary ("rb") mode and reads
 * every Patient record one by one using fread() in a loop.
 * Displays them in a formatted table.
 *
 * Loop pattern for reading all records from a binary file:
 *
 *   while (fread(&p, sizeof(Patient), 1, fp) == 1)
 *   {
 *       // process record p
 *   }
 *
 * fread() returns 1 (records read) on success, 0 at EOF or error.
 * This makes it a natural loop condition.
 */
void viewPatients(void)
{
    Patient p;         /* Temporary variable to hold each record read */
    FILE   *fp;        /* File pointer                                 */
    int     count = 0; /* Counter to detect empty file                 */

    printf("  +----------------------------------+\n");
    printf("  |       ALL PATIENT RECORDS        |\n");
    printf("  +----------------------------------+\n\n");

    /* "rb" = read binary — file must already exist */
    fp = fopen(PATIENT_FILE, "rb");
    if (fp == NULL)
    {
        printf("  [INFO] No patient records found. "
               "The file '%s' does not exist yet.\n", PATIENT_FILE);
        printf("  Add a patient first to create the data file.\n");
        return;
    }

    printTableHeader(); /* Print column headings and top border */

    /*
     * Read records one at a time until EOF.
     * fread fills 'p' with sizeof(Patient) bytes each iteration.
     */
    while (fread(&p, sizeof(Patient), 1, fp) == 1)
    {
        printPatientRow(&p); /* Print this record as one table row */
        count++;
    }

    printDivider(); /* Bottom border of the table */
    fclose(fp);

    if (count == 0)
    {
        printf("  [INFO] No patient records found in '%s'.\n", PATIENT_FILE);
    }
    else
    {
        printf("\n  Total Patients on Record: %d\n", count);
    }
}

/* ============================================================
 *  SECTION 3 — SEARCH PATIENT BY ID
 * ============================================================ */

/*
 * searchPatient()
 * ---------------
 * Prompts the user for a Patient ID, then scans patients.dat
 * sequentially until a matching record is found. Displays the
 * full record details in a readable card format.
 *
 * Sequential search is used because records are stored
 * contiguously in a binary file without indexing.
 */
void searchPatient(void)
{
    Patient p;           /* Holds each record as we scan the file */
    FILE   *fp;          /* File pointer                           */
    int     searchId;    /* The ID entered by the user             */
    int     found = 0;   /* Flag: 1 if record found, 0 otherwise   */

    printf("  +----------------------------------+\n");
    printf("  |         SEARCH PATIENT           |\n");
    printf("  +----------------------------------+\n");

    printf("\n  Enter Patient ID to search: ");
    if (scanf("%d", &searchId) != 1)
    {
        printf("  [ERROR] Invalid input. Please enter a numeric ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fp = fopen(PATIENT_FILE, "rb");
    if (fp == NULL)
    {
        printf("  [INFO] No records found. File '%s' does not exist.\n",
               PATIENT_FILE);
        return;
    }

    /*
     * Sequential scan:
     *   Read each Patient record and compare its patientId
     *   with the search target. Stop as soon as we find a match.
     */
    while (fread(&p, sizeof(Patient), 1, fp) == 1)
    {
        if (p.patientId == searchId)
        {
            found = 1; /* Mark as found */
            break;     /* No need to read further */
        }
    }

    fclose(fp);

    if (found)
    {
        /* Display the record in a structured card layout */
        printf("\n  [SUCCESS] Patient record found:\n");
        printf("  +----------------------------------+\n");
        printf("  |  PATIENT DETAILS                 |\n");
        printf("  +----------------------------------+\n");
        printf("  |  Patient ID  : %-18d|\n", p.patientId);
        printf("  |  Name        : %-18s|\n", p.name);
        printf("  |  Age         : %-18d|\n", p.age);
        printf("  |  Gender      : %-18s|\n", p.gender);
        printf("  |  Diagnosis   : %-18s|\n", p.diagnosis);
        printf("  +----------------------------------+\n");
    }
    else
    {
        printf("\n  [NOT FOUND] No patient with ID %d exists in the system.\n",
               searchId);
    }
}

/* ============================================================
 *  SECTION 4 — DELETE PATIENT BY ID
 * ============================================================ */

/*
 * deletePatient()
 * ---------------
 * Deletes a patient record identified by Patient ID using the
 * TEMPORARY FILE METHOD — the safest approach for binary file
 * record deletion in C.
 *
 * Algorithm:
 *   1. Open the original file for reading.
 *   2. Open a temporary file for writing.
 *   3. Copy every record EXCEPT the one to delete into the temp file.
 *   4. Close both files.
 *   5. Delete the original file using remove().
 *   6. Rename the temp file to the original filename using rename().
 *
 * Why a temp file?
 *   There is no way to remove bytes from the middle of a file
 *   in-place. The temp-file swap is the standard, safe pattern.
 */
void deletePatient(void)
{
    Patient p;             /* Holds each record during the copy loop */
    FILE   *fp;            /* File pointer for the original file     */
    FILE   *tempFp;        /* File pointer for the temporary file    */
    int     deleteId;      /* Patient ID the user wants to remove    */
    int     found = 0;     /* Flag: 1 if the record was located      */

    /* Temporary file created beside the original */
    const char *tempFile = "data/patients_temp.dat";

    printf("  +----------------------------------+\n");
    printf("  |         DELETE PATIENT           |\n");
    printf("  +----------------------------------+\n");

    printf("\n  Enter Patient ID to delete: ");
    if (scanf("%d", &deleteId) != 1)
    {
        printf("  [ERROR] Invalid input. Please enter a numeric ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    /* ── Step 1: Open original file for reading ──────────────── */
    fp = fopen(PATIENT_FILE, "rb");
    if (fp == NULL)
    {
        printf("  [INFO] No records found. File '%s' does not exist.\n",
               PATIENT_FILE);
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

    /* ── Step 3: Copy all records EXCEPT the one to delete ───── */
    while (fread(&p, sizeof(Patient), 1, fp) == 1)
    {
        if (p.patientId == deleteId)
        {
            found = 1; /* Skip this record — effectively deletes it */
            continue;  /* Do NOT write it to the temp file          */
        }
        /* Write every other record to the temp file */
        fwrite(&p, sizeof(Patient), 1, tempFp);
    }

    /* ── Step 4: Close both files ────────────────────────────── */
    fclose(fp);
    fclose(tempFp);

    if (!found)
    {
        /*
         * The ID was not found. Clean up the temp file and
         * leave the original untouched.
         */
        remove(tempFile);
        printf("\n  [NOT FOUND] No patient with ID %d exists in the system.\n",
               deleteId);
        return;
    }

    /* ── Step 5: Delete the original file ────────────────────── */
    if (remove(PATIENT_FILE) != 0)
    {
        printf("  [ERROR] Could not remove original file during deletion.\n");
        remove(tempFile); /* Clean up temp file */
        return;
    }

    /* ── Step 6: Rename temp file → original filename ────────── */
    if (rename(tempFile, PATIENT_FILE) != 0)
    {
        printf("  [ERROR] Could not rename temporary file. "
               "Data may be in '%s'.\n", tempFile);
        return;
    }

    printf("\n  [SUCCESS] Patient with ID %d has been deleted from the system.\n",
           deleteId);
}

/* ============================================================
 *  SECTION 5 — INTERNAL HELPER FUNCTIONS (static)
 * ============================================================
 *
 * These functions are declared 'static' which restricts their
 * visibility to this file only. They cannot be called from
 * main.c or any other module — they are private utilities.
 */

/* ──────────────────────────────────────────────────────────── */
/*  patientExists()                                             */
/*  Returns 1 if the given ID is already in patients.dat,      */
/*  returns 0 if the ID is free (or the file doesn't exist).   */
/* ──────────────────────────────────────────────────────────── */
int patientExists(int id)
{
    Patient p;
    FILE   *fp;

    fp = fopen(PATIENT_FILE, "rb");
    if (fp == NULL)
    {
        /*
         * File doesn't exist yet — that means no records at all,
         * so this ID definitely doesn't exist. Return 0 (safe).
         */
        return 0;
    }

    while (fread(&p, sizeof(Patient), 1, fp) == 1)
    {
        if (p.patientId == id)
        {
            fclose(fp);
            return 1; /* Duplicate found */
        }
    }

    fclose(fp);
    return 0; /* ID is unique */
}

/* ──────────────────────────────────────────────────────────── */
/*  printDivider()                                              */
/*  Prints a horizontal separator line for table formatting.   */
/* ──────────────────────────────────────────────────────────── */
static void printDivider(void)
{
    printf("  +----------+----------------------+-----+----------+---------------------------+\n");
}

/* ──────────────────────────────────────────────────────────── */
/*  printTableHeader()                                          */
/*  Prints the column header row of the patient records table. */
/* ──────────────────────────────────────────────────────────── */
static void printTableHeader(void)
{
    printDivider();
    printf("  | %-8s | %-20s | %-3s | %-8s | %-25s |\n",
           "ID", "Name", "Age", "Gender", "Diagnosis");
    printDivider();
}

/* ──────────────────────────────────────────────────────────── */
/*  printPatientRow()                                           */
/*  Prints a single Patient record as one formatted table row. */
/*  Takes a const pointer to avoid copying the full struct.    */
/* ──────────────────────────────────────────────────────────── */
static void printPatientRow(const Patient *p)
{
    printf("  | %-8d | %-20s | %-3d | %-8s | %-25s |\n",
           p->patientId,
           p->name,
           p->age,
           p->gender,
           p->diagnosis);
}

/* ──────────────────────────────────────────────────────────── */
/*  clearInputBuffer()                                          */
/*  Discards all characters remaining in stdin up to and       */
/*  including the next newline. This prevents stale '\n' or    */
/*  extra characters from corrupting subsequent fgets() calls. */
/* ──────────────────────────────────────────────────────────── */
static void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* Intentionally empty — just consuming leftover characters */
    }
}