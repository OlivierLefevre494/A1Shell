#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CMD_LEN 1024
#define MAX_OUTPUT_LEN 4096

// Helper function to read file content into a buffer
// Returns 1 on success, 0 on failure
int read_file_content(const char *filepath, char *buffer, size_t max_len) {
  FILE *f = fopen(filepath, "r");
  if (!f)
    return 0;

  size_t len = fread(buffer, 1, max_len - 1, f);
  buffer[len] = '\0';
  fclose(f);
  return 1;
}

int main() {
  DIR *dir;
  struct dirent *entry;
  char command[MAX_CMD_LEN];
  char expected_output[MAX_OUTPUT_LEN];
  char actual_output[MAX_OUTPUT_LEN];
  char line_buffer[MAX_OUTPUT_LEN];

  // Ensure mysh is available (assuming it's in starter-code/mysh)
  if (access("mysh", F_OK) != 0) {
    printf("Error: starter-code/mysh not found. Please build it first.\n");
    return 1;
  }

  dir = opendir("../test-cases");
  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  while ((entry = readdir(dir)) != NULL) {
    // Skip . and ..
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    // Check if file ends with .txt and NOT _result.txt
    size_t len = strlen(entry->d_name);
    if (len > 4 && strcmp(entry->d_name + len - 4, ".txt") == 0 &&
        (len < 11 || strcmp(entry->d_name + len - 11, "_result.txt") != 0)) {

      char test_name[256];
      strncpy(test_name, entry->d_name, len - 4);
      test_name[len - 4] = '\0';

      // Construct paths
      char test_file_path[256];
      snprintf(test_file_path, sizeof(test_file_path), "../test-cases/%s",
               entry->d_name);

      char expected_file_path[256];
      snprintf(expected_file_path, sizeof(expected_file_path),
               "../test-cases/%s_result.txt", test_name);

      // Read expected output
      if (!read_file_content(expected_file_path, expected_output,
                             MAX_OUTPUT_LEN)) {
        printf("Test: %s SKIPPED (No result file found: %s)\n", test_name,
               expected_file_path);
        continue;
      }

      // Construct command to run mysh with input redirection
      // Use 2>&1 to capture stderr if needed, or just stdout
      snprintf(command, sizeof(command), "./mysh < %s", test_file_path);

      FILE *fp = popen(command, "r");
      if (fp == NULL) {
        printf("Failed to run command for test: %s\n", test_name);
        continue;
      }

      // Read actual output
      size_t actual_len = 0;
      actual_output[0] = '\0';
      while (fgets(line_buffer, sizeof(line_buffer), fp) != NULL) {
        if (actual_len + strlen(line_buffer) < MAX_OUTPUT_LEN) {
          strcat(actual_output, line_buffer);
          actual_len += strlen(line_buffer);
        }
      }
      pclose(fp);

      // Compare outputs
      // Ensure null termination just in case
      actual_output[MAX_OUTPUT_LEN - 1] = '\0';
      expected_output[MAX_OUTPUT_LEN - 1] = '\0';

      if (strcmp(actual_output, expected_output) == 0) {
        printf("Test: %s PASS\n", test_name);
      } else {
        printf("Test: %s FAIL\n", test_name);
        // Optional: print diff or details
        // printf("Expected:\n%s\nActual:\n%s\n", expected_output,
        // actual_output);
      }
    }
  }
  closedir(dir);
  return 0;
}