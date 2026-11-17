#include <iostream>
#include <fstream>
#include <cstdint>

using std::uint64_t;
using std::int64_t;

struct InputValues {
        const char* input_path = nullptr;
        const char* output_path = nullptr;
        uint64_t max_iter = 0;
        uint64_t freq = 0;
        bool has_max_iter = false;
};
uint64_t* CreateField(uint64_t x, uint64_t y){
    uint64_t* field = new uint64_t[x * y]{};
    return field;
}
uint64_t StringToUint64(const char* text) {
    uint64_t result = 0;

    for (int i = 0; text[i] != '\0'; ++i) {
        result = result * 10 + (text[i] - '0');
    }

    return result;
}

bool StringComprasion (const char* string1, const char* string2){
    int i = 0;
    for (; (string1[i] != '\0') && (string2[i] != '\0'); i++){
        if (string1[i] != string2[i]){
            return false;
        }
    }
    return string1[i] == string2[i];

}

uint64_t* FileToField(const char* file_name, uint64_t* height, uint64_t* width) {
    *width = 0;
    *height = 0;
    std::ifstream file(file_name, std::ios::binary);
    int64_t x;
    int64_t y;
    uint64_t grains;
    if (!(file >> x >> y >> grains)) {
        return nullptr;
    }
    int64_t min_x = x;
    int64_t max_x = x;
    int64_t min_y = y;
    int64_t max_y = y;
    while (file >> x >> y >> grains ) {
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
    }
    file.close();
    std::ifstream second_file(file_name);
    if (!second_file.is_open()) {
        return nullptr;
    }
    *width = max_x - min_x + 1;
    *height = max_y - min_y + 1;
    uint64_t* field = CreateField(*width, *height);
    while (second_file >> x >> y >> grains) {
        uint64_t column = x - min_x;
        uint64_t strings = y - min_y;
        uint64_t index = strings * (*width) + column;
        field[index] = grains;
    }
    return field;
}

InputValues GetInputValues(char* argv[], int argc) {
    InputValues values {};
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (i + 1 >= argc ||
                argv[i + 1][0] == '\0' ||
                argv[i + 1][0] == '-') {
                std::cout << "Нет корректного значения после " << argv[i] << std::endl;
                std::exit(1);
    }
            if (argv[i][1] == '-') {
                if (StringComprasion(argv[i], "--input")) {
                    values.input_path = argv[i + 1];
                }
                else if (StringComprasion(argv[i], "--output")) {
                    values.output_path = argv[i + 1];
                }
                else if (StringComprasion(argv[i], "--max-iter")) {
                    values.max_iter = StringToUint64(argv[i + 1]);
                    values.has_max_iter = true;
                }
                else if (StringComprasion(argv[i], "--freq")) {
                    values.freq = StringToUint64(argv[i + 1]);
                }
                else {
                    std::cout << "Неизвестный Аргумент" << argv[i] << std::endl;
                    std::exit(1);
                }
            } else {
                if (StringComprasion(argv[i], "-i")) {
                    values.input_path = argv[i + 1];
                }
                else if (StringComprasion(argv[i], "-o")) {
                    values.output_path = argv[i + 1];
                }
                else if (StringComprasion(argv[i], "-m")) {
                    values.max_iter = StringToUint64(argv[i + 1]);
                    values.has_max_iter = true;
                }
                else if (StringComprasion(argv[i], "-f")) {
                    values.freq = StringToUint64(argv[i + 1]);
                }
                else {
                    std::cout << "Неизвестный Аргумент" << argv[i] << std::endl;
                    std::exit(1);
                }
            }
        }
    }
    if (values.input_path == nullptr || values.output_path == nullptr) {
        std::cout << "Нужно указать --input и --output" << std::endl;
        std::exit(1);
    }
    return values;
}
bool IsStable (uint64_t* field, uint64_t x, uint64_t y ) {
    bool flag = true;
    for (uint64_t i = 0; i < x * y; i++) {
        if (field[i] >= 4) {
            flag = false;
            break;
        }
    }
    return flag;
}
uint64_t FindNewFieldIndex (int top_side, int left_side, int right_side, uint64_t i, uint64_t x) {
    uint64_t new_column = (i % x) + left_side;
    uint64_t new_line = (i / x) + top_side;
    uint64_t new_width = x + left_side + right_side;
    uint64_t new_index = new_line * new_width + new_column;
    return new_index;
}
uint64_t* Iterator (uint64_t* initial_field, uint64_t* initial_x, uint64_t* initial_y) {
    uint64_t x = *initial_x;
    uint64_t y = *initial_y;
    int top_side = 0;
    int bottom_side = 0;
    int left_side = 0;
    int right_side = 0;
    for (uint64_t i = 0; i < x * y; i++) {
        if (i < x && initial_field[i] >= 4) {
            top_side = 1;
        }
        if (i >= x * (y - 1) && initial_field[i] >= 4) {
            bottom_side = 1;
        }
        if (i % x == 0 && initial_field[i] >= 4) {
            left_side = 1;
        }
        if (i % x == x - 1 && initial_field[i] >=4) {
            right_side = 1;
        }
    }
    uint64_t* new_field = CreateField(x + left_side + right_side, y + top_side + bottom_side);
    for (uint64_t i = 0; i < x * y; i++) {

        new_field[FindNewFieldIndex(top_side, left_side, right_side, i, x)] = initial_field[i];
    }
    for (uint64_t i = 0; i < x * y; i++) {
        if (initial_field [i] >= 4) {
            uint64_t new_index = FindNewFieldIndex(top_side, left_side, right_side, i, x);
            uint64_t new_width = x + left_side + right_side;
            new_field[new_index + 1] += 1;
            new_field[new_index - 1] += 1;
            new_field[new_index - new_width] += 1;
            new_field[new_index + new_width] += 1;
            new_field[new_index] -= 4;
        }
    }
    *initial_x += left_side + right_side;
    *initial_y += top_side + bottom_side;
    delete[] initial_field;
    return new_field;
}

void Write4Bytes(std::ofstream& file, uint64_t number) {
    for (int i = 0; i < 4; i++) {
        int byte = number % 256;
        file.put(static_cast<char>(byte));
        number = number / 256;
    }
}

void Write2Bytes(std::ofstream& file, uint64_t number) {
    for (int i = 0; i < 2; i++) {
        int byte = number % 256;
        file.put(static_cast<char>(byte));
        number = number / 256;
    }
}

void WriteBMP (const char* file_name, uint64_t* field,uint64_t width, uint64_t height) {
    std::ofstream file(file_name, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Не удалось создать BMP-файл\n";
        std::exit(1);
    }
    uint64_t string_bytes = (width + 1) / 2;
    uint64_t padding = (4 - string_bytes % 4) % 4;
    uint64_t image_size = (string_bytes + padding) * height;
    uint64_t file_size = 118 + image_size;
    file.put('B');
    file.put('M');
    Write4Bytes(file, file_size);
    Write2Bytes(file, 0);
    Write2Bytes(file, 0);
    Write4Bytes(file, 118);
    // Описание файла
    Write4Bytes(file, 40);
    Write4Bytes(file, width);
    Write4Bytes(file, -height);
    Write2Bytes(file, 1);
    Write2Bytes(file, 4);
    Write4Bytes(file, 0);
    Write4Bytes(file, image_size);
    Write4Bytes(file, 0);
    Write4Bytes(file, 0);
    Write4Bytes(file, 16);
    Write4Bytes(file, 0);
    // 0 — чёрный
    file.put(0);
    file.put(0);
    file.put(0);
    file.put(0);
    // 1 — зелёный
    file.put(0);
    file.put(static_cast<char>(255));
    file.put(0);
    file.put(0);
    // 2 — фиолетовый
    file.put(static_cast<char>(255));
    file.put(0);
    file.put(static_cast<char>(255));
    file.put(0);
    // 3 — жёлтый
    file.put(0);
    file.put(static_cast<char>(255));
    file.put(static_cast<char>(255));
    file.put(0);
    // 4 — белый
    file.put(static_cast<char>(255));
    file.put(static_cast<char>(255));
    file.put(static_cast<char>(255));
    file.put(0);
    // Остальные без цвета
    for (int i = 0; i < 44; i++) {
        file.put(0);
    }


    for (uint64_t i = 0; i < width * height; i++) {
        if ((i % width) % 2 == 0) {
            uint64_t first_color = field[i];
            uint64_t second_color = 0;
            if (i % width + 1 < width) {
                second_color = field[i + 1];
            }
            if (first_color > 4) {
                first_color = 4;
            }
            if (second_color > 4) {
                second_color = 4;
            }
            file.put(static_cast<char>(first_color * 16 + second_color));
        }
        if (i % width == width - 1) {
            for (uint64_t j = 0; j < padding; j++) {
                file.put(0);
            }
        }
    }
}

int main(int argc,char* argv[]) {
    InputValues input_values = GetInputValues(argv, argc);
    uint64_t height = 0;
    uint64_t width = 0;
    uint64_t* field = FileToField(input_values.input_path, &height, &width);
    uint64_t iterations = 0;
    if (field == nullptr) {
        std::cerr << "Ошибка чтения входного файла\n";
        return 1;
    }
    while (!IsStable(field, width, height) && (!input_values.has_max_iter || iterations < input_values.max_iter)) {
        field = Iterator(field, &width, &height);
        iterations ++;
        if (input_values.freq > 0 && iterations % input_values.freq == 0) {
            WriteBMP(input_values.output_path, field, width, height);
        }
    }
    WriteBMP(input_values.output_path, field, width, height);
    std::cout << "Итераций: " << iterations << '\n';
    std::cout << "Размер: " << width << " x " << height << '\n';
    delete [] field;
}