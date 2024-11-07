#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "get_next_line_bonus.h"

// Helper function to create test files for the tests
void create_test_file(const char *filename, const char *content)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("Failed to open file");
        exit(1);
    }
    write(fd, content, strlen(content));
    close(fd);
}

// Test function to ensure `get_next_line` works with multiple file descriptors
void test_multiple_fds()
{
    // Create two small test files with distinct contents
    create_test_file("test_file1.txt", "File 1, Line 1\nFile 1, Line 2\nFile 1, Line 3\n");
    create_test_file("test_file2.txt", "File 2, Line 1\nFile 2, Line 2\n");

    // Open both files for reading
    int fd1 = open("test_file1.txt", O_RDONLY);
    int fd2 = open("test_file2.txt", O_RDONLY);
    assert(fd1 != -1 && fd2 != -1);

    // Test reading lines from both files sequentially
    char *line1 = get_next_line(fd1);
    char *line2 = get_next_line(fd2);
    assert(line1 && strcmp(line1, "File 1, Line 1\n") == 0);
    assert(line2 && strcmp(line2, "File 2, Line 1\n") == 0);
    free(line1);
    free(line2);

    // Continue reading from both files
    line1 = get_next_line(fd1);
    line2 = get_next_line(fd2);
    assert(line1 && strcmp(line1, "File 1, Line 2\n") == 0);
    assert(line2 && strcmp(line2, "File 2, Line 2\n") == 0);
    free(line1);
    free(line2);

    // Continue reading until all lines are consumed
    line1 = get_next_line(fd1);
    line2 = get_next_line(fd2);
    assert(line1 && strcmp(line1, "File 1, Line 3\n") == 0);
    assert(line2 == NULL); // File 2 is finished
    free(line1);

    // Finish reading remaining file
    line1 = get_next_line(fd1);
    assert(line1 == NULL); // File 1 is finished
    free(line1);

    // Close file descriptors
    close(fd1);
    close(fd2);
}

// Test with multiple file descriptors but one file is empty
void test_multiple_fds_with_empty_file()
{
    // Create files: one with content, one empty
    create_test_file("test_file1.txt", "File 1, Line 1\nFile 1, Line 2\n");
    create_test_file("test_file2.txt", "");

    int fd1 = open("test_file1.txt", O_RDONLY);
    int fd2 = open("test_file2.txt", O_RDONLY);
    assert(fd1 != -1 && fd2 != -1);

    // Test reading from the first file (should have data) and the second (empty)
    char *line1 = get_next_line(fd1);
    char *line2 = get_next_line(fd2);
    assert(line1 && strcmp(line1, "File 1, Line 1\n") == 0);
    assert(line2 == NULL); // File 2 is empty
    free(line1);

    // Continue reading from the first file
    line1 = get_next_line(fd1);
    assert(line1 && strcmp(line1, "File 1, Line 2\n") == 0);
    free(line1);

    line1 = get_next_line(fd1);
    assert(line1 == NULL); // File 1 is finished
    free(line1);

    // Close file descriptors
    close(fd1);
    close(fd2);
}

// Test case for a file that has just one line
void test_single_line_file()
{
    // Create a file with a single line
    create_test_file("test_file.txt", "Single Line\n");

    int fd = open("test_file.txt", O_RDONLY);
    assert(fd != -1);

    // Read the line from the file
    char *line = get_next_line(fd);
    assert(line && strcmp(line, "Single Line\n") == 0);
    free(line);

    // Read again, should return NULL (EOF)
    line = get_next_line(fd);
    assert(line == NULL);

    // Close file descriptor
    close(fd);
}

// Test case for multiple files with no newlines
void test_multiple_files_no_newline()
{
    // Create files with no newlines
    create_test_file("test_file1.txt", "NoNewlineHere");
    create_test_file("test_file2.txt", "AnotherFileNoNewline");

    int fd1 = open("test_file1.txt", O_RDONLY);
    int fd2 = open("test_file2.txt", O_RDONLY);
    assert(fd1 != -1 && fd2 != -1);

    // Test reading from both files, both should return the entire content
    char *line1 = get_next_line(fd1);
    char *line2 = get_next_line(fd2);
    assert(line1 && strcmp(line1, "NoNewlineHere") == 0);
    assert(line2 && strcmp(line2, "AnotherFileNoNewline") == 0);
    free(line1);
    free(line2);

    // Test reading again, both should return NULL (EOF)
    line1 = get_next_line(fd1);
    line2 = get_next_line(fd2);
    assert(line1 == NULL);
    assert(line2 == NULL);

    // Close file descriptors
    close(fd1);
    close(fd2);
}

// Test case where one file has an empty line
void test_file_with_empty_line()
{
    // Create a file with an empty line between content
    create_test_file("test_file.txt", "Line 1\n\nLine 3\n");

    int fd = open("test_file.txt", O_RDONLY);
    assert(fd != -1);

    // Read lines from the file, including the empty line
    char *line = get_next_line(fd);
    assert(line && strcmp(line, "Line 1\n") == 0);
    free(line);

    line = get_next_line(fd);
    assert(line && strcmp(line, "\n") == 0); // Empty line
    free(line);

    line = get_next_line(fd);
    assert(line && strcmp(line, "Line 3\n") == 0);
    free(line);

    // Read again, should return NULL (EOF)
    line = get_next_line(fd);
    assert(line == NULL);

    // Close file descriptor
    close(fd);
}

// Test case where multiple files are read simultaneously and EOF occurs
void test_multiple_fds_with_eof()
{
    // Create files with simple contents
    create_test_file("test_file1.txt", "File 1, Line 1\n");
    create_test_file("test_file2.txt", "File 2, Line 1\n");

    int fd1 = open("test_file1.txt", O_RDONLY);
    int fd2 = open("test_file2.txt", O_RDONLY);
    assert(fd1 != -1 && fd2 != -1);

    // Read from both files until EOF is reached
    char *line1 = get_next_line(fd1);
    char *line2 = get_next_line(fd2);
    assert(line1 && strcmp(line1, "File 1, Line 1\n") == 0);
    assert(line2 && strcmp(line2, "File 2, Line 1\n") == 0);
    free(line1);
    free(line2);

    // Check if both files are at EOF
    line1 = get_next_line(fd1);
    line2 = get_next_line(fd2);
    assert(line1 == NULL);
    assert(line2 == NULL);

    // Close file descriptors
    close(fd1);
    close(fd2);
}

// Main function to run the tests
int main()
{
    test_multiple_fds();
    test_multiple_fds_with_empty_file();
    test_single_line_file();
    test_multiple_files_no_newline();
    test_file_with_empty_line();
    test_multiple_fds_with_eof();

    printf("All tests passed successfully!\n");

    return 0;
}
