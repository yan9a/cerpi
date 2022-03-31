// Reference
// https://github.com/tranter/blogs/blob/master/gpio/part9/example.c

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  const char *chipname = "gpiochip2";
  struct gpiod_chip *chip;
  struct gpiod_line *lineO1;    
  struct gpiod_line *lineO2;  
  struct gpiod_line *lineO3;
  struct gpiod_line *lineO4;
  struct gpiod_line *lineI5;
  struct gpiod_line *lineI6;
  struct gpiod_line *lineI7;
  struct gpiod_line *lineI8;
  int i, ret, val;

  // Open GPIO chip
  chip = gpiod_chip_open_by_name(chipname);
  if (!chip) {
    perror("Open chip failed\n");
    return 1;
  }

  // Open GPIO lines
  lineO1 = gpiod_chip_get_line(chip, 15);
  if (!lineO1) {
    perror("Get line failed\n");
    return 1;
  }

  lineO2 = gpiod_chip_get_line(chip, 14);
  if (!lineO2) {
    perror("Get line failed\n");
    return 1;
  }

  lineO3 = gpiod_chip_get_line(chip, 13);
  if (!lineO3) {
    perror("Get line failed\n");
    return 1;
  }

  lineO4 = gpiod_chip_get_line(chip, 12);
  if (!lineO4) {
    perror("Get line failed\n");
    return 1;
  }

  lineI5 = gpiod_chip_get_line(chip, 11);
  if (!lineI5) {
    perror("Get line failed\n");
    return 1;
  }

  lineI6 = gpiod_chip_get_line(chip, 10);
  if (!lineI6) {
    perror("Get line failed\n");
    return 1;
  }

  lineI7 = gpiod_chip_get_line(chip, 9);
  if (!lineI7) {
    perror("Get line failed\n");
    return 1;
  }

  lineI8 = gpiod_chip_get_line(chip, 8);
  if (!lineI8) {
    perror("Get line failed\n");
    return 1;
  }

  // Open LED lines for output
  ret = gpiod_line_request_output(lineO1, "example1", 0);
  if (ret < 0) {
    perror("Request line as output failed\n");
    return 1;
  }

  ret = gpiod_line_request_output(lineO2, "example1", 0);
  if (ret < 0) {
    perror("Request line as output failed\n");
    return 1;
  }
  
  ret = gpiod_line_request_output(lineO3, "example1", 0);
  if (ret < 0) {
    perror("Request line as output failed\n");
    return 1;
  }

  ret = gpiod_line_request_output(lineO4, "example1", 0);
  if (ret < 0) {
    perror("Request line as output failed\n");
    return 1;
  }

  // Open switch line for input
  ret = gpiod_line_request_input(lineI5, "example1");
  if (ret < 0) {
    perror("Request line as input failed\n");
    return 1;
  }

  ret = gpiod_line_request_input(lineI6, "example1");
  if (ret < 0) {
    perror("Request line as input failed\n");
    return 1;
  }

  ret = gpiod_line_request_input(lineI7, "example1");
  if (ret < 0) {
    perror("Request line as input failed\n");
    return 1;
  }

  ret = gpiod_line_request_input(lineI8, "example1");
  if (ret < 0) {
    perror("Request line as input failed\n");
    return 1;
  }

  // Blink LEDs in a binary pattern
  i = 0;
  while (true) {
    ret = gpiod_line_set_value(lineO1, (i & 1) != 0);
    if (ret < 0) {
      perror("Set line output failed\n");
      return 1;
    }
    ret = gpiod_line_set_value(lineO2, (i & 2) != 0);
    if (ret < 0) {
      perror("Set line output failed\n");
      return 1;
    }
    ret = gpiod_line_set_value(lineO3, (i & 4) != 0);
    if (ret < 0) {
      perror("Set line output failed\n");
      return 1;
    }
    
    ret = gpiod_line_set_value(lineO4, (i & 8) != 0);
    if (ret < 0) {
      perror("Set line output failed\n");
      return 1;
    }

   if(i%10==0){
        val = gpiod_line_get_value(lineI5);
        if (val < 0) {
        perror("Read line input failed\n");
        return 1;
        }
        printf("%d ",val);

        val = gpiod_line_get_value(lineI6);
        if (val < 0) {
        perror("Read line input failed\n");
        return 1;
        }
        printf("%d ",val);

        val = gpiod_line_get_value(lineI7);
        if (val < 0) {
        perror("Read line input failed\n");
        return 1;
        }
        printf("%d ",val);

        val = gpiod_line_get_value(lineI8);
        if (val < 0) {
        perror("Read line input failed\n");
        return 1;
        }
        printf("%d \n",val);
    // // Exit if button pressed
    // if (val == 0) {
    //   break;
    // }
   }
    usleep(100000);
    i++;
  }

  // Release lines and chip
  gpiod_line_release(lineO1);
  gpiod_line_release(lineO2);
  gpiod_line_release(lineO3);
  gpiod_line_release(lineI8);
  gpiod_chip_close(chip);
  return 0;
}