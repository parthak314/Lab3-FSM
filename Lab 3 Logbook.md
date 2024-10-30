## Task 0 - Setup GTest
GTest, or Google Test, is a C++ testing framework that facilitates unit testing with features like test case creation and assertions. 
It supports test fixtures for setup and teardown processes and integrates well with build systems and includes Google Mock for creating mock objects.

Installed using:
```bash
sudo apt install libgtest-dev
```

Then creating a test case for multiplying two numbers as:
```c++
int multiply(int a, int b) { return a * b; }
```

This is now passed after running the test and reaches the expected value of 15 for 3 * 5.

---
## Task 1 - 4-bit LFSR and Pseudo Random Binary Sequence
We first create the component `lfsr.sv` as the top level circuit such that:
- All 4 bits of the shift register output are brought out as data_out[3:0]
- `en` is the enable signal
- Reset brings the state back to 1 (not 0)
The ideal model is shown here:
<p align="center"> <img src="images/lfsr.jpg" /> </p>

The changes made here are:
- setting enable as `vbdFlag()`: Note that this should work but results in an error despite using 
``` c++
top->en = vbdFlag(); // This doesn't work despite including vbuddy.cpp and creating vbuddy.cfg
top->en = 1; // temporarily using this and passes the tests
```
- creating `lfsr.sv`

This passes the tests once run.
#### Primitive Polynomial using a PRBS generator
This will make use of `lfsr_7.sv` which is a 7 bit PRBS (Pseudo Random Binary Sequence) generator. In this case we use the 7th order primitive polynomial, $1 + X^3 + X^7$ .
```verilog
module lfsr(
    input   logic       clk,
    input   logic       rst,
    input   logic       en,
    output  logic [6:0] data_out
);
    logic   [6:0]   sreg; 

always_ff @ (posedge clk, posedge rst)
    if (rst) sreg <= 7'b1;
    else sreg <= {sreg[5:0], sreg[6] ^ sreg[2]};
    
assign data_out = sreg;

endmodule
```

---
## Task 2 - Formula 1 Light Sequence

Designing an F1 light sequence using the light bar as per the following FSM:

<p align="center"> <img src="images/state_diag.jpg" /> </p>

We first design the System Verilog file which iterates through each of the states.
We need to be mindful for the following:
- ##### Parameters: 
	- The parameters are `rst`, `en`, `clk` (all 1 bit inputs) and `data_out` (an 8 bit output). Where `data_out` will be used to reflect the changes to the light strip so is 8 bits with one for the state of each LED.
- ##### Defining states:
	- We need to define the states. Since this isn't built in, ,we will use `typedef` to define a new type which will be a list of named constants (enumerators) `S_0` to `S_8` of type `my_state`.
	- We can now create variables of type `my_state`. This is `current_state` for holding the current state of the FSM and `next_state` which holds the value that it will transition to after processing a certain condition.
	- Note that these variables can only contain the values that we have associated to them in the list provided.
- ##### State transition:
	- This is a simpler block where we say that if `rst` is high, then set the current state to `S_0`, the default or base case.
	- Otherwise set `current_state = next_state`
- ##### Next State Logic:
	- This implements a switch case statement. In this case there are no other conditions associated with going to the next state (as can be seen in the FSM diagram) apart from when `en` is high.
	- We set the default case to `S_0`
- ##### Output Logic:
	- For each state, we have a specific 8 bit output, with each bit associated with each LED ranging from `0000 0000` in `S_0` to `1111 1111` in `S_8`
For the state transition, we use `always_ff` since this is sequential logic.
Whereas we use `always_comb` since this is combinational logic. Hence we use blocking assignment here (`=`) as opposed to non-blocking in sequential logic (`<=`).

Then we design the test bench which:
- Sets up the environment and vbuddy
- Initialises the inputs
- Runs the simulation cycle
	- Inside this it utilises the following code to light up the LED strip. Since `vbdBar()` takes an unsigned 8-bit parameter between 0 and 255, we need to implement a mask of `0xFF`
```c++
      vbdBar(top->data_out & 0xFF);
```

Finally making the required changes to `doit.sh`, we are ready to test the light strip, which works as expected. 

We can add a 1 second delay in C++ using the following libraries and the line:
```c++
#include <iostream>
#include <chrono>
#include <thread>

std::this_thread::sleep_for(std::chrono::seconds(1));
```




---
## Task 3

---
## Task 4
