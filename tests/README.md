# Tests

## Structure
It is important to distinguish between the two types of tests performed during development:
1. **Logic test**: in the file `test_fsm_logic.c`, it is possible to find the test of the logic of our project: we implemented in this file all the states of the Finite State Machine that we used on our project with all their transition functions. To ensure the logic works independently of the physical setup, hardware-dependent instructions were replaced with mock functions. These functions return predefined fake values or print status messages to the console to simulate real-world behavior.

2. **Hardware test**: in all the other files, we tested how each component works with some basic functions, which we used further on on our project. Precisely, we tested:  
* `test_display.c`: Draws a static radar map and a simulated scanning line to verify SPI communication and the Graphics Library setup.  
* `test_sensor.c`: Triggers the ultrasonic sensor and prints the measured distance (in cm) to the console to verify timing and signal integrity.  
* `test_servo.c`: Validates PWM generation by moving the servo through three key positions (0°, 90°, and 180°) with a 2-second delay between steps.;  
* `test_uart_iot.c`: Tests UART communication with the ESP-12E module by sending mock CSV data to the web dashboard, ensuring the cloud-link is active.

## How to run
Here, it is also needed to distinguish between the two types of tests:
### 1. **Logic test**:
To test this part, it is only needed to compile and run the code with gcc, so: 
```bash
gcc test_fsm_logic.c -o test_fsm_logic
./test_fsm_logic.
``` 
If everything is set correctly, the program should run and simulate how our project works, printing each passage that is being done with the results of each operation (e.g., "scanned an object" or "nothing within the working range").  
### 2. **Hardware tests**: 
To test this part, it is needed to use Code Composer Studio with the physical MSP432 board. Before starting, ensure that the `LcdDriver` folder and TI `DriverLib` are present in your project, and that all include paths are correctly configured (which passages are explained better in the README of the general project).

Follow these steps to run a specific test:

* **Import the Test Files**: Add the desired testing files (e.g., `test_servo.c`, `test_sensor.c`, etc.) to your CCS project folder.
* **Exclude Unnecessary Files**: To avoid "duplicate symbol" errors during compilation, you must ensure that only one `main()` function is active at a time. 
    1. Right-click on the file you do **not** want to run (e.g., your original `main.c` or other test files).
    2. Select **"Exclude from Build"**. The file icon will turn gray, indicating it is no longer part of the current compilation.
* **Build and Run**: 
    1. Make sure only the specific test file you want to execute is NOT excluded.
    2. Click the **Debug** button (the bug icon) in the top toolbar.
    3. CCS will build the selected test and flash it onto the MSP432.
* **Monitor Results**: View the output through the **CCS Post-build Console** for UART/Sensor data or directly on the **BoosterPack LCD** for display tests.