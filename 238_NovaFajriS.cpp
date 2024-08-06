#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

// Struktur data Buku
struct bookData
{
    int ID;
    string Title;
    string Author;
    int Year;
    bool borrowed;
    string borrower;
    bookData *next;

    bookData(int i, string j, string p, int t) : ID(i), Title(j), Author(p), Year(t), borrowed(false), borrower(""), next(nullptr) {}
};

// Struktur data untuk node pada binary search tree
struct TreeNode
{
    bookData book;
    TreeNode *left;
    TreeNode *right;

    TreeNode(bookData b) : book(b), left(nullptr), right(nullptr) {}
};

// Struktur data untuk node pada graph
struct GraphNode
{
    string loc;
    GraphNode **adjacent;
    int adjacentCount;
    bool visited;

    GraphNode(string l) : loc(l), adjacentCount(0), visited(false)
    {
        adjacent = new GraphNode *[10]; // Asumsikan maksimal 10 adjacent nodes
    }

    void addEdge(GraphNode *node)
    {
        adjacent[adjacentCount++] = node;
    }

    ~GraphNode()
    {
        delete[] adjacent;
    }
};

// Struktur data untuk akun pengguna
struct User
{
    string ID;
    string name;
    User *next;

    User(string u, string p) : ID(u), name(p), next(nullptr) {}
};

// Struktur data Transaksi
struct Transaction
{
    string bookTitle; // Mengganti idBuku menjadi judulBuku
    string borrowDate;
    string backDate;
    string borrower; // Menyimpan informasi username peminjam
    Transaction *next;

    Transaction(const string &title, const string &borrDate) : bookTitle(title), borrowDate(borrDate), backDate(""), borrower(""), next(nullptr) {}
};

// Fungsi hash sederhana
int hashFunction(const string &key, int tableSize)
{
    int hash = 0;
    for (char ch : key)
    {
        hash = (hash * 31 + ch) % tableSize;
    }
    return hash;
}

// Fungsi rekursif untuk memasukkan buku ke dalam binary search tree
TreeNode *insertTree(TreeNode *root, bookData book)
{
    if (root == nullptr)
    {
        return new TreeNode(book);
    }

    if (book.Title < root->book.Title)
    {
        root->left = insertTree(root->left, book);
    }
    else
    {
        root->right = insertTree(root->right, book);
    }

    return root;
}

// Fungsi rekursif untuk mencari buku dalam binary search tree berdasarkan judul
TreeNode *searchTree(TreeNode *root, const string &title)
{
    if (root == nullptr || root->book.Title == title)
    {
        return root;
    }

    if (title < root->book.Title)
    {
        return searchTree(root->left, title);
    }
    else
    {
        return searchTree(root->right, title);
    }
}

// Breadth First Search untuk menemukan lokasi tertentu di perpustakaan
bool bfs(GraphNode *start, const string &targetLoc)
{
    if (start == nullptr)
        return false;

    start->visited = true;
    GraphNode *queue[100]; // Asumsikan maksimal 100 nodes dalam graph
    int front = 0, rear = 0;

    queue[rear++] = start;

    while (front < rear)
    {
        GraphNode *node = queue[front++];
        if (node->loc == targetLoc)
        {
            return true;
        }

        for (int i = 0; i < node->adjacentCount; ++i)
        {
            if (!node->adjacent[i]->visited)
            {
                node->adjacent[i]->visited = true;
                queue[rear++] = node->adjacent[i];
            }
        }
    }

    return false;
}

// Fungsi untuk menambahkan buku ke dalam linked list
void addBook(bookData *&head, bookData *bukuBaru)
{
    if (head == nullptr)
    {
        head = bukuBaru;
    }
    else
    {
        bookData *temp = head;
        while (temp->next != nullptr)
        {
            temp = temp->next;
        }
        temp->next = bukuBaru;
    }
}

// Sorting buku berdasarkan ID menggunakan Bubble Sort
void sortBook(bookData *head)
{
    if (head == nullptr)
        return;

    bool swapped;
    bookData *ptr1;
    bookData *lptr = nullptr;

    do
    {
        swapped = false;
        ptr1 = head;

        while (ptr1->next != lptr)
        {
            if (ptr1->ID > ptr1->next->ID)
            {
                swap(ptr1->ID, ptr1->next->ID);
                swap(ptr1->Title, ptr1->next->Title);
                swap(ptr1->Author, ptr1->next->Author);
                swap(ptr1->Year, ptr1->next->Year);
                swap(ptr1->borrowed, ptr1->next->borrowed);
                swapped = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

// Fungsi untuk menampilkan buku dalam linked list
void displayBook(bookData *head)
{
    bookData *temp = head;
    while (temp != nullptr)
    {
        cout << "\n=============================================" << endl;
        cout << "Book Code\t\t :: " << temp->ID << endl;
        cout << "Title\t\t\t :: " << temp->Title << endl;
        cout << "Author\t\t\t :: " << temp->Author << endl;
        cout << "Year Published\t :: " << temp->Year << endl;
        cout << "=============================================" << endl;
        temp = temp->next;
    }
}

// Fungsi untuk mencari buku berdasarkan judul dalam linked list
bookData *findBookbyTitle(bookData *head, const string &title)
{
    bookData *temp = head;
    while (temp != nullptr)
    {
        if (temp->Title == title)
        {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

// Fungsi untuk menambahkan transaksi ke dalam linked list
void addTransaction(Transaction *&head, Transaction *newTransaction)
{
    if (head == nullptr)
    {
        head = newTransaction;
    }
    else
    {
        Transaction *temp = head;
        while (temp->next != nullptr)
        {
            temp = temp->next;
        }
        temp->next = newTransaction;
    }
}

bool sudahDikembalikan(Transaction *TransactionList, const string &currentUser, const string &title)
{
    Transaction *temp = TransactionList;
    while (temp != nullptr)
    {
        if (temp->borrower == currentUser && temp->bookTitle == title && !temp->backDate.empty())
        {
            return true; // Jika buku sudah dikembalikan sebelumnya
        }
        temp = temp->next;
    }
    return false;
}

bool punyaBukuBelumDikembalikan(Transaction *TransactionList, const string &currentUser)
{
    Transaction *temp = TransactionList;
    while (temp != nullptr)
    {
        if (temp->borrower == currentUser && temp->backDate.empty())
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

// Fungsi untuk meminjam buku
void sudahMeminjam(bookData *head, const string &title, Transaction *&TransactionList, const string &borrowDate, const string &currentUser)
{
    // Memeriksa apakah pengguna sudah meminjam buku sebelumnya
    if (punyaBukuBelumDikembalikan(TransactionList, currentUser))
    {
        cout << "You cant borrow other book before back your borrowed book.\n";
        return;
    }

    bookData *book = findBookbyTitle(head, title);
    if (book != nullptr)
    {
        if (!book->borrowed)
            ;
        {
            book->borrowed = true;
            book->borrower = currentUser;
            Transaction *newTransaction = new Transaction(title, borrowDate);
            newTransaction->borrower = currentUser;
            addTransaction(TransactionList, newTransaction);
            cout << "Book \"" << book->Title << "\" Has been borrowed successfully." << endl;
        }
        if (book->borrowed)
        {
            cout << "Book \"" << book->Title << "\" already borrowed." << endl;
        }
    }
    else
    {
        cout << "Book not found." << endl;
    }
}

// Fungsi untuk menyimpan data buku ke dalam file teks
void saveBookData(bookData *head)
{
    ofstream file("dataBook.txt");
    if (file.is_open())
    {
        bookData *temp = head;
        while (temp != nullptr)
        {
            file << temp->ID << "\t" << temp->Title << "\t" << temp->Author << "\t" << temp->Year << "\t" << temp->borrowed << "\t" << temp->borrower << endl;
            temp = temp->next;
        }
        file.close();
    }
    else
    {
        cout << "Gagal menyimpan data buku ke dalam file dataBook.txt" << endl;
    }
}

void loadBookData(bookData *&head)
{
    ifstream file("dataBook.txt");
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string ID, title, author, yearPub, borrowed, borrower;

            getline(ss, ID, '\t');
            getline(ss, title, '\t');
            getline(ss, author, '\t');
            getline(ss, yearPub, '\t');
            getline(ss, borrowed, '\t');
            getline(ss, borrower, '\t');

            int bookID = stoi(ID);
            int year = stoi(yearPub);
            bool statusDipinjam = (borrowed == "1");
            bookData *newBook = new bookData(bookID, title, author, year);
            newBook->borrowed = statusDipinjam;
            newBook->borrower = borrower;

            addBook(head, newBook);
        }
        file.close();
    }
    else
    {
        cout << "Error opening file." << endl;
    }
}

// Fungsi untuk menyimpan akun ke file
void saveUsertoFile(User *user)
{
    ofstream file("dataAccount.txt", ios::app);
    if (file.is_open())
    {
        file << user->ID << "\t" << user->name << endl;
        file.close();
    }
}

// Fungsi untuk menambahkan akun ke hash table
void addUser(User **hashTable, int tableSize, User *newUser)
{
    int index = hashFunction(newUser->ID, tableSize);
    newUser->next = hashTable[index];
    hashTable[index] = newUser;
}

// Fungsi untuk memuat akun dari file ke hash table
void loadUserfromFile(User **hashTable, int tableSize)
{
    ifstream file("dataAccount.txt");
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string ID, name;
        getline(ss, ID, '\t');
        getline(ss, name, '\t');
        User *newUser = new User(ID, name);
        addUser(hashTable, tableSize, newUser);
    }
    file.close();
}

int generateIDAccount()
{
    int randomNumber;

    srand(time(NULL));
    for (int i = 0; i < 6; i++)
    {
        randomNumber = (rand() * rand() + rand() * rand()) % 100000; // Menghasilkan angka random antara 0 dan 999999
        randomNumber += 100000;                                      // Menambahkan 1 juta ke angka random
    }
    int ID = randomNumber;
}

int generateIDBook()
{
    int randomNumber;

    srand(time(NULL));
    for (int i = 0; i < 3; i++)
    {
        randomNumber = (rand() * rand() + rand() * rand()) % 100; // Menghasilkan angka random antara 0 dan 999999
        randomNumber += 100;                                      // Menambahkan 1 juta ke angka random
    }
    return randomNumber;
}

// Fungsi untuk membuat akun baru
void createUser(User **hashTable, int tableSize)
{
    string ID, nama;
    cout << "Masukkan Nama : ";
    cin.ignore();
    getline(cin, nama);
    int number = generateIDAccount();
    ID = to_string(number);
    cout << "ID Akun Kamu : " << ID << endl;

    User *newUser = new User(ID, nama);
    addUser(hashTable, tableSize, newUser);
    saveUsertoFile(newUser);
}

// Fungsi untuk memeriksa apakah akun sudah ada
bool checkUser(User **hashTable, int tableSize, const string &ID)
{
    int index = hashFunction(ID, tableSize);
    User *temp = hashTable[index];
    while (temp != nullptr)
    {
        if (temp->ID == ID)
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void saveBorrowData(Transaction *TransactionList)
{
    ofstream file("dataBorrow.txt");
    if (file.is_open())
    {
        Transaction *temp = TransactionList;
        while (temp != nullptr)
        {
            file << temp->bookTitle << "\t" << temp->borrowDate << "\t" << temp->borrower << endl; // Menggunakan judulBuku
            temp = temp->next;
        }
        file.close();
    }
    else
    {
        cout << "Error saving file." << endl;
    }
}

void loadBorrowData(Transaction *&TransactionList)
{
    ifstream file("dataBorrow.txt");
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string title, borrowDate, borrower;

            getline(ss, title, '\t');
            getline(ss, borrowDate, '\t');
            getline(ss, borrower, '\t');

            Transaction *newTransaction = new Transaction(title, borrowDate); // Menggunakan judulBuku
            newTransaction->borrower = borrower;                              // Simpan informasi username peminjam
            addTransaction(TransactionList, newTransaction);
        }
        file.close();
    }
    else
    {
        cout << "Error opening file." << endl;
    }
}
// Fungsi untuk menampilkan detail pinjaman buku
void BorrowDetail(bookData *head)
{
    cout << "Detail pinjaman buku:\n";
    bookData *temp = head;
    bool adaPinjaman = false;
    while (temp != nullptr)
    {
        if (temp->borrowed)
        {
            adaPinjaman = true;
            cout << "\n===========================";
            cout << "\nBook Code\t : " << temp->ID;
            cout << "\nTitle\t\t : " << temp->Title;
            cout << "\nAuthor\t\t : " << temp->Author;
            cout << "\nYear Published\t : " << temp->Year;
            cout << "\nStatus\t\t : Borrowed\n\n";
            cout << "Borrower ID\t : " << temp->borrower << "\n";
        }
        else
        {
            cout << "\n===========================";
            cout << "\nBook Code\t : " << temp->ID;
            cout << "\nTitle\t\t : " << temp->Title;
            cout << "\nAuthor\t\t : " << temp->Author;
            cout << "\nYear Published\t : " << temp->Year;
            cout << "\nStatus\t\t : Available\n\n";
        }
        temp = temp->next;
    }
    if (!adaPinjaman)
    {
        cout << "No borrowed book\n";
    }
}

bool loggedIn = false;
void borrowBook(bookData *head, const string &title, Transaction *&TransactionList, const string &borrowDate, const string &currentUser)
{
    // Memeriksa apakah pengguna sudah meminjam buku sebelumnya
    if (punyaBukuBelumDikembalikan(TransactionList, currentUser))
    {
        cout << "You can't borrow other book before you back your borrowed book." << endl;
        return;
    }

    // Memeriksa apakah pengguna sudah mengembalikan buku dengan judul yang sama sebelumnya
    if (sudahDikembalikan(TransactionList, currentUser, title))
    {
        cout << "Please back your book to borrow other book." << endl;
        return;
    }

    bookData *book = findBookbyTitle(head, title);
    if (book != nullptr)
    {
        if (!book->borrowed)
        {
            book->borrowed = true;
            book->borrower = currentUser;
            Transaction *newTransaction = new Transaction(title, borrowDate);
            newTransaction->borrower = currentUser;
            addTransaction(TransactionList, newTransaction);
            cout << "Book \"" << book->Title << "\" borrowed succesfully." << endl;
        }
        else
        {
            cout << "Book \"" << book->Title << "\" has already borrowed." << endl;
        }
    }
    else
    {
        cout << "Book not found." << endl;
    }
}

void backBook(bookData *head, const string &title, Transaction *&TransactionList, const string &backDate, const string &currentUser, bool loggedIn, bookData *&bookList)
{
    if (!loggedIn)
    {
        cout << "Please login first!" << endl;
        return;
    }
    bookData *book = findBookbyTitle(head, title);
    if (book != nullptr)
    {
        if (book->borrowed)
        {
            book->borrowed = false;
            book->borrower = ""; // Kosongkan username peminjam
            Transaction *temp = TransactionList;
            while (temp != nullptr)
            {
                if (temp->borrower == currentUser && temp->bookTitle == title && temp->backDate.empty())
                {
                    temp->backDate = backDate;
                    break;
                }
                temp = temp->next;
            }
            cout << "Book \"" << book->Title << "\" Has been back succesfully." << endl;
            saveBookData(bookList);          // Simpan data buku setelah pengembalian
            saveBorrowData(TransactionList); // Simpan data pinjaman setelah pengembalian
        }
        else
        {
            cout << "Book \"" << book->Title << "\" was not borrowed." << endl;
        }
    }
    else
    {
        cout << "Book not found." << endl;
    }

    // Membersihkan data di dataBorrow.txt setelah pengembalian buku
    ofstream clearFile("dataBorrow.txt");
    clearFile.close();
}

int main()
{

    const int tableSize = 100;
    User *hashTableUser[tableSize] = {nullptr};
    Transaction *TransactionList = nullptr;
    bookData *bookList = nullptr;

    loadUserfromFile(hashTableUser, tableSize);
    loadBookData(bookList);
    loadBorrowData(TransactionList); // Tambahkan pemanggilan untuk memuat data pinjaman

    int pilihan;
    string currentUser;

    do
    {
        if (!loggedIn)
        {
            cout << "\n|===== Library Management =====|\n";
            cout << "|=====     Login Menu     =====|\n";
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "3. Exit\n";
            cout << "Pilih : ";
            cin >> pilihan;

            switch (pilihan)
            {
            case 1:
            {
                string ID;
                cout << "Input your ID : ";
                cin >> ID;
                if (checkUser(hashTableUser, tableSize, ID))
                {
                    cout << "Login success!!\n";
                    loggedIn = true;
                    currentUser = ID;
                }
                else
                {
                    cout << "Login failed. User ID not found.\n";
                }
                break;
            }
            case 2:
            {
                createUser(hashTableUser, tableSize);
                cout << "New User has been created!\n";
                break;
            }
            case 3:
            {
                cout << "Good Bye!\n";
                return 0;
            }
            default:
            {
                cout << "Choice not valid. Please Input [1/2/3].\n";
                break;
            }
            }
        }
        else
        {
            cout << "\n|===== Library Management =====|\n";
            cout << "|=====     Main  Menu     =====|\n";
            cout << "1. Add Book\n";
            cout << "2. Display Book\n";
            cout << "3. Borrow Book\n";
            cout << "4. Back Book\n";
            cout << "5. Borow Book Detail\n";
            cout << "0. Exit\n";
            cout << "Pilih: ";
            cin >> pilihan;

            switch (pilihan)
            {
            case 1:
            { // Setelah menambah buku
                int id, year;
                string title, author;
                id = generateIDBook();
                cout << "Input Book Title : ";
                cin.ignore();
                getline(cin, title);
                cout << "Input Book Author : ";
                getline(cin, author);
                cout << "Input Book  Published Published Year : ";
                cin >> year;
                bookData *newBook = new bookData(id, title, author, year);
                addBook(bookList, newBook);
                saveBookData(bookList); // Memanggil fungsi untuk menyimpan data buku ke dalam file
                break;
            }

            case 2:
            {
                ifstream fileCheck("dataBook.txt");
                if (fileCheck.peek() == ifstream::traits_type::eof())
                {
                    cout << "No book available." << endl;
                    fileCheck.close();
                }
                else
                {
                    displayBook(bookList);
                }
                break;
            }

            case 3:
            { // Meminjam buku
                string ID;
                cout << "Input your ID : ";
                cin >> ID;

                if (checkUser(hashTableUser, tableSize, ID))
                {
                    string title, borrowDate;
                    cout << "Input book title you want to borrow :: ";
                    cin.ignore();
                    getline(cin, title);
                    cout << "Input borrow Date [DD/MM/YYYY] :: ";
                    cin >> borrowDate;
                    borrowBook(bookList, title, TransactionList, borrowDate, ID);
                    saveBorrowData(TransactionList);
                }
                else
                {
                    cout << "Login failed. User ID not found.\n";
                }
                break;
            }

            case 4:
            { // Mengembalikan buku
                string ID;
                cout << "Input your ID : ";
                cin >> ID;

                if (checkUser(hashTableUser, tableSize, ID))
                {
                    string title, backDate;
                    cout << "Input book title you want to back : ";
                    cin.ignore();
                    getline(cin, title);
                    cout << "Input back Date [DD/MM/YYYY]: ";
                    cin >> backDate;
                    backBook(bookList, title, TransactionList, backDate, ID, loggedIn, bookList);
                    saveBookData(bookList);
                    saveBorrowData(TransactionList);
                }
                else
                {
                    cout << "Login failed. User ID not found.\n";
                }
                break;
            }

            case 5:
            {
                BorrowDetail(bookList);
                break;
            }

            case 0:
            {
                loggedIn = false;
                currentUser = "";
                cout << "Logout success!!\n";
                break;
            }
            default:
            {
                cout << "Choice not valid.\n";
                break;
            }
            }
        }
    } while (pilihan != 0);

    return 0;
}
