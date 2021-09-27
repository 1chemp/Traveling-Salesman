#include <iostream>
#include <fstream>
#include <string>

struct Error
{
    Error(const std::string& message_) {
        this->message = message_;    
    }

    std::string showErrorMessage() const {
        return this->message;
    }

private:
    std::string message;
};


struct Element
{
    int base;

    Element() {
        this->base = 0;
    }

    Element(int& data) {
        this->base = data;
    }
	
    void insertAfter(int* n) {
        this->next->base = *n;
    }

    int getReference() {
        return this->next->base;
    }

    Element* operator()(const int data) {
        next = new Element[1];
        this->base = data;
        return this;
    }

    friend std::ostream& operator<<(std::ostream& output, const Element& elem) {
        output << elem.base;
        return output;
    }

    ~Element() {next = nullptr;}

private:
    Element* next;
};

class TravelingSalesman
{
    int m_col;
    int m_row;
    Element** resultMatrix;

    void clear() {
        for (int i{0}; i < m_row; i++) {
            delete[] resultMatrix[i];
        }
        delete[] resultMatrix;

        m_col = 0;
        m_row = 0;
    }

public:  
    // конструктор, который по столбцам и колонкам формирует матрицу
    // и выставляет зависимости между узлами
    TravelingSalesman(const int& row, const int& col, int** data) {
        this->m_row = row;
        this->m_col = col;

        resultMatrix = new Element*[m_row];

        for (int i{0}; i<m_row; i++) {
            resultMatrix[i] = new Element[m_col];
        
            for (int j{0}; j<m_col; j++) {
                resultMatrix[i][j](data[i][j]);
                resultMatrix[i][j].insertAfter(&data[j][i]);
            }
        }
    }
    // перегрузка оператора вывода для матрицы
    friend std::ostream& operator<<(std::ostream& output, const TravelingSalesman& matrixValid) {
        for (int i{0}; i < matrixValid.m_row; i++) {
            for (int j{0}; j < matrixValid.m_col; j++) {
                // перед тем как выводить, необходимо перегрузить оператор вывода в структуре Element
                output << matrixValid.resultMatrix[i][j] << " " << " -> (" << 
                matrixValid.resultMatrix[i][j].getReference() << ") ";
            }
            output << "\n";
        }
        return output;
    }

    // Деструктор - освобождаем память
    ~TravelingSalesman()
    {
        for (int i{m_row}; i<m_row; i++)
            delete[] resultMatrix[i];

        delete[] resultMatrix;

    }
    
};

// получаем количество строк и столбцов
// число строк должно быть равно числу 
// столбцов поэтому возвращается одно число
int getRowsCols(const char* file_name) {
    int count = 0;
    char c;

    FILE *f = fopen(file_name, "r");

    while (!feof(f))
        if ((c = fgetc(f)) == '\n' || c == EOF)
            count++;
    fclose(f);

    return count;
}

// Освобождаем память выделенной матрицы
template <typename T>
void deleteMatrix(T** matrix, const int& counter) {
    for (int i{0}; i<counter; i++)
        delete[] matrix[i];

    delete[] matrix;
}

// Переводим строковый формат в целочисленный
int** fromStrToInt(std::string** matrix, const int& count) {
    int** res_matrix;
    res_matrix = new int*[count];

    for (int i{0}; i < count; i++)
    {
        res_matrix[i] = new int[count];

        for (int j{0}; j < count; j++)
        {
            res_matrix[i][j] = atoi(matrix[i][j].c_str());
        }
    }
    deleteMatrix(matrix, count);
    return res_matrix;
}

// Выделяем матрицу из файла
int** getMatrixFromFile(const char* file_name, const int& count) {
    std::string* res;
    std::string** data;
    
    res = new std::string[count*count];
    data = new std::string*[count];

    std::ifstream file{file_name};
    if (file.is_open()) {   
        std::string spam;
        int counter = 0;

        for (file>>spam; !file.eof(); file>>spam) {   
            res[counter] = spam;
            counter++;
        }

        counter = 0;
        for (int i{0}; i < count; i++) {
            data[i] = new std::string[count];

            for (int j{0} ; j < count; j++) {
                data[i][j] = res[counter];
                ++counter;
            }
        }
    }
    file.close();
    delete[] res;

    return fromStrToInt(data, count);
}

// Ищем флаги -f, -m, -h
bool findFlags(const char* searched, const char* all)
{
    int i{0}, j{0};

    while (all[i]!=0) {
        if (all[i] == searched[1])
            break;
        i++;
    }
    while (all[j]!=0) {
        if (all[j] == searched[0])
            break;
        j++;
    }
    if (i - j == 1)
        return true;
    else return false;
}

 
int main(int argc, char* argv[]) 
{
    const char* data = *(argv+1);
    std::string all_data = "Compile: /usr/bin/g++ -g3 -O0 -Wall -Wextra -Wpedantic -c main.cpp -o build/Debug/main.o\n"\
            "Traveling Salesman Problem Solver\n"\
            "Input data: arguments and their parameters\n"\
            "-f <full_path_to_file_name>.txt -- input matrix of distances from node X to node Y\n"\
            "-h -- displaying information about the program\n"\
            "-m np_complete -- solves the problem by brute force\n"\
            "-m np_partial -- solves the problem in any other way\n";
    try {
        if (argc <= 1)
        {
            std::cout << all_data;
        }
        else if (argc >= 2 && findFlags("-h", data))
        {
            std::cout << all_data;
        }
        else if (argc >= 2 && findFlags("-f", data))
        {
            const char* path = *(argv+2);
            
            int rows_and_cols{getRowsCols(path)};

            int** matrix{getMatrixFromFile(path, rows_and_cols)};

            TravelingSalesman mtx(rows_and_cols, rows_and_cols, matrix);
        
            deleteMatrix(matrix, rows_and_cols);

            std::cout << mtx;

            if (*(argv+3))
            {
                if (findFlags("-m", *(argv+3)) && (*(argv+4)))
                {
                    std::string spam = static_cast<std::string>(*(argv+4));
                    if (spam == "np_complete")
                    {
                        std::cout << "Do 1" << std::endl;
                    }
                    else if (spam == "np_partial")
                    {
                        std::cout << "Do 2" << std::endl;
                    }
                    else std::cout << "Try again! Wrong parameters" << std::endl;
                }
                else std::cout << "Try again! Wrong parameters" << std::endl;
            }
            else throw Error("Thats wrong parameters! Try again");
        }
        else throw Error("Someting went wrong!");
    }
    catch (const Error& err) {
        std::cerr << err.showErrorMessage() << std::endl;
    }
    catch (...) {
        std::cerr << "Panic!!!" << std::endl;
    }

    // int rows_and_cols{getRowsCols("/home/chemp/program_projects/twoGis_test/file.txt")};
    // int** matrix{getMatrixFromFile("/home/chemp/program_projects/twoGis_test/file.txt", rows_and_cols)};
    // TravelingSalesman mtx(rows_and_cols, rows_and_cols, matrix);
    // deleteMatrix(matrix, rows_and_cols);
    // std::cout << mtx;

    data = nullptr;
    return 0;
}