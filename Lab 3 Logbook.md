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
- creating `lfsr.sv`:
```verilog
module lfsr(
    input   logic       clk,
    input   logic       rst,
    input   logic       en,
    output  logic [3:0] data_out
);

    logic   [3:0]   sreg;

always_ff @ (posedge clk, posedge rst)
    if (rst) sreg <= 4'b1;
    else sreg <= {sreg[2:0], sreg[3] ^ sreg[2]};
    
assign data_out = sreg;

endmodule
```

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


---
## Task 3

---
## Task 4
