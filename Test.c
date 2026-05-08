#include <stdio.h>
#include <string.h>

#define MAX 100

struct Student {
    int id;
    char name[50];
    float marks;
};

struct Student s[MAX];
int count = 0;

// Function to add student
void addStudent() {
    if (count >= MAX) {
        printf("Database is full!\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &s[count].id);

    printf("Enter Name: ");
    scanf(" %[^\n]", s[count].name);

    printf("Enter Marks: ");
    scanf("%f", &s[count].marks);

    count++;
    printf("Student added successfully!\n");
}

// Function to display all students
void displayStudents() {
    if (count == 0) {
        printf("No records found!\n");
        return;
    }

    printf("\nID\tName\t\tMarks\n");
    printf("----------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%d\t%s\t\t%.2f\n", s[i].id, s[i].name, s[i].marks);
    }
}

// Function to search student by ID
void searchStudent() {
    int id, found = 0;

    printf("Enter ID to search: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (s[i].id == id) {
            printf("Record Found!\n");
            printf("ID: %d\nName: %s\nMarks: %.2f\n", s[i].id, s[i].name, s[i].marks);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student not found!\n");
    }
}

// Function to delete student
void deleteStudent() {
    int id, found = 0;

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (s[i].id == id) {
            for (int j = i; j < count - 1; j++) {
                s[j] = s[j + 1];
            }
            count--;
            printf("Student deleted successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student not found!\n");
    }
}

// Function to update student
void updateStudent() {
    int id, found = 0;

    printf("Enter ID to update: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (s[i].id == id) {
            printf("Enter new Name: ");
            scanf(" %[^\n]", s[i].name);

            printf("Enter new Marks: ");
            scanf("%f", &s[i].marks);

            printf("Record updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student not found!\n");
    }
}

// Function to sort students by name
void sortStudents() {
    struct Student temp;

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(s[i].name, s[j].name) > 0) {
                temp = s[i];
                s[i] = s[j];
                s[j] = temp;
            }
        }
    }

    printf("Students sorted by name successfully!\n");
}

// Main function
int main() {
    int choice;

    do {
        printf("\n--- Student Record Management ---\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Update Student\n");
        printf("6. Sort Students by Name\n");
        printf("7. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: deleteStudent(); break;
            case 5: updateStudent(); break;
            case 6: sortStudents(); break;
            case 7: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }

    } while (choice != 7);

    return 0;
}