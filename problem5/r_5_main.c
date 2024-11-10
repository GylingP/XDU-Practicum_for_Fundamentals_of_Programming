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
        printf("�޷��򿪹���Ա���ݱ�\n");
        return 0;
    }

    char line[100];
    int found = 0;

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0';

        // ʹ�ÿո�ָ��û���������
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
    FILE *readerFile = fopen("reader.txt", "r"); // �򿪶����ļ�

    if (readerFile == NULL)
    {
        printf("�޷��򿪶����ļ���\n");
        return 0;
    }

    char storedUsername[100];
    char storedPassword[100];

    // ��reader.txt�ļ��в��Ҷ�Ӧ�û���������Ķ�����Ϣ
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

    // δ�ҵ���Ӧ�û���������Ķ�����Ϣ
    reader->id = 0;
    strcpy(reader->password, "");
    strcpy(reader->name, "");
    reader->borrowedBookId = 0;

    return 0;
}

void borrowBook(struct Reader *reader)
{
    // ����Ƿ����ڽ���Ŀ
    if (reader->borrowedBookId != 0)
    {
        printf("���ڽ���Ŀ����黹����������顣\n");
        return;
    }

    int bookId;
    printf("������Ҫ���ĵ���ĿID��");
    scanf("%d", &bookId);

    FILE *bookFile = fopen("book.txt", "r"); // ����Ŀ�ļ�

    if (bookFile == NULL)
    {
        printf("�޷�����Ŀ�ļ���\n");
        return;
    }

    FILE *readerFile = fopen("reader.txt", "r"); // �򿪶����ļ�

    if (readerFile == NULL)
    {
        printf("�޷��򿪶����ļ���\n");
        fclose(bookFile);
        return;
    }

    FILE *tempReaderFile = fopen("tempReader.txt", "w"); // ������ʱ�����ļ�

    if (tempReaderFile == NULL)
    {
        printf("�޷�������ʱ�����ļ���\n");
        fclose(bookFile);
        fclose(readerFile);
        return;
    }

    char bookLine[400]; // ���ڴ洢ÿ����Ŀ���ݵĻ�����
    int foundBook = 0;  // ����Ƿ��ҵ���Ӧ����ĿID

    // ��book.txt�ļ��в��Ҷ�Ӧ����ĿID
    while (fgets(bookLine, sizeof(bookLine), bookFile))
    {
        int id;
        char title[100];
        int quantity;

        // ʹ��sscanf����ÿ������
        if (sscanf(bookLine, "%d %s %d", &id, title, &quantity) != 3)
        {
            printf("��Ŀ�ļ���ʽ����\n");
            fclose(bookFile);
            fclose(readerFile);
            fclose(tempReaderFile);
            remove("tempReader.txt"); // ɾ����ʱ�����ļ�
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
        printf("δ�ҵ���Ӧ����ĿID��\n");
        fclose(readerFile);
        fclose(tempReaderFile);
        remove("tempReader.txt"); // ɾ����ʱ�����ļ�
        return;
    }

    char readerLine[400]; // ���ڴ洢ÿ�ж������ݵĻ�����
    int foundReader = 0;  // ����Ƿ��ҵ���Ӧ�Ķ���ID

    // ��reader.txt�ļ��в��Ҷ�Ӧ�Ķ���ID���޸�borrowedBookId
    while (fgets(readerLine, sizeof(readerLine), readerFile))
    {
        int readerId;
        char password[100];
        char name[100];
        int borrowedBookId;

        // ʹ��sscanf����ÿ������
        if (sscanf(readerLine, "%d %s %s %d", &readerId, password, name, &borrowedBookId) != 4)
        {
            printf("�����ļ���ʽ����\n");
            fclose(readerFile);
            fclose(tempReaderFile);
            remove("tempReader.txt"); // ɾ����ʱ�����ļ�
            return;
        }

        if (readerId == reader->id)
        {
            foundReader = 1;
            // ���޸ĺ�Ķ�����Ϣд����ʱ�����ļ�
            fprintf(tempReaderFile, "%d %s %s %d\n", reader->id, reader->password, reader->name, bookId);
        }
        else
        {
            // ��δ�޸ĵĶ�����Ϣд����ʱ�����ļ�
            fprintf(tempReaderFile, "%d %s %s %d\n", readerId, password, name, borrowedBookId);
        }
    }

    fclose(readerFile);
    fclose(tempReaderFile);

    if (!foundReader)
    {
        printf("δ�ҵ���Ӧ�Ķ���ID��\n");
        remove("tempReader.txt"); // ɾ����ʱ�����ļ�
        return;
    }

    // ɾ��ԭ�����ļ�
    remove("reader.txt");

    // ��������ʱ�����ļ�Ϊ�����ļ�
    rename("tempReader.txt", "reader.txt");

    // ����reader�ṹ����Ϣ
    reader->borrowedBookId = bookId;

    printf("���ĳɹ���\n");
}

void returnBook(struct Reader *reader)
{
    // ����Ƿ����ڽ���Ŀ
    if (reader->borrowedBookId == 0)
    {
        printf("���ڽ���Ŀ����Ŀ�ѱ�ɾ����\n");
        return;
    }

    FILE *readerFile = fopen("reader.txt", "r"); // �򿪶����ļ�

    if (readerFile == NULL)
    {
        printf("�޷��򿪶����ļ���\n");
        return;
    }

    FILE *tempReaderFile = fopen("tempReader.txt", "w"); // ������ʱ�����ļ�

    if (tempReaderFile == NULL)
    {
        printf("�޷�������ʱ�����ļ���\n");
        fclose(readerFile);
        return;
    }

    char readerLine[400]; // ���ڴ洢ÿ�ж������ݵĻ�����
    int foundReader = 0;  // ����Ƿ��ҵ���Ӧ�Ķ���ID

    // ��reader.txt�ļ��в��Ҷ�Ӧ�Ķ���ID���޸�borrowedBookId
    while (fgets(readerLine, sizeof(readerLine), readerFile))
    {
        int readerId;
        char password[100];
        char name[100];
        int borrowedBookId;

        // ʹ��sscanf����ÿ������
        if (sscanf(readerLine, "%d %s %s %d", &readerId, password, name, &borrowedBookId) != 4)
        {
            printf("�����ļ���ʽ����\n");
            fclose(readerFile);
            fclose(tempReaderFile);
            remove("tempReader.txt"); // ɾ����ʱ�����ļ�
            return;
        }

        if (readerId == reader->id)
        {
            foundReader = 1;
            // �޸�borrowedBookIdΪ0
            borrowedBookId = 0;
            // ���޸ĺ�Ķ�����Ϣд����ʱ�����ļ�
            fprintf(tempReaderFile, "%d %s %s %d\n", readerId, password, name, borrowedBookId);
        }
        else
        {
            // ��δ�޸ĵĶ�����Ϣд����ʱ�����ļ�
            fprintf(tempReaderFile, "%d %s %s %d\n", readerId, password, name, borrowedBookId);
        }
    }

    fclose(readerFile);
    fclose(tempReaderFile);

    if (!foundReader)
    {
        printf("δ�ҵ���Ӧ�Ķ���ID��\n");
        remove("tempReader.txt"); // ɾ����ʱ�����ļ�
        return;
    }

    // ɾ��ԭ�����ļ�
    remove("reader.txt");

    // ��������ʱ�����ļ�Ϊ�����ļ�
    rename("tempReader.txt", "reader.txt");

    // ����reader�ṹ����Ϣ
    reader->borrowedBookId = 0;

    printf("�黹�ɹ���\n");
}

void searchBook()
{
    FILE *bookFile = fopen("book.txt", "r"); // ����Ŀ�ļ�

    if (bookFile == NULL)
    {
        printf("�޷�����Ŀ�ļ���\n");
        return;
    }

    char searchOption[10];
    printf("��ѡ�������ʽ��id��������I/n����");
    scanf("%s", searchOption);

    // �����û�ѡ��ļ�����ʽ��������Ӧ������
    if (strcmp(searchOption, "i") == 0 || strcmp(searchOption, "I") == 0)
    {
        int searchId;
        printf("��������ĿID��");
        scanf("%d", &searchId);

        struct Book book;

        // ��book.txt�ļ��в���������������ĿID
        while (fscanf(bookFile, "%d %s %d", &book.id, book.title, &book.quantity) == 3)
        {
            if (book.id == searchId)
            {
                printf("ID: %d, ����: %s, ����: %d\n", book.id, book.title, book.quantity);
            }
        }
    }
    else if (strcmp(searchOption, "n") == 0 || strcmp(searchOption, "N") == 0)
    {
        char searchTitle[100];
        printf("��������Ŀ���ƣ�");
        scanf(" %[^\n]", searchTitle);

        struct Book book;

        // ��book.txt�ļ��в���������������Ŀ����
        while (fscanf(bookFile, "%d %s %d", &book.id, book.title, &book.quantity) == 3)
        {
            if (strcmp(book.title, searchTitle) == 0)
            {
                printf("ID: %d, ����: %s, ����: %d\n", book.id, book.title, book.quantity);
            }
        }
    }
    else
    {
        printf("��Ч�ļ�����ʽ��\n");
    }

    fclose(bookFile);
}

void addBook()
{
    int bookId;
    char title[100];
    int quantity;

    printf("��������ĿID��");
    scanf("%d", &bookId);

    if (bookId == 0)
    {
        printf("��ĿID��ֹΪ0\n");
        return;
    }

    FILE *file = fopen("book.txt", "r");

    if (file == NULL)
    {
        printf("�޷�����Ŀ�ļ���\n");
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
            printf("��Ŀ�ļ���ʽ����\n");
            fclose(file);
            return;
        }

        if (id == bookId)
        {
            printf("�Ѵ�����ͬ����ĿID��\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    printf("������������");
    fflush(stdin);
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';

    printf("������������");
    scanf("%d", &quantity);

    file = fopen("book.txt", "a");

    if (file == NULL)
    {
        printf("�޷�����Ŀ�ļ���\n");
        return;
    }

    fprintf(file, "%d %s %d\n", bookId, title, quantity);

    fclose(file);

    printf("��Ŀ��ӳɹ���\n");
}

void deleteBook()
{
    int id;

    printf("������Ҫɾ������ĿID��");
    scanf("%d", &id);

    FILE *bookFile = fopen("book.txt", "r");
    FILE *tempBookFile = fopen("tempBook.txt", "w");
    FILE *readerFile = fopen("reader.txt", "r");
    FILE *tempReaderFile = fopen("tempReader.txt", "w");

    if (bookFile == NULL || tempBookFile == NULL || readerFile == NULL || tempReaderFile == NULL)
    {
        printf("�޷����ļ���\n");
        return;
    }

    char line[400];

    // ����book.txt�ļ���������Ҫɾ������Ŀд��tempBook.txt
    int flag=0;
    while (fgets(line, sizeof(line), bookFile))
    {
        int existingId;
        char title[100];
        int quantity;

        if (sscanf(line, "%d %s %d", &existingId, title, &quantity) != 3)
        {
            printf("��Ŀ�ļ���ʽ����\n");
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
            printf("��Ŀɾ���ɹ���\n");
        }
    }

    if(!flag) printf("δ�ҵ�����Ŀ\n");

    // ����reader.txt�ļ���borrowedBookIdΪ��Ӧid��ֵΪ0
    while (fgets(line, sizeof(line), readerFile))
    {
        int existingId;
        char password[100];
        char name[100];
        int borrowedBookId;

        if (sscanf(line, "%d %s %s %d", &existingId, password, name, &borrowedBookId) != 4)
        {
            printf("�����ļ���ʽ����\n");
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

    // ɾ��ԭʼ��book.txt��reader.txt�ļ�
    if (remove("book.txt") != 0 || remove("reader.txt") != 0)
    {
        printf("�޷�ɾ���ļ���\n");
        return;
    }

    // ��������ʱ�ļ�Ϊbook.txt��reader.txt
    if (rename("tempBook.txt", "book.txt") != 0 || rename("tempReader.txt", "reader.txt") != 0)
    {
        printf("�޷��������ļ���\n");
        return;
    }
}

void addReader()
{
    int id;
    char name[100];
    char password[100];

    printf("�������½����ߵ�ID��");
    scanf("%d", &id);

    printf("�������½����ߵ����ƣ�");
    scanf("%s", name);

    FILE *readerFile = fopen("reader.txt", "r+"); // �򿪶����ļ�

    if (readerFile == NULL)
    {
        printf("�޷��򿪶����ļ���\n");
        return;
    }

    char line[400]; // ���ڴ洢ÿ�����ݵĻ�����

    // ������ID�������Ƿ��Ѵ���
    while (fgets(line, sizeof(line), readerFile))
    {
        int existingId;
        char existingPassword[100];
        char existingName[100];
        int existingBorrowedBookId;

        // ʹ��sscanf����ÿ������
        if (sscanf(line, "%d %s %s %d", &existingId, existingPassword, existingName, &existingBorrowedBookId) != 4)
        {
            printf("�����ļ���ʽ����\n");
            fclose(readerFile);
            return;
        }

        if (existingId == id || strcmp(existingName, name) == 0)
        {
            printf("����ID�������Ѵ��ڡ�\n");
            fclose(readerFile);
            return;
        }
    }

    printf("�������½����ߵ����룺");
    scanf("%s", password);
    // ���¶��ߵ���Ϣд��reader.txt�ļ�
    fseek(readerFile, 0, SEEK_END);                          // ���ļ�ָ�������ļ�ĩβ
    fprintf(readerFile, "%d %s %s 0\n", id, password, name); // д���¶��ߵ���Ϣ

    fclose(readerFile);

    printf("������ӳɹ���\n");
}

void deleteReader()
{
    int id;

    printf("������Ҫɾ���Ķ���ID��");
    scanf("%d", &id);

    FILE *readerFile = fopen("reader.txt", "r");
    FILE *tempReaderFile = fopen("tempReader.txt", "w");

    if (readerFile == NULL || tempReaderFile == NULL)
    {
        printf("�޷����ļ���\n");
        return;
    }

    char line[400];
    int found = 0;

    // ����reader.txt�ļ���������Ҫɾ���Ķ���д��tempReader.txt
    while (fgets(line, sizeof(line), readerFile))
    {
        int existingId;
        char password[100];
        char name[100];
        int borrowedBookId;

        if (sscanf(line, "%d %s %s %d", &existingId, password, name, &borrowedBookId) != 4)
        {
            printf("�����ļ���ʽ����\n");
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

    // ɾ��ԭʼ��reader.txt�ļ�
    if (remove("reader.txt") != 0)
    {
        printf("�޷�ɾ���ļ���\n");
        return;
    }

    // ��������ʱ�ļ�Ϊreader.txt
    if (rename("tempReader.txt", "reader.txt") != 0)
    {
        printf("�޷��������ļ���\n");
        return;
    }

    if (found)
    {
        printf("����ɾ���ɹ���\n");
    }
    else
    {
        printf("δ�ҵ�Ҫɾ���Ķ���ID��\n");
    }
}

void modifyBookQuantity()
{
    FILE *file = fopen("book.txt", "r+");
    if (file == NULL)
    {
        printf("�޷����ļ���\n");
        return;
    }

    int targetId;
    printf("������Ҫ�޸ĵ�bookId: ");
    scanf("%d", &targetId);

    struct Book book;
    char line[256];
    int found = 0;

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        printf("�޷�������ʱ�ļ���\n");
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "%d %s %d", &book.id, book.title, &book.quantity);

        if (book.id == targetId)
        {
            printf("�������µ�����ֵ: ");
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
        printf("bookId�����ڡ�\n");
    }
    else
    {
        printf("����������Ϣ�ɹ���\n");
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
