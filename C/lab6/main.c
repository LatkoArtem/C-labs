#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// Date of birth structure
struct DateOfBirth {
    int year;
    int month;
    int day;
};

// Gender enumeration
enum Gender {
    MALE,
    FEMALE
};

// Student information structure
struct Student {
    char *last_name;
    char *first_name;
    char *patronymic;
    struct DateOfBirth birth_data;
    char *group;
    enum Gender gender;
};

// Linked list node for students
struct StudentsList {
    struct Student data;
    struct StudentsList *next_student;
};

// Function to validate integer input
int check_for_int(char *text, int *variable) {
    printf("%s", text);
    char input[100];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(input, "%d", variable) != 1) {
        printf("Incorrect data entry\n");
        return 1;
    }
    return 0;
}

// Function to validate string input
void check_for_char(char *text, char **variable) {
    printf("%s", text);
    char input[100];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';  // Remove newline character
    } else {
        // Input too long
        printf("Input too long\n");
        exit(EXIT_FAILURE);
    }
    *variable = strdup(input);
    if (*variable == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}

// Function to print the student list
void output_list(struct StudentsList *list) {
    if (list == NULL) {
        printf("The list of students is empty\n");
        return;
    }
    struct StudentsList *current = list;
    while (current != NULL) {
        printf("%s %s, group %s\n", current->data.last_name, current->data.first_name, current->data.group);
        current = current->next_student;
    }
}

// Function to create a new student
struct Student *new_student() {
    struct Student *new_student = malloc(sizeof(struct Student));
    if (new_student == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Get input for each string
    check_for_char("Enter a last name: ", &new_student->last_name);
    check_for_char("Enter a name: ", &new_student->first_name);
    check_for_char("Enter a patronymic: ", &new_student->patronymic);
    check_for_int("Enter a day of birth: ", &new_student->birth_data.day);
    check_for_int("Enter a month of birth: ", &new_student->birth_data.month);
    check_for_int("Enter a year of birth: ", &new_student->birth_data.year);
    check_for_char("Enter the group: ", &new_student->group);
    while (true) {
        printf("Gender (0 - male, 1 - female): ");
        if (check_for_int("", (int *)&new_student->gender) != 0) {
            continue;
        }
        if (new_student->gender == MALE || new_student->gender == FEMALE) {
            break;
        }
        printf("Incorrect data entry\n");
    }
    printf("\n");
    return new_student;
}

// Function to add a new student to the list
void add_student(struct StudentsList **head_list) {
    struct Student *new_student_data = new_student();

    struct StudentsList *new_node = malloc(sizeof(struct StudentsList));
    if (new_node == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = *new_student_data;
    new_node->next_student = NULL;

    if (*head_list == NULL) {
        *head_list = new_node;
    } else {
        struct StudentsList *current_student = *head_list;
        while (current_student->next_student != NULL) {
            current_student = current_student->next_student;
        }
        current_student->next_student = new_node;
    }
    free(new_student_data);
}

// Function to delete a student from the list
void delete_student(struct StudentsList **head_list, char *last_name, char *first_name) {
    if (*head_list == NULL) {
        printf("The list of students is empty\n");
        return;
    }

    struct StudentsList *current = *head_list;
    struct StudentsList *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->data.last_name, last_name) == 0 && strcmp(current->data.first_name, first_name) == 0) {
            if (prev == NULL) {
                *head_list = current->next_student;
            } else {
                prev->next_student = current->next_student;
            }
            free(current->data.last_name);
            free(current->data.first_name);
            free(current->data.patronymic);
            free(current->data.group);
            free(current);
            printf("Student successfully removed\n");
            return;
        }
        prev = current;
        current = current->next_student;
    }

    printf("Student not found\n");
}

// Function to save data to a file
void save_data(struct StudentsList *head_list) {
    FILE *file = fopen("data.bin", "wb");
    if (file == NULL) {
        perror("Failed to open file for writing");
        return;
    }
    struct StudentsList *current = head_list;
    while (current != NULL) {
        fwrite(&current->data.birth_data, sizeof(struct DateOfBirth), 1, file);
        int last_name_len = strlen(current->data.last_name) + 1;
        int first_name_len = strlen(current->data.first_name) + 1;
        int patronymic_len = strlen(current->data.patronymic) + 1;
        int group_len = strlen(current->data.group) + 1;

        fwrite(&last_name_len, sizeof(int), 1, file);
        fwrite(current->data.last_name, sizeof(char), last_name_len, file);

        fwrite(&first_name_len, sizeof(int), 1, file);
        fwrite(current->data.first_name, sizeof(char), first_name_len, file);

        fwrite(&patronymic_len, sizeof(int), 1, file);
        fwrite(current->data.patronymic, sizeof(char), patronymic_len, file);

        fwrite(&group_len, sizeof(int), 1, file);
        fwrite(current->data.group, sizeof(char), group_len, file);

        fwrite(&current->data.gender, sizeof(enum Gender), 1, file);

        current = current->next_student;
    }

    fclose(file);
    printf("Data saved to 'data.bin'\n");
}

// Function to load data from a file
void load_data(struct StudentsList **head_list) {
    FILE *file = fopen("data.bin", "rb");
    if (file == NULL) {
        if (errno == ENOENT) {
            printf("File 'data.bin' does not exist. Starting with an empty list.\n");
            return;
        } else {
            perror("Failed to open file for reading");
            exit(EXIT_FAILURE);
        }
    }

    while (!feof(file)) {
        struct DateOfBirth birthdate;
        if (fread(&birthdate, sizeof(struct DateOfBirth), 1, file) != 1) break;

        int last_name_len, first_name_len, patronymic_len, group_len;
        fread(&last_name_len, sizeof(int), 1, file);
        char *last_name = malloc(last_name_len);
        fread(last_name, sizeof(char), last_name_len, file);
        last_name[last_name_len - 1] = '\0'; // Null-terminate the string

        fread(&first_name_len, sizeof(int), 1, file);
        char *first_name = malloc(first_name_len);
        fread(first_name, sizeof(char), first_name_len, file);
        first_name[first_name_len - 1] = '\0'; // Null-terminate the string

        fread(&patronymic_len, sizeof(int), 1, file);
        char *patronymic = malloc(patronymic_len);
        fread(patronymic, sizeof(char), patronymic_len, file);
        patronymic[patronymic_len - 1] = '\0'; // Null-terminate the string

        fread(&group_len, sizeof(int), 1, file);
        char *group = malloc(group_len);
        fread(group, sizeof(char), group_len, file);
        group[group_len - 1] = '\0'; // Null-terminate the string

        enum Gender gender;
        fread(&gender, sizeof(enum Gender), 1, file);

        struct Student *new_student_data = malloc(sizeof(struct Student));
        if (new_student_data == NULL) {
            printf("Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        new_student_data->last_name = last_name;
        new_student_data->first_name = first_name;
        new_student_data->patronymic = patronymic;
        new_student_data->group = group;
        new_student_data->gender = gender;

        struct StudentsList *new_node = malloc(sizeof(struct StudentsList));
        if (new_node == NULL) {
            printf("Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        new_node->data = *new_student_data;
        new_node->next_student = NULL;

        if (*head_list == NULL) {
            *head_list = new_node;
        } else {
            struct StudentsList *current_student = *head_list;
            while (current_student->next_student != NULL) {
                current_student = current_student->next_student;
            }
            current_student->next_student = new_node;
        }
        free(new_student_data);
    }

    fclose(file);
    printf("Data loaded from 'data.bin'\n");
}

// Function to clean up memory used by the student list
void cleanup_list(struct StudentsList *head_list) {
    struct StudentsList *current = head_list;
    while (current != NULL) {
        struct StudentsList *temp = current;
        current = current->next_student;
        free(temp->data.last_name);
        free(temp->data.first_name);
        free(temp->data.patronymic);
        free(temp->data.group);
        free(temp);
    }
}

int main() {
    struct StudentsList *head_list = NULL;
    load_data(&head_list);

    printf("1. Display the list of students\n");
    printf("2. Add a new student\n");
    printf("3. Delete a student\n");
    printf("0. EXIT\n\n");

    int choice;
    while (true) {
        check_for_int("Enter your choice(1, 2, 3 or 0): ", &choice);
        if (choice == 1) {
            printf("\n");
            output_list(head_list);
            printf("\n");
        } else if (choice == 2) {
            add_student(&head_list);
        } else if (choice == 3) {
            char *last_name_del, *first_name_del;
            check_for_char("Enter the last name of the student you want to remove: ", &last_name_del);
            check_for_char("Enter the name of the student you want to remove: ", &first_name_del);
            delete_student(&head_list, last_name_del, first_name_del);
            free(last_name_del);
            free(first_name_del);
        } else if (choice == 0) {
            save_data(head_list);
            printf("\nEXIT!!!\n");
            break;
        } else {
            printf("Incorrect data entry\n");
        }
    }
    cleanup_list(head_list);
    return 0;
}