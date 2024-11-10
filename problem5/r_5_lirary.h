#define MAX_BOOKS 100
#define MAX_READERS 100
#define MAX_ADMIN 10

struct Book
{
    int id;
    char title[100];
    int quantity;
};

struct Reader
{
    int id;
    char password[100];
    char name[100];
    int borrowedBookId;
};

struct Admin
{
    int id;
    char password[100];
};
