#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include "get_next_line.h"

// Helper function to create test files
void create_test_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file) {
        fputs(content, file);
        fclose(file);
    }
}

void test_basic_functionality() {
    create_test_file("test_basic.txt", "Hello\nWorld\nThis is a test file.\n");

    int fd = open("test_basic.txt", O_RDONLY);
    assert(fd != -1);

    char *line = get_next_line(fd);
    assert(line && strcmp(line, "Hello\n") == 0);
    free(line);

    line = get_next_line(fd);
    assert(line && strcmp(line, "World\n") == 0);
    free(line);

    line = get_next_line(fd);
    assert(line && strcmp(line, "This is a test file.\n") == 0);
    free(line);

    line = get_next_line(fd);
    assert(line == NULL); // Should return NULL after last line
    close(fd);
}

void test_empty_file() {
    create_test_file("test_empty.txt", "");

    int fd = open("test_empty.txt", O_RDONLY);
    assert(fd != -1);

    char *line = get_next_line(fd);
    assert(line == NULL); // Empty file should return NULL
    close(fd);
}

void test_single_line_without_newline() {
    create_test_file("test_single_line.txt", "This is a single line without newline");

    int fd = open("test_single_line.txt", O_RDONLY);
    assert(fd != -1);

    char *line = get_next_line(fd);
    assert(line && strcmp(line, "This is a single line without newline") == 0);
    free(line);

    line = get_next_line(fd);
    assert(line == NULL); // Should return NULL after single line
    close(fd);
}

void test_single_line_with_newline() {
    create_test_file("test_single_line_with_newline.txt", "This line has a newline\n");

    int fd = open("test_single_line_with_newline.txt", O_RDONLY);
    assert(fd != -1);

    char *line = get_next_line(fd);
    assert(line && strcmp(line, "This line has a newline\n") == 0);
    free(line);

    line = get_next_line(fd);
    assert(line == NULL); // Should return NULL after single line
    close(fd);
}

void test_large_file() {
    // Create a large file content
    const int repeat_count = 10000;
    FILE *file = fopen("test_large.txt", "w");
    for (int i = 0; i < repeat_count; i++) {
        fputs("This is a line in a large file\n", file);
    }
    fclose(file);

    int fd = open("test_large.txt", O_RDONLY);
    assert(fd != -1);

    for (int i = 0; i < repeat_count; i++) {
        char *line = get_next_line(fd);
        assert(line && strcmp(line, "This is a line in a large file\n") == 0);
        free(line);
    }

    char *line = get_next_line(fd);
    assert(line == NULL); // Should return NULL after last line
    close(fd);
}

void test_multiple_fds() {
    create_test_file("test_file1.txt", "File 1, Line 1\nFile 1, Line 2\n");
    create_test_file("test_file2.txt", "File 2, Line 1\nFile 2, Line 2\n");

    int fd1 = open("test_file1.txt", O_RDONLY);
    int fd2 = open("test_file2.txt", O_RDONLY);
    assert(fd1 != -1 && fd2 != -1);

    char *line1 = get_next_line(fd1);
    char *line2 = get_next_line(fd2);
    assert(line1 && strcmp(line1, "File 1, Line 1\n") == 0);
    assert(line2 && strcmp(line2, "File 2, Line 1\n") == 0);
    free(line1);
    free(line2);

    line1 = get_next_line(fd1);
    line2 = get_next_line(fd2);
    assert(line1 && strcmp(line1, "File 1, Line 2\n") == 0);
    assert(line2 && strcmp(line2, "File 2, Line 2\n") == 0);
    free(line1);
    free(line2);

    line1 = get_next_line(fd1);
    line2 = get_next_line(fd2);
    assert(line1 == NULL);
    assert(line2 == NULL);

    close(fd1);
    close(fd2);
}

// Function to dynamically set BUFFER_SIZE for tests
#define BUFFER_SIZE 42 // adjust this dynamically if needed in compilation

void test_varied_buffer_sizes() {
    create_test_file("test_buffer_sizes.txt", "Line one\nLine two\nLine three\n");

    for (int buf_size = 1; buf_size <= 1024; buf_size *= 2) {
        // Compile with dynamic BUFFER_SIZE adjustment if possible
        int fd = open("test_buffer_sizes.txt", O_RDONLY);
        assert(fd != -1);

        // Assuming BUFFER_SIZE is dynamically set per compilation, this loop allows varied testing
        char *line = get_next_line(fd);
        assert(line && strcmp(line, "Line one\n") == 0);
        free(line);

        line = get_next_line(fd);
        assert(line && strcmp(line, "Line two\n") == 0);
        free(line);

        line = get_next_line(fd);
        assert(line && strcmp(line, "Line three\n") == 0);
        free(line);

        close(fd);
    }
}

void test_empty_lines_and_multinewline() {
    create_test_file("test_multinewline.txt", "\n\n\n\n");

    int fd = open("test_multinewline.txt", O_RDONLY);
    assert(fd != -1);

    for (int i = 0; i < 4; i++) {
        char *line = get_next_line(fd);
        assert(line && strcmp(line, "\n") == 0);
        free(line);
    }

    char *line = get_next_line(fd);
    assert(line == NULL); // Should return NULL after all lines
    close(fd);
}

void test_very_long_line() {
    // Create a long line much larger than typical BUFFER_SIZE
    char long_line[5000];
    for (int i = 0; i < 4999; i++) long_line[i] = 'A';
    long_line[4999] = '\n';
    create_test_file("test_very_long_line.txt", long_line);

    int fd = open("test_very_long_line.txt", O_RDONLY);
    assert(fd != -1);

    char *line = get_next_line(fd);
    assert(line && strlen(line) == 5000);
    assert(line[4999] == '\n');
    free(line);

    line = get_next_line(fd);
    assert(line == NULL); // Should return NULL after long line
    close(fd);
}

void test_multifd_complex() {
    create_test_file("test_fileA.txt", "File A, Line 1\nFile A, Line 2\n");
    create_test_file("test_fileB.txt", "File B, Line 1\nFile B, Line 2\n");

    int fdA = open("test_fileA.txt", O_RDONLY);
    int fdB = open("test_fileB.txt", O_RDONLY);
    assert(fdA != -1 && fdB != -1);

    // Read alternately from different file descriptors
    char *lineA = get_next_line(fdA);
    char *lineB = get_next_line(fdB);
    assert(lineA && strcmp(lineA, "File A, Line 1\n") == 0);
    assert(lineB && strcmp(lineB, "File B, Line 1\n") == 0);
    free(lineA);
    free(lineB);

    lineA = get_next_line(fdA);
    lineB = get_next_line(fdB);
    assert(lineA && strcmp(lineA, "File A, Line 2\n") == 0);
    assert(lineB && strcmp(lineB, "File B, Line 2\n") == 0);
    free(lineA);
    free(lineB);

    close(fdA);
    close(fdB);
}

void test_invalid_fd() {
    int invalid_fd = -1;
    char *line = get_next_line(invalid_fd);
    assert(line == NULL); // Invalid FD should return NULL
}

int main() {
    test_basic_functionality();
    test_empty_file();
    test_single_line_without_newline();
    test_single_line_with_newline();
    test_large_file();
    test_multiple_fds();
	test_varied_buffer_sizes();
    test_empty_lines_and_multinewline();
    test_very_long_line();
    test_multifd_complex();
    test_invalid_fd();

    printf("All tests passed.\n");
    return 0;
}