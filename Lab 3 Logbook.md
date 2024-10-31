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
- #### Parameters: 
	- The parameters are `rst`, `en`, `clk` (all 1 bit inputs) and `data_out` (an 8 bit output). Where `data_out` will be used to reflect the changes to the light strip so is 8 bits with one for the state of each LED.
- #### Defining states:
	- We need to define the states. Since this isn't built in, ,we will use `typedef` to define a new type which will be a list of named constants (enumerators) `S_0` to `S_8` of type `my_state`.
	- We can now create variables of type `my_state`. This is `current_state` for holding the current state of the FSM and `next_state` which holds the value that it will transition to after processing a certain condition.
	- Note that these variables can only contain the values that we have associated to them in the list provided.
- #### State transition:
	- This is a simpler block where we say that if `rst` is high, then set the current state to `S_0`, the default or base case.
	- Otherwise set `current_state = next_state`
- #### Next State Logic:
	- This implements a switch case statement. In this case there are no other conditions associated with going to the next state (as can be seen in the FSM diagram) apart from when `en` is high.
	- We set the default case to `S_0`
- #### Output Logic:
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

We can make this more efficient by using this style:
``` systemVerilog
always_comb begin
    if (en) begin
        case (current_state)
            S_0:    next_state = S_1;
            S_1:    next_state = S_2;
            S_2:    next_state = S_3;
            S_3:    next_state = S_4;
            S_4:    next_state = S_5;
            S_5:    next_state = S_6;
            S_6:    next_state = S_7;
            S_7:    next_state = S_8;
            S_8:    next_state = S_0;
            default: next_state = S_0;
        endcase
    end else begin
        next_state = current_state; // Remain in the current state if `en` is low
    end
end
```

---
## Task 3 - Exploring the clktick.sv and the delay.sv modules
We now aim to create a delay using System Verilog and not use any in built functions in C++.

This is achieved by using `clktick.sv` and `delay.sv`.
The test bench flashes the LEDs at a rate determined by N, which needs to be calibrated as per the device's clock to achieve a 1 second time period.

Breaking down `clktick.sv`, we see that it has the interface signals, `clk`, `rst`, `en`, `N`, `tick` (with only tick as the output).
It also utilises `count` although it is local to the file.
This makes use of sequential logic to:
- set `tick` to `0` and `count` to `N` is `rst` is high.
- otherwise, if `en` is high, then:
	- if `count` = 0, `tick` = `1`, `count` = `N`
	- if `count` != 0, `tick` = `0`, `count` is decremented by `1`

Setting the metronome to 60bpm, we can acquire a value for `N` as `29`. This is equivalent to a tick period of 1 second.

The next step is to implement the following design which combines `clktick.sv` with `f1_fsm.sv` to add a manual 1 second delay as opposed to using C++ libraries.

<p align="center"> <img src="images/f1_sequence.jpg" /> </p>

Previously we created `f1_fsm_tb.cpp` and `f1_fsm.sv` in task 2 and `clktick.sv` and `clktick_tb.cpp` were provided in the previous step.
As per the diagram above, we create a `top.sv` file which combines these two together.
The key components of each file can be seen below:

For f1_fsm (test bench):
```c++
vbdBar(top->data_out & 0xFF);
```

For clktick.sv:
```systemVerilog
if (rst) begin
        tick <= 1'b0;
        count <= N;  
        end
    else if (en) begin
        if (count == 0) begin
            tick <= 1'b1;
            count <= N;
            end
        else begin
            tick <= 1'b0;
            count <= count - 1'b1;
            end
        end
```

We can merge these together by using `top.sv`  with the key line connecting both being:
```systemverilog
    .en         (tick),
```
So whenever `tick` is high, f1_fsm is run and goes to the next state.

The test bench only contains the line as above (from f1_fsm) but all variables from f1_fsm and `clktick` are initialised.

---
## Task 4 - Full implementation of F1 starting light
The final part of labs combines all the previous tasks. It uses `lfsr.sv` whose output is the input to `delay.sv`. We then use a MUX and depending on the value of `cmd_seq`, there is either a 1 second delay using `clktick.sv` (if 1) or there is a `trigger` signal and counts down for a specific number of clock cycles.
This needs to match the following diagram:
<p align="center"> <img src="images/F1_full.jpg" /> </p>

#### `delay.sv`
When `trigger` is asserted, it starts counting for `K` clock cycles, then `time_out` goes high for 1 clock cycle.
The differences between this and `clktick.sv` are:
1. `trigger` which is edge driven instead of `en`
2. FSM can only be triggered after `trigger_signal` is 0

#### `f1_fsm.sv`
This now needs to include a trigger input with 2 additional output signals:
1. `cmd_seq` is high during the sequential logic execution of `data_out[7:0]`
2. `cmd_delay` triggers the start of `delay.sv`

The 7 bit LFSR may be needed to provide a random delay for all 7 bits to go from `S_8` (all LED on) to `S_0` (all LED off). This is randomised since LFSR utilises a Pseudo Random Binary Sequence (PRBS).

#### Changes to test bench
This is to measure the reaction time
1. When all lights are OFF, after a random delay, `vbdInitWathc()` function is used to start the stopwatch.
2. The user reacts to this by pressing the switch as quickly as possible and Vbuddy records the elapsed time.
3. The test bench calls `vbdElapsed()` to read the reaction time in milliseconds.
4. The test bench reports this by sending it to Vbuddy as a message on the TFT screen.

