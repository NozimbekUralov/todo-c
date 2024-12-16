#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const char TASKS_FILE[] = "tasks.csv";

int n_rows = 1, id;
char buffer[1024];

#define BUFFER_SIZE 1024

// Task strukturasini yaratamiz
typedef struct {
    char title[32];  // Vazifaning nomi
    char description[128];
    int status;       // 1 - "Bajarilgan", 0 - "Bajarilmagan"
} Task;

Task tasks[50], task;

// clear the console
void clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void add_task(); // Yangi vazifa qo'shish

void list_all_tasks(); // Barcha vazifalarni ro'yxatini ko'rsatish

void get_task_by_id(); // Vazifani ID bo'yicha ko'rsatish

void delete_task_by_id(); // Vazifani ID bo'yicha o'chirish

void mark_task_completed(); // Vazifani "Completed" deb belgilash

void mark_task_incomplete(); // Vazifani "Incomplete" deb belgilash

void show_menu(char menu[]); // Foydalanuvchiga menyuni ko'rsatish

// utility functions
void load_file();
void write_file();
void append_file();

// start the program
int main() {
    char menu[] = "start";
    int choice = 0;
    
    while (1) {

        if(strcmp(menu, "start") == 0){
            // load the file
            load_file();
            clear();
            list_all_tasks();
        }

        show_menu(menu);  

        here:
        printf("Enter your choice: "); fgets(buffer, sizeof(buffer), stdin);
        
        choice = atoi(buffer);
        if(choice == 0 || choice > 4) goto here;
        
        if(strcmp(menu, "start") == 0){
            // change menu if needed
            if(choice == 2 && n_rows -1 != 0){
                strcpy(menu, "task");
            }

            switch(choice){
                case 1:
                    add_task();
                    break;
                case 2:
                    get_task_by_id();
                    break;
                case 3:
                    delete_task_by_id();
                    break;
                case 4:
                    printf("Exiting the program...\n");
                    return 0;
                    break;
                default:
                    printf("Invalid choice. Please try again.\n");
                    break;
            }

        }else if(strcmp(menu, "task") == 0){
            switch(choice){
                case 1:
                    mark_task_completed();
                    strcpy(menu, "start");
                    break;
                case 2:
                    mark_task_incomplete();
                    strcpy(menu, "start");
                    break;
                default:
                    break;
            }
        }
    }
    
    return 0;
}


void show_menu(char menu[]){
    char start_menu[][18] = {
        "Add Task",
        "Get Task by ID",
        "Delete Task by ID",
        "Exit"
    };
    char task_menu[][20] = {
        "Mark as Completed",
        "Mark as Incompleted",
    };
    
    int start_len = sizeof(start_menu) / sizeof(start_menu[0]);
    int task_menu_len = sizeof(task_menu) / sizeof(task_menu[0]);

    printf("\nActions:");
    if(strcmp(menu, "start") == 0){
        for(int i = 0; i < start_len; i++){
            printf("| %d.%s ", i+1, start_menu[i]);
        }
        printf("|\n");
    }else if(strcmp(menu, "task") == 0){
        for(int i = 0; i < task_menu_len; i++){
            printf("| %d.%s ", i+1, task_menu[i]);
        }
        printf("|\n");
    }
}

void list_all_tasks(){
    // Print the table header
    printf("%-4s| %-16s| %-32s| %-7s\n", "ID", "Title", "Description", "Status");
    for(int i = 0; i < 64; i++){
        printf("-");
    }
    printf("\n");

    if(n_rows -1 == 0){
        printf("No tasks found.\n");
        return;
    }

    // Print the tasks
    for(int i = 1; i < n_rows; i++){
        printf("%-4d| %-16s| %-32s| %-7d\n", i, tasks[i].title, tasks[i].description, tasks[i].status);
    }
}

void add_task(){
    // get task from stdin
    printf("Enter Task Title (max: 16)"); fgets(task.title, sizeof(task.title), stdin);
    task.title[strcspn(task.title, "\n")] = 0;

    printf("Enter Task Description (max: 32)"); fgets(task.description, sizeof(task.description), stdin);
    task.description[strcspn(task.description, "\n")] = 0;

    task.status = 0;

    // write in file
    append_file();

    puts("Task added successfully!");
}

void get_task_by_id(){
    here:
    if(n_rows -1 == 0){
        puts("Try other options.");
        return;
    }

    // Get the task ID from the user
    printf("Enter task ID to get: "); fgets(buffer, sizeof(buffer), stdin);
   
    id = atoi(buffer);
    if(id == 0 || id > n_rows) goto here;

    // Clear the screen
    clear();
    
    // Print the table header
    printf("%-4s| %-16s| %-32s| %-7s\n", "ID", "Title", "Description", "Status");
    for(int i = 0; i < 64; i++){
        printf("-");
    }
    printf("\n");
    
    // Print the task
    printf("%-4d| %-16s| %-32s| %-7d\n", id, tasks[id].title, tasks[id].description, tasks[id].status);

}

void delete_task_by_id() {
    here:
    if(n_rows -1 == 0){
        return;
    }

    // Get the task ID from the user
    printf("Enter task ID to delete: "); fgets(buffer, sizeof(buffer), stdin);
    
    id = atoi(buffer);
    if(id == 0 || id > n_rows) goto here;

    // Shift elements to the left to overwrite the target index
    for (int i = id; i < n_rows; i++) {
        strcpy(tasks[i].title, tasks[i + 1].title);
        strcpy(tasks[i].description, tasks[i + 1].description);
        tasks[i].status = tasks[i + 1].status;
    }

    // Decrement the number of rows since one element is removed
    n_rows--;

    // write in file
    write_file();

    puts("\nTask deleted successfully!");
}

void mark_task_completed(){
    tasks[id].status = 1;

    // write in file
    write_file();

    // Print the task
    printf("%-4d| %-16s| %-32s| %-7d\n", id, tasks[id].title, tasks[id].description, tasks[id].status);

    puts("Task marked as Completed!");
}

void mark_task_incomplete(){
    tasks[id].status = 0;

    // write in file
    write_file();

    // Print the task
    printf("%-4d| %-16s| %-32s| %-7d\n", id, tasks[id].title, tasks[id].description, tasks[id].status);

    puts("Task marked as Incomplete!");
}

//utility functions
void write_file(){
    
    FILE *file = fopen(TASKS_FILE, "w");

    for (int i = 0; i < n_rows; i++) {
        fprintf(file, "%s,%s,%d\n",
            tasks[i].title,
            tasks[i].description,
            tasks[i].status
        );
    }
    fclose(file);

    // set the number of rows to 1
    n_rows = 1;
}

void load_file(){
    
    int i = 0;

    // load the file
    start:
    FILE *file = fopen(TASKS_FILE, "r");

    if(file == NULL){
        file = fopen(TASKS_FILE, "w");
        fprintf(file, "%s,%s,%s\n", "Title", "Description", "Status");
        fclose(file);
        goto start;
    } 

    // Buffer to store each line
    char buffer[BUFFER_SIZE];

    // Read and process each line
    while (fgets(buffer, BUFFER_SIZE, file)) {
        // Split the line into fields using ',' as a delimiter
        char *field = strtok(buffer, ",");
        while (field) {
            strncpy(tasks[i].title, field, sizeof(tasks[i].title) - 1);
            tasks[i].title[sizeof(tasks[i].title) - 1] = '\0';
            field = strtok(NULL, ",");

            strncpy(tasks[i].description, field, sizeof(tasks[i].description) - 1);
            tasks[i].description[sizeof(tasks[i].description) - 1] = '\0';
            field = strtok(NULL, ",");

            tasks[i].status = (strchr(field, '1')? 1:0);
            field = strtok(NULL, ",");

            i++;
        }
        n_rows++;
    }
    fclose(file);
    
    // decrement the number of rows since there is empty row at the end
    n_rows--;
}

void append_file(){
    // append in file
    FILE *file = fopen(TASKS_FILE, "a");
    
    fprintf(file, "%s,%s,%d\n",
        task.title,
        task.description,
        task.status
    ); fclose(file);

    // set the number of rows to 1
    n_rows = 1;
}