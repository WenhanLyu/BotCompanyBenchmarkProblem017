#ifndef FILEIO_HPP
#define FILEIO_HPP

#include <fstream>
#include <cstring>

/**
 * File I/O Utilities for binary data persistence
 * Provides efficient read/write operations for disk-based data structures
 */

class FileIO {
private:
    std::fstream file;
    std::string filename;
    bool is_open;

public:
    FileIO() : is_open(false) {}

    explicit FileIO(const std::string& fname) : filename(fname), is_open(false) {
        open(fname);
    }

    ~FileIO() {
        close();
    }

    // Open file for read/write, create if not exists
    bool open(const std::string& fname) {
        filename = fname;
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

        if (!file) {
            // File doesn't exist, create it
            file.open(filename, std::ios::out | std::ios::binary);
            file.close();
            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        }

        is_open = file.is_open();
        return is_open;
    }

    // Close the file
    void close() {
        if (is_open && file.is_open()) {
            file.close();
            is_open = false;
        }
    }

    // Read data at specific position
    template<typename T>
    bool read(T& data, std::streampos pos) {
        if (!is_open) return false;

        file.seekg(pos);
        if (file.fail()) {
            file.clear();
            return false;
        }

        file.read(reinterpret_cast<char*>(&data), sizeof(T));
        return !file.fail();
    }

    // Write data at specific position
    template<typename T>
    bool write(const T& data, std::streampos pos) {
        if (!is_open) return false;

        file.seekp(pos);
        if (file.fail()) {
            file.clear();
            return false;
        }

        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
        file.flush();
        return !file.fail();
    }

    // Append data to end of file
    template<typename T>
    bool append(const T& data) {
        if (!is_open) return false;

        file.seekp(0, std::ios::end);
        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
        file.flush();
        return !file.fail();
    }

    // Get file size
    std::streampos size() {
        if (!is_open) return 0;

        std::streampos current = file.tellg();
        file.seekg(0, std::ios::end);
        std::streampos result = file.tellg();
        file.seekg(current);
        return result;
    }

    // Check if file is open
    bool isOpen() const {
        return is_open;
    }

    // Truncate file (delete all content)
    void truncate() {
        close();
        file.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
        file.close();
        open(filename);
    }
};

/**
 * Memory-mapped file buffer for efficient batch I/O
 * Reduces disk access overhead by buffering operations
 */
template<typename T, int BufferSize = 4096>
class BufferedFileIO {
private:
    FileIO file;
    T buffer[BufferSize];
    int buffer_count;
    std::streampos write_pos;

public:
    BufferedFileIO() : buffer_count(0), write_pos(0) {}

    explicit BufferedFileIO(const std::string& fname) : buffer_count(0), write_pos(0) {
        file.open(fname);
    }

    bool open(const std::string& fname) {
        return file.open(fname);
    }

    void close() {
        flush();
        file.close();
    }

    // Add item to buffer, flush if full
    bool write(const T& data) {
        buffer[buffer_count++] = data;

        if (buffer_count >= BufferSize) {
            return flush();
        }
        return true;
    }

    // Flush buffer to disk
    bool flush() {
        if (buffer_count == 0) return true;

        for (int i = 0; i < buffer_count; i++) {
            if (!file.write(buffer[i], write_pos)) {
                return false;
            }
            write_pos += sizeof(T);
        }

        buffer_count = 0;
        return true;
    }

    // Read single item at position
    bool read(T& data, std::streampos pos) {
        return file.read(data, pos);
    }
};

#endif // FILEIO_HPP
