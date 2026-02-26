#include <iostream>
#include <cstring>

int parsePipeSeparated(const char* input, char result[][105], int max_count) {
    int count = 0;
    int pos = 0;
    int idx = 0;

    while (input[pos] != '\0' && count < max_count) {
        if (input[pos] == '|') {
            result[count][idx] = '\0';
            count++;
            idx = 0;
            pos++;
        } else {
            result[count][idx++] = input[pos++];
        }
    }

    if (idx > 0 && count < max_count) {
        result[count][idx] = '\0';
        count++;
    }

    return count;
}

int main() {
    const char* input = "湖北省黄石市|安徽省滁州市|江西省乐平市|河南省鹤壁市|四川省都江堰市|贵州省贵阳市|北京市|福建省建阳市|江西省上饶市|广东省清远市|河北省任丘市|四川省峨眉山市";

    char stations[100][105];
    int count = parsePipeSeparated(input, stations, 100);

    std::cout << "Parsed " << count << " stations:" << std::endl;
    for (int i = 0; i < count; i++) {
        std::cout << i << ": " << stations[i] << std::endl;
    }

    return 0;
}
