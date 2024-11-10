#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "r_5_lirary.h"

void displayReaderMenu()
{
    printf("\nReader Menu:\n");
    printf("1. Borrow a book\n");
    printf("2. Return a book\n");
    printf("3. Search for a book\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
}

void displayAdminMenu()
{
    printf("\nAdmin Menu:\n");
    printf("1. Add a book\n");
    printf("2. Delete a book\n");
    printf("3. Add a reader\n");
    printf("4. Delete a reader\n");
    printf("5. Modify book quantity\n");
    printf("6. Search for a book\n");
    printf("7. Exit\n");
    printf("Enter your choice: ");
}

int checkAdminCredentials(const char *username, const char *password)
{
    FILE *file;

    file = fopen("admin.txt", "r");

    if (file == NULL)
    {
        printf("无法打开管理员数据表\n");
        return 0;
    }

    char line[100];
    int found = 0;

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0';

        // 使用空格分割用户名和密码
        char *savedUsername = strtok(line, " ");
        char *savedPassword = strtok(NULL, " ");

        if (savedUsername != NULL && savedPassword != NULL)
        {
            if (strcmp(savedUsername, username) == 0 && strcmp(savedPassword, password) == 0)
            {
                found = 1;
                break;
            }
        }
    }

    fclose(file);

    return found;
}

int checkReaderCredentials(struct Reader *reader, const char *username, const char *password)
{
    FILE *readerFile = fopen("reader.txt", "r"); // 打开读者文件

    if (readerFile == NULL)
    {
        printf("无法打开读者文件。\n");
        return 0;
    }

    char storedUsername[100];
    char storedPassword[100];

    // 在reader.txt文件中查找对应用户名和密码的读者信息
    while (fscanf(readerFile, "%d %s %s %d", &reader->id, reader->password, storedUsername, &reader->borrowedBookId) == 4)
    {
        if (strcmp(storedUsername, username) == 0 && strcmp(reader->password, password) == 0)
        {
            strcpy(reader->name, storedUsername);
            fclose(readerFile);
            return 1;
        }
    }

    fclose(readerFile);

    // 未找到对应用户名和密码的读者信息
    reader->id = 0;
    strcpy(reader->password, "");
    strcpy(reader->name, "");
    reader->borrowedBookId = 0;

    return 0;
}

void borrowBook(struct Reader *reader)
{
    // 检查是否有在借书目
    if (reader->borrowedBookId != 0)
    {
        printf("有在借书目，请归还后再申请借书。\n");
        return;
    }

    int bookId;
    printf("请输入要借阅的书目ID：");
    scanf("%d", &bookId);

    FILE *bookFile = fopen("book.txt", "r"); // 打开书目文件

    if (bookFile == NULL)
    {
        printf("无法打开书目文件。\n");
        return;
    }

    FILE *readerFile = fopen("reader.txt", "r"); // 打开读者文件

    if (readerFile == NULL)
    {
        printf("无法打开读者文件。\n");
        fclose(bookFile);
        return;
    }

    FILE *tempReaderFile = fopen("tempReader.txt", "w"); // 创建临时读者文件

    if (tempReaderFile == NULL)
    {
        printf("无法创建临时读者文件。\n");
        fclose(bookFile);
        fclose(readerFile);
        return;
    }

    char bookLine[400]; // 用于存储每行书目数据的缓冲区
    int foundBook = 0;  // 标记是否找到对应的书目ID

    // 在book.txt文件中查找对应的书目ID
    while (fgets(bookLine, sizeof(bookLine), bookFile))
    {
        int id;
        char title[100];
        int quantity;

        // 使用sscanf解析每行数据
        if (sscanf(bookLine, "%d %s %d", &id, title, &quantity) != 3)
        {
            printf("书目文件格式错误。\n");
            fclose(bookFile);
            fclose(readerFile);
            fclose(tempReaderFile);
            remove("tempReader.txt"); // 删除临时读者文件
            return;
        }

        if (id == bookId)
        {
            foundBook = 1;
        }
    }

    fclose(bookFile);

    if (!foundBook)
    {
        printf("未找到对应的书目ID。\n");
        fclose(readerFile);
        fclose(tempReaderFile);
        remove("tempReader.txt"); // 删除临时读者文件
        return;
    }

    char readerLine[400]; // 用于存储每行读者数据的缓冲区
    int foundReader = 0;  // 标记是否找到对应的读者ID

    // 在reader.txt文件中查找对应的读者ID并修改borrowedBookId
    while (fgets(readerLine, sizeof(readerLine), readerFile))
    {
        int readerId;
        char password[100];
        char name[100];
        int borrowedBookId;

        // 使用sscanf解析每行数据
        if (sscanf(readerLine, "%d %s %s %d", &readerId, password, name, &borrowedBookId) != 4)
        {
            printf("读者文件格式错误。\n");
            fclose(readerFile);
            fclose(tempReaderFile);
            remove("tempReader.txt"); // 删除临时读者文件
            return;
        }

        if (readerId == reader->id)
        {
            foundReader = 1;
            // 将修改后的读者信息写入临时读者文件
            fprintf(tempReaderFile, "%d %s %s %d\n", reader->id, reader->password, reader->name, bookId);
        }
        else
        {
            // 将未修改的读者信息写入临时读者文件
            fprintf(tempReaderFile, "%d %s %s %d\n", readerId, password, name, borrowedBookId);
        }
    }

    fclose(readerFile);
    fclose(tempReaderFile);

    if (!foundReader)
    {
        printf("未找到对应的读者ID。\n");
        remove("tempReader.txt"); // 删除临时读者文件
        return;
    }

    // 删除原读者文件
    remove("reader.txt");

    // 重命名临时读者文件为读者文件
    rename("tempReader.txt", "reader.txt");

    // 更新reader结构体信息
    reader->borrowedBookId = bookId;

    printf("借阅成功。\n");
}

void returnBook(struct Reader *reader)
{
    // 检查是否有在借书目
    if (reader->borrowedBookId == 0)
    {
        printf("无在借书目或书目已被删除。\n");
        return;
    }

    FILE *readerFile = fopen("reader.txt", "r"); // 打开读者文件

    if (readerFile == NULL)
    {
        printf("无法打开读者文件。\n");
        return;
    }

    FILE *tempReaderFile = fopen("tempReader.txt", "w"); // 创建临时读者文件

    if (tempReaderFile == NULL)
    {
        printf("无法创建临时读者文件。\n");
        fclose(readerFile);
        return;
    }

    char readerLine[400]; // 用于存储每行读者数据的缓冲区
    int foundReader = 0;  // 标记是否找到对应的读者ID

    // 在reader.txt文件中查找对应的读者ID并修改borrowedBookId
    while (fgets(readerLine, sizeof(readerLine), readerFile))
    {
        int readerId;
        char password[100];
        char name[100];
        int borrowedBookId;

        // 使用sscanf解析每行数据
        if (sscanf(readerLine, "%d %s %s %d", &readerId, password, name, &borrowedBookId) != 4)
        {
            printf("读者文件格式错误。\n");
            fclose(readerFile);
            fclose(tempReaderFile);
            remove("tempReader.txt"); // 删除临时读者文件
            return;
        }

        if (readerId == reader->id)
        {
            foundReader = 1;
            // 修改borrowedBookId为0
            borrowedBookId = 0;
            // 将修改后的读者信息写入临时读者文件
            fprintf(tempReaderFile, "%d %s %s %d\n", readerId, password, name, borrowedBookId);
        }
        else
        {
            // 将未修改的读者信息写入临时读者文件
            fprintf(tempReaderFile, "%d %s %s %d\n", readerId, password, name, borrowedBookId);
        }
    }

    fclose(readerFile);
    fclose(tempReaderFile);

    if (!foundReader)
    {
        printf("未找到对应的读者ID。\n");
        remove("tempReader.txt"); // 删除临时读者文件
        return;
    }

    // 删除原读者文件
    remove("reader.txt");

    // 重命名临时读者文件为读者文件
    rename("tempReader.txt", "reader.txt");

    // 更新reader结构体信息
    reader->borrowedBookId = 0;

    printf("归还成功。\n");
}

void searchBook()
{
    FILE *bookFile = fopen("book.txt", "r"); // 打开书目文件

    if (bookFile == NULL)
    {
        printf("无法打开书目文件。\n");
        return;
    }

    char searchOption[10];
    printf("请选择检索方式，id或书名（I/n）：");
    scanf("%s", searchOption);

    // 根据用户选择的检索方式，请求相应的输入
    if (strcmp(searchOption, "i") == 0 || strcmp(searchOption, "I") == 0)
    {
        int searchId;
        printf("请输入书目ID：");
        scanf("%d", &searchId);

        struct Book book;

        // 在book.txt文件中查找满足条件的书目ID
        while (fscanf(bookFile, "%d %s %d", &book.id, book.title, &book.quantity) == 3)
        {
            if (book.id == searchId)
            {
                printf("ID: %d, 标题: %s, 数量: %d\n", book.id, book.title, book.quantity);
            }
        }
    }
    else if (strcmp(searchOption, "n") == 0 || strcmp(searchOption, "N") == 0)
    {
        char searchTitle[100];
        printf("请输入书目名称：");
        scanf(" %[^\n]", searchTitle);

        struct Book book;

        // 在book.txt文件中查找满足条件的书目名称
        while (fscanf(bookFile, "%d %s %d", &book.id, book.title, &book.quantity) == 3)
        {
            if (strcmp(book.title, searchTitle) == 0)
            {
                printf("ID: %d, 标题: %s, 数量: %d\n", book.id, book.title, book.quantity);
            }
        }
    }
    else
    {
        printf("无效的检索方式。\n");
    }

    fclose(bookFile);
}

void addBook()
{
    int bookId;
    char title[100];
    int quantity;

    printf("请输入书目ID：");
    scanf("%d", &bookId);

    if (bookId == 0)
    {
        printf("书目ID禁止为0\n");
        return;
    }

    FILE *file = fopen("book.txt", "r");

    if (file == NULL)
    {
        printf("无法打开书目文件。\n");
        return;
    }

    char line[400];

    while (fgets(line, sizeof(line), file))
    {
        int id;
        char storedTitle[100];
        int storedQuantity;

        if (sscanf(line, "%d %s %d", &id, storedTitle, &storedQuantity) != 3)
        {
            printf("书目文件格式错误。\n");
            fclose(file);
            return;
        }

        if (id == bookId)
        {
            printf("已存在相同的书目ID。\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    printf("请输入书名：");
    fflush(stdin);
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';

    printf("请输入数量：");
    scanf("%d", &quantity);

    file = fopen("book.txt", "a");

    if (file == NULL)
    {
        printf("无法打开书目文件。\n");
        return;
    }

    fprintf(file, "%d %s %d\n", bookId, title, quantity);

    fclose(file);

    printf("书目添加成功。\n");
}

void deleteBook()
{
    int id;

    printf("请输入要删除的书目ID：");
    scanf("%d", &id);

    FILE *bookFile = fopen("book.txt", "r");
    FILE *tempBookFile = fopen("tempBook.txt", "w");
    FILE *readerFile = fopen("reader.txt", "r");
    FILE *tempReaderFile = fopen("tempReader.txt", "w");

    if (bookFile == NULL || tempBookFile == NULL || readerFile == NULL || tempReaderFile == NULL)
    {
        printf("无法打开文件。\n");
        return;
    }

    char line[400];

    // 遍历book.txt文件，将不是要删除的书目写入tempBook.txt
    int flag=0;
    while (fgets(line, sizeof(line), bookFile))
    {
        int existingId;
        char title[100];
        int quantity;

        if (sscanf(line, "%d %s %d", &existingId, title, &quantity) != 3)
        {
            printf("书目文件格式错误。\n");
            fclose(bookFile);
            fclose(tempBookFile);
            fclose(readerFile);
            fclose(tempReaderFile);
            return;
        }

        if (existingId != id)
        {
            fprintf(tempBookFile, "%d %s %d\n", existingId, title, quantity);
            
        }
        else{
            flag=1;
            printf("书目删除成功。\n");
        }
    }

    if(!flag) printf("未找到该书目\n");

    // 更新reader.txt文件中borrowedBookId为对应id的值为0
    while (fgets(line, sizeof(line), readerFile))
    {
        int existingId;
        char password[100];
        char name[100];
        int borrowedBookId;

        if (sscanf(line, "%d %s %s %d", &existingId, password, name, &borrowedBookId) != 4)
        {
            printf("读者文件格式错误。\n");
            fclose(bookFile);
            fclose(tempBookFile);
            fclose(readerFile);
            fclose(tempReaderFile);
            return;
        }

        if (borrowedBookId == id)
        {
            borrowedBookId = 0;
        }

        fprintf(tempReaderFile, "%d %s %s %d\n", existingId, password, name, borrowedBookId);
    }

    fclose(bookFile);
    fclose(tempBookFile);
    fclose(readerFile);
    fclose(tempReaderFile);

    // 删除原始的book.txt和reader.txt文件
    if (remove("book.txt") != 0 || remove("reader.txt") != 0)
    {
        printf("无法删除文件。\n");
        return;
    }

    // 重命名临时文件为book.txt和reader.txt
    if (rename("tempBook.txt", "book.txt") != 0 || rename("tempReader.txt", "reader.txt") != 0)
    {
        printf("无法重命名文件。\n");
        return;
    }
}

void addReader()
{
    int id;
    char name[100];
    char password[100];

    printf("请输入新建读者的ID：");
    scanf("%d", &id);

    printf("请输入新建读者的名称：");
    scanf("%s", name);

    FILE *readerFile = fopen("reader.txt", "r+"); // 打开读者文件

    if (readerFile == NULL)
    {
        printf("无法打开读者文件。\n");
        return;
    }

    char line[400]; // 用于存储每行数据的缓冲区

    // 检查读者ID和名称是否已存在
    while (fgets(line, sizeof(line), readerFile))
    {
        int existingId;
        char existingPassword[100];
        char existingName[100];
        int existingBorrowedBookId;

        // 使用sscanf解析每行数据
        if (sscanf(line, "%d %s %s %d", &existingId, existingPassword, existingName, &existingBorrowedBookId) != 4)
        {
            printf("读者文件格式错误。\n");
            fclose(readerFile);
            return;
        }

        if (existingId == id || strcmp(existingName, name) == 0)
        {
            printf("读者ID或名称已存在。\n");
            fclose(readerFile);
            return;
        }
    }

    printf("请输入新建读者的密码：");
    scanf("%s", password);
    // 将新读者的信息写入reader.txt文件
    fseek(readerFile, 0, SEEK_END);                          // 将文件指针移至文件末尾
    fprintf(readerFile, "%d %s %s 0\n", id, password, name); // 写入新读者的信息

    fclose(readerFile);

    printf("读者添加成功。\n");
}

void deleteReader()
{
    int id;

    printf("请输入要删除的读者ID：");
    scanf("%d", &id);

    FILE *readerFile = fopen("reader.txt", "r");
    FILE *tempReaderFile = fopen("tempReader.txt", "w");

    if (readerFile == NULL || tempReaderFile == NULL)
    {
        printf("无法打开文件。\n");
        return;
    }

    char line[400];
    int found = 0;

    // 遍历reader.txt文件，将不是要删除的读者写入tempReader.txt
    while (fgets(line, sizeof(line), readerFile))
    {
        int existingId;
        char password[100];
        char name[100];
        int borrowedBookId;

        if (sscanf(line, "%d %s %s %d", &existingId, password, name, &borrowedBookId) != 4)
        {
            printf("读者文件格式错误。\n");
            fclose(readerFile);
            fclose(tempReaderFile);
            return;
        }

        if (existingId != id)
        {
            fprintf(tempReaderFile, "%d %s %s %d\n", existingId, password, name, borrowedBookId);
        }
        else
        {
            found = 1;
        }
    }

    fclose(readerFile);
    fclose(tempReaderFile);

    // 删除原始的reader.txt文件
    if (remove("reader.txt") != 0)
    {
        printf("无法删除文件。\n");
        return;
    }

    // 重命名临时文件为reader.txt
    if (rename("tempReader.txt", "reader.txt") != 0)
    {
        printf("无法重命名文件。\n");
        return;
    }

    if (found)
    {
        printf("读者删除成功。\n");
    }
    else
    {
        printf("未找到要删除的读者ID。\n");
    }
}

void modifyBookQuantity()
{
    FILE *file = fopen("book.txt", "r+");
    if (file == NULL)
    {
        printf("无法打开文件。\n");
        return;
    }

    int targetId;
    printf("请输入要修改的bookId: ");
    scanf("%d", &targetId);

    struct Book book;
    char line[256];
    int found = 0;

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        printf("无法创建临时文件。\n");
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "%d %s %d", &book.id, book.title, &book.quantity);

        if (book.id == targetId)
        {
            printf("请输入新的数量值: ");
            scanf("%d", &book.quantity);

            found = 1;
            fprintf(tempFile, "%d %s %d\n", book.id, book.title, book.quantity);
        }
        else
        {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("book.txt");
    rename("temp.txt", "book.txt");

    if (!found)
    {
        printf("bookId不存在。\n");
    }
    else
    {
        printf("更新数量信息成功！\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid command line arguments!\n");
        return 1;
    }

    char *userType = argv[1];
    char *userName = argv[2];

    if (strcmp(userType, "-a") == 0)
    {
        // Admin user
        char password[100];
        printf("Enter your password: ");
        scanf("%s", password);

        int isAdmin = checkAdminCredentials(userName, password);
        if (!isAdmin)
        {
            printf("Invalid username or password for admin!\n");
            return 1;
        }

        while (1)
        {
            displayAdminMenu();

            int choice;
            scanf("%d", &choice);

            switch (choice)
            {
            case 1:
                addBook();
                break;
            case 2:
                deleteBook();
                break;
            case 3:
                addReader();
                break;
            case 4:
                deleteReader();
                break;
            case 5:
                modifyBookQuantity();
                break;
            case 6:
                searchBook();
                break;
            case 7:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
            }
        }
    }
    else if (strcmp(userType, "-u") == 0)
    {
        // Reader user
        char password[100];
        printf("Enter your password: ");
        scanf("%s", password);

        struct Reader reader;
        int isReader = checkReaderCredentials(&reader, userName, password);
        if (!isReader)
        {
            printf("Invalid username or password for reader!\n");
            return 1;
        }

        while (1)
        {
            displayReaderMenu();

            int choice;
            scanf("%d", &choice);

            switch (choice)
            {
            case 1:
                borrowBook(&reader); // Assuming only one reader for simplicity
                break;
            case 2:
                returnBook(&reader); // Assuming only one reader for simplicity
                break;
            case 3:
                searchBook();
                break;
            case 4:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
            }
        }
    }
    else
    {
        printf("Invalid user type!\n");
        return 1;
    }

    return 0;
}
