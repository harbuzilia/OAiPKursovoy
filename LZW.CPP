#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <chrono>

// Функция для сжатия данных с помощью алгоритма LZW
std::vector<unsigned int> compressLZW(const std::string& input) {
    std::unordered_map<std::string, unsigned int> dictionary;
    std::vector<unsigned int> compressedData;

    // Инициализация словаря
    unsigned int dictionarySize = 256;
    for (unsigned int i = 0; i < dictionarySize; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string currentCode;
    for (char c : input) {
        std::string newCode = currentCode + c;
        if (dictionary.find(newCode) != dictionary.end()) {
            // Продолжаем расширять текущий код
            currentCode = newCode;
        } else {
            // Добавляем код в сжатые данные и расширяем словарь
            compressedData.push_back(dictionary[currentCode]);
            dictionary[newCode] = dictionarySize++;
            currentCode = std::string(1, c);
        }
    }

    // Добавляем последний код в сжатые данные
    if (!currentCode.empty()) {
        compressedData.push_back(dictionary[currentCode]);
    }

    return compressedData;
}

// Функция для распаковки данных сжатых с помощью алгоритма LZW
std::string decompressLZW(const std::vector<unsigned int>& compressedData) {
    std::unordered_map<unsigned int, std::string> dictionary;
    std::string decompressedData;

    // Инициализация словаря
    unsigned int dictionarySize = 256;
    for (unsigned int i = 0; i < dictionarySize; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    unsigned int previousCode = compressedData[0];
    decompressedData += dictionary[previousCode];

    for (unsigned int i = 1; i < compressedData.size(); ++i) {
        unsigned int currentCode = compressedData[i];
        std::string currentString;

        if (dictionary.find(currentCode) != dictionary.end()) {
            currentString = dictionary[currentCode];
        } else if (currentCode == dictionarySize) {
            currentString = dictionary[previousCode] + dictionary[previousCode][0];
        }

        decompressedData += currentString;

        // Обновление словаря
        dictionary[dictionarySize++] = dictionary[previousCode] + currentString[0];
        previousCode = currentCode;
    }

    return decompressedData;
}

// Функция для записи сжатых данных в файл
void writeCompressedDataToFile(const std::string& filename, const std::vector<unsigned int>& compressedData) {
    std::ofstream outputFile(filename, std::ios::binary);

    // Записываем размер словаря в файл
    unsigned int dictionarySize = compressedData.size();
    outputFile.write(reinterpret_cast<const char*>(&dictionarySize), sizeof(dictionarySize));

    // Записываем сжатые данные в файл
    outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size() * sizeof(unsigned int));

    outputFile.close();
}

// Функция для чтения сжатых данных из файла
std::vector<unsigned int> readCompressedDataFromFile(const std::string& filename) {
std::ifstream inputFile(filename, std::ios::binary);
// Считываем размер словаря из файла
unsigned int dictionarySize;
inputFile.read(reinterpret_cast<char*>(&dictionarySize), sizeof(dictionarySize));

// Считываем сжатые данные из файла
std::vector<unsigned int> compressedData(dictionarySize);
inputFile.read(reinterpret_cast<char*>(compressedData.data()), dictionarySize * sizeof(unsigned int));

inputFile.close();

return compressedData;
}

int main() {
setlocale(LC_ALL, "Russian");
int choice;
std::string inputFilename, outputFilename;

while (true) {
    std::cout << "Меню:" << std::endl;
    std::cout << "1: Сжать файл" << std::endl;
    std::cout << "2: Распаковка" << std::endl;
    std::cout << "3: Выход" << std::endl;
    std::cout << "Введите ваш выбор: ";
    std::cin >> choice;

    switch (choice) {
        case 1: {
            std::cout << "Введите имя входного файла: ";
            std::cin >> inputFilename;
            std::cout << "Введите имя выходного файла: ";
            std::cin >> outputFilename;

            std::ifstream inputFile(inputFilename, std::ios::binary);
            if (!inputFile) {
                std::cerr << "Не удалось открыть файл: " << inputFilename << std::endl;
                return 1;
            }

            std::string inputData(std::istreambuf_iterator<char>(inputFile), {});
            inputFile.close();

            auto startTime = std::chrono::steady_clock::now();

            std::vector<unsigned int> compressedData = compressLZW(inputData);

            writeCompressedDataToFile(outputFilename, compressedData);

            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

            std::cout << "Сжатие завершено. Результат сохранен в файл: " << outputFilename << std::endl;
            std::cout << "Время сжатия: " << duration << " мс" << std::endl;

            break;
        }
        case 2: {
            std::cout << "Введите имя сжатого файла для распаковки: ";
            std::cin >> inputFilename;
            std::cout << "Введите имя файла для сохранения распакованных данных: ";
            std::cin >> outputFilename;

            auto startTime = std::chrono::steady_clock::now();

            std::vector<unsigned int> compressedData = readCompressedDataFromFile(inputFilename);
            std::string decompressedData = decompressLZW(compressedData);

            std::ofstream outputFile(outputFilename, std::ios::binary);
            if (!outputFile) {
                std::cerr << "Не удалось открыть файл для записи: " << outputFilename << std::endl;
                return 1;
            }

            outputFile.write(decompressedData.data(), decompressedData.size());
            outputFile.close();

            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

            std::cout << "Распаковка завершена. Результат сохранен в файл: " << outputFilename << std::endl;
                std::cout << "Время распаковки: " << duration << " мс" << std::endl;

            break;
        }
        case 3: {
            std::cout << "Выход из программы." << std::endl;
            return 0;
        }
        default:
            std::cout << "Неверный выбор. Пожалуйста, повторите ввод." << std::endl;
            break;
    }
}

return 0;
}
