/* ============================================================
 * FILE        : billing.c
 * MODULE      : Billing Module
 * PROJECT     : Smart Hospital Management System
 * SUBJECT     : Data Structures Using C (DSC)
 * AUTHOR      : Kunal (Appointment Management & Billing)
 * DESCRIPTION : Implementation of all billing functions.
 *               generateBill() collects fee inputs, computes
 *               the total automatically, validates all fields,
 *               and persists the record via binary file I/O.
 *               viewBills() reads and displays every bill with
 *               a formatted table and a grand-total summary.
 *
 * ── HOW TOTAL IS CALCULATED ────────────────────────────────
 *
 *   consultationFee  ──┐
 *                      ├──► totalAmount = consultationFee
 *   medicineFee      ──┘                 + medicineFee
 *
 *   The user NEVER types the total — the program calculates
 *   it and stores it with the record so viewBills() can both
 *   display per-bill totals and accumulate a grand total.
 *
 * ── BILL RECEIPT LAYOUT (generateBill output) ──────────────
 *
 *   ╔══════════════════════════════════╗
 *   ║         PAYMENT RECEIPT         ║
 *   ╠══════════════════════════════════╣
 *   ║  Bill ID          :  1001       ║
 *   ║  Patient ID       :  201        ║
 *   ╠══════════════════════════════════╣
 *   ║  Consultation Fee : Rs.  500.00 ║
 *   ║  Medicine Fee     : Rs.  250.00 ║
 *   ╠══════════════════════════════════╣
 *   ║  TOTAL AMOUNT     : Rs.  750.00 ║
 *   ╚══════════════════════════════════╝
 *
 * ============================================================ */

#include <stdio.h>    /* printf, scanf, fopen, fclose, fread, fwrite */
#include <string.h>   /* (included for consistency — used in other modules) */
#include <stdlib.h>   /* exit()                                             */
#include "billing.h"  /* Bill struct and function prototypes                */
#include "patient.h"  /* patientExists()                                    */

/* ── Internal Helper Prototypes ─────────────────────────────── */
/*
 * All helpers are 'static' — they are private to this file.
 * No other module (.c file) can call or access them.
 */
static void  printBillDivider(void);
static void  printBillTableHeader(void);
static void  printBillRow(const Bill *b);
static void  printReceipt(const Bill *b);
static int   billIdExists(int id);
static void  clearBillInputBuffer(void);

/* ============================================================
 *  SECTION 1 — GENERATE BILL
 * ============================================================ */

/*
 * generateBill()
 * --------------
 * Walks the user through entering bill details step by step:
 *
 *   1. Bill ID        → unique ID, validated against bills.dat
 *   2. Patient ID     → links this bill to an existing patient
 *   3. Consultation Fee → must be >= 0.0
 *   4. Medicine Fee     → must be >= 0.0
 *   5. Total Amount   → CALCULATED automatically (not entered)
 *   6. Write record   → appended to bills.dat in binary mode
 *   7. Print receipt  → formatted payment summary on screen
 *
 * File mode used: "ab" (append binary)
 *   - Creates bills.dat on first run if it does not exist.
 *   - All subsequent bills are appended at the end.
 *   - Existing records are never overwritten.
 */
void generateBill(void)
{
    Bill  newBill;   /* Local variable — holds the bill being created */
    FILE *fp;        /* File pointer for bills.dat                    */

    printf("  +--------------------------------------+\n");
    printf("  |         GENERATE PATIENT BILL        |\n");
    printf("  +--------------------------------------+\n");

    /* ── Step 1: Bill ID ─────────────────────────────────────── */
    printf("\n  Enter Bill ID            : ");
    if (scanf("%d", &newBill.billId) != 1)
    {
        printf("  [ERROR] Invalid input. Bill ID must be a number.\n");
        clearBillInputBuffer();
        return;
    }
    clearBillInputBuffer();

    /*
     * Duplicate Bill ID check:
     *   Scan bills.dat before accepting this record.
     *   Every bill must have a unique ID in the system.
     */
    if (billIdExists(newBill.billId))
    {
        printf("  [ERROR] Bill ID %d already exists.\n", newBill.billId);
        printf("  Please use a unique Bill ID.\n");
        return;
    }

    /* ── Step 2: Patient ID ──────────────────────────────────── */
    printf("  Enter Patient ID         : ");
    if (scanf("%d", &newBill.patientId) != 1)
    {
        printf("  [ERROR] Invalid input. Patient ID must be a number.\n");
        clearBillInputBuffer();
        return;
    }
    clearBillInputBuffer();

    /* Validate Patient existence */
    if (!patientExists(newBill.patientId))
    {
        printf("  [ERROR] Patient ID %d does not exist in the system.\n", newBill.patientId);
        printf("  Please register the patient first.\n");
        return;
    }

    /* ── Step 3: Consultation Fee ────────────────────────────── */
    printf("  Enter Consultation Fee   : Rs. ");
    if (scanf("%f", &newBill.consultationFee) != 1)
    {
        printf("  [ERROR] Invalid input. Fee must be a numeric value.\n");
        clearBillInputBuffer();
        return;
    }
    clearBillInputBuffer();

    /*
     * Negative fee guard:
     *   A fee of 0.0 is permitted (e.g. waived charges).
     *   Anything below 0.0 is invalid — reject it.
     */
    if (newBill.consultationFee < 0.0f)
    {
        printf("  [ERROR] Consultation fee cannot be negative.\n");
        return;
    }

    /* ── Step 4: Medicine Fee ────────────────────────────────── */
    printf("  Enter Medicine Fee       : Rs. ");
    if (scanf("%f", &newBill.medicineFee) != 1)
    {
        printf("  [ERROR] Invalid input. Fee must be a numeric value.\n");
        clearBillInputBuffer();
        return;
    }
    clearBillInputBuffer();

    if (newBill.medicineFee < 0.0f)
    {
        printf("  [ERROR] Medicine fee cannot be negative.\n");
        return;
    }

    /* ── Step 5: Auto-Calculate Total ────────────────────────── */
    /*
     * The total is NEVER entered by the user.
     * It is computed here and stored with the record so that:
     *   a) viewBills() can display it without recalculating.
     *   b) A grand total can be accumulated across all bills.
     */
    newBill.totalAmount = newBill.consultationFee + newBill.medicineFee;

    /* ── Step 6: Write to Binary File ────────────────────────── */
    /*
     * "ab" = append binary
     *   Appends this bill at the end of bills.dat.
     *   If the file does not exist, it is created automatically.
     *
     * fwrite(&newBill, sizeof(Bill), 1, fp):
     *   Writes sizeof(Bill) bytes starting at &newBill.
     *   Returns 1 on success, 0 on failure.
     */
    fp = fopen(BILL_FILE, "ab");
    if (fp == NULL)
    {
        printf("  [ERROR] Could not open '%s' for writing.\n", BILL_FILE);
        printf("  Ensure the 'data/' directory exists.\n");
        return;
    }

    if (fwrite(&newBill, sizeof(Bill), 1, fp) != 1)
    {
        printf("  [ERROR] Failed to save bill record. Please try again.\n");
        fclose(fp);
        return;
    }

    fclose(fp); /* Always close the file immediately after use */

    /* ── Step 7: Print Payment Receipt ──────────────────────── */
    printReceipt(&newBill);

    printf("  [SUCCESS] Bill ID %d saved to '%s'.\n",
           newBill.billId, BILL_FILE);
}

/* ============================================================
 *  SECTION 2 — VIEW ALL BILLS
 * ============================================================ */

/*
 * viewBills()
 * -----------
 * Opens bills.dat in read-binary ("rb") mode and reads every
 * Bill record sequentially using fread() in a while loop.
 * Prints each record as a formatted table row and accumulates
 * a grand total across all bills for the summary footer.
 *
 * fread() loop pattern:
 *
 *   while (fread(&b, sizeof(Bill), 1, fp) == 1)
 *   {
 *       // process record b
 *   }
 *
 * fread() returns 1 on a successful single-record read,
 * and returns 0 at EOF or on error — a natural loop condition.
 *
 * Grand total accumulation:
 *   grandTotal += b.totalAmount;  (added for each record read)
 */
void viewBills(void)
{
    Bill  b;                  /* Temporary holder for each record read */
    FILE *fp;                 /* File pointer for bills.dat            */
    int   count      = 0;    /* Number of bill records found          */
    float grandTotal = 0.0f; /* Running sum of all totalAmount fields */

    printf("  +--------------------------------------+\n");
    printf("  |          ALL BILL RECORDS            |\n");
    printf("  +--------------------------------------+\n\n");

    /* "rb" = read binary — file must already exist */
    fp = fopen(BILL_FILE, "rb");
    if (fp == NULL)
    {
        printf("  [INFO] No bill records found.\n");
        printf("  The file '%s' does not exist yet.\n", BILL_FILE);
        printf("  Generate a bill first to create the data file.\n");
        return;
    }

    printBillTableHeader(); /* Column headings + top border */

    /*
     * Read one Bill record at a time.
     * Loop exits automatically when fread() hits EOF (returns 0).
     */
    while (fread(&b, sizeof(Bill), 1, fp) == 1)
    {
        printBillRow(&b);           /* Print as one table row     */
        grandTotal += b.totalAmount; /* Accumulate running total   */
        count++;
    }

    printBillDivider(); /* Bottom border of the table */
    fclose(fp);

    /* ── Summary Footer ──────────────────────────────────────── */
    if (count == 0)
    {
        printf("  [INFO] No bill records found in '%s'.\n", BILL_FILE);
    }
    else
    {
        printf("\n");
        printf("  +----------------------------------------------+\n");
        printf("  |               BILLING SUMMARY                |\n");
        printf("  +----------------------------------------------+\n");
        printf("  |  Total Bills Generated  : %-18d |\n", count);
        printf("  |  Grand Total Revenue    : Rs. %13.2f |\n", grandTotal);
        printf("  |  Average Bill Amount    : Rs. %13.2f |\n",
               grandTotal / (float)count);
        printf("  +----------------------------------------------+\n");
    }
}

/* ============================================================
 *  SECTION 3 — INTERNAL HELPER FUNCTIONS (static)
 * ============================================================
 *
 * 'static' restricts these functions to this translation unit.
 * They are private implementation details invisible to all
 * other modules in the project.
 */

/* ──────────────────────────────────────────────────────────── */
/*  billIdExists()                                              */
/*  Scans bills.dat for an existing record with the given ID.  */
/*  Returns 1 (true) if a duplicate is found.                  */
/*  Returns 0 (false) if the ID is free or the file is absent. */
/* ──────────────────────────────────────────────────────────── */
static int billIdExists(int id)
{
    Bill  b;
    FILE *fp;

    fp = fopen(BILL_FILE, "rb");
    if (fp == NULL)
    {
        /*
         * File doesn't exist yet — no bills have been saved,
         * so this ID is guaranteed to be unique. Return 0.
         */
        return 0;
    }

    while (fread(&b, sizeof(Bill), 1, fp) == 1)
    {
        if (b.billId == id)
        {
            fclose(fp);
            return 1; /* Duplicate found */
        }
    }

    fclose(fp);
    return 0; /* ID is free — no duplicate detected */
}

/* ──────────────────────────────────────────────────────────── */
/*  printReceipt()                                              */
/*  Displays a formatted payment receipt for the bill that     */
/*  was just generated. Called immediately after a successful  */
/*  fwrite() so the user sees a confirmation on screen.        */
/*  Takes a const pointer — the record is not modified here.  */
/* ──────────────────────────────────────────────────────────── */
static void printReceipt(const Bill *b)
{
    printf("  +--------------------------------------+\n");
    printf("  |           PAYMENT RECEIPT            |\n");
    printf("  +--------------------------------------+\n");
    printf("  |  Bill ID            : %-14d  |\n", b->billId);
    printf("  |  Patient ID         : %-14d  |\n", b->patientId);
    printf("  +--------------------------------------+\n");
    printf("  |  Consultation Fee   : Rs. %10.2f  |\n", b->consultationFee);
    printf("  |  Medicine Fee       : Rs. %10.2f  |\n", b->medicineFee);
    printf("  +--------------------------------------+\n");
    printf("  |  TOTAL AMOUNT       : Rs. %10.2f  |\n", b->totalAmount);
    printf("  +--------------------------------------+\n\n");
}

/* ──────────────────────────────────────────────────────────── */
/*  printBillDivider()                                          */
/*  Prints a horizontal separator line for table formatting.   */
/* ──────────────────────────────────────────────────────────── */
static void printBillDivider(void)
{
    printf("  +---------+------------+------------------+------------------+------------------+\n");
}

/* ──────────────────────────────────────────────────────────── */
/*  printBillTableHeader()                                      */
/*  Prints the column header row of the bills table.           */
/* ──────────────────────────────────────────────────────────── */
static void printBillTableHeader(void)
{
    printBillDivider();
    printf("  | %-7s | %-10s | %-16s | %-16s | %-16s |\n",
           "Bill ID",
           "Patient ID",
           "Consult Fee(Rs)",
           "Medicine Fee(Rs)",
           "Total Amount(Rs)");
    printBillDivider();
}

/* ──────────────────────────────────────────────────────────── */
/*  printBillRow()                                              */
/*  Prints one Bill record as a single formatted table row.    */
/*  Uses const pointer — avoids copying the struct and marks   */
/*  the data as read-only within this function.                */
/* ──────────────────────────────────────────────────────────── */
static void printBillRow(const Bill *b)
{
    printf("  | %-7d | %-10d | %16.2f | %16.2f | %16.2f |\n",
           b->billId,
           b->patientId,
           b->consultationFee,
           b->medicineFee,
           b->totalAmount);
}

/* ──────────────────────────────────────────────────────────── */
/*  clearBillInputBuffer()                                      */
/*  Discards all characters remaining in stdin up to and       */
/*  including the next '\n' or EOF.                            */
/*                                                              */
/*  Why this is needed:                                         */
/*  scanf("%d") or scanf("%f") leaves a '\n' in stdin after    */
/*  the user presses Enter. If fgets() runs next, it reads     */
/*  that stale '\n' immediately and returns an empty string.   */
/*  Calling clearBillInputBuffer() after every scanf() call    */
/*  prevents this class of input bug entirely.                 */
/* ──────────────────────────────────────────────────────────── */
static void clearBillInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* Intentionally empty — consuming leftover characters */
    }
}