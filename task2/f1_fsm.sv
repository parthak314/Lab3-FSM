module f1_fsm (
    input   logic       rst,
    input   logic       en,
    input   logic       clk,
    output  logic [7:0] data_out
);
    // define states
    typedef enum {S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, S_8} my_state;
    my_state current_state, next_state;

    // state transition
    always_ff @(posedge clk)
        if (rst)    current_state <= S_0;
        else        current_state <= next_state;

    // next state logic
    always_comb
        case (current_state)
            S_0:    if (en)     next_state = S_1;
                    else        next_state = current_state;
            S_1:    if (en)     next_state = S_2;
                    else        next_state = current_state;
            S_2:    if (en)     next_state = S_3;
                    else        next_state = current_state;
            S_3:    if (en)     next_state = S_4;
                    else        next_state = current_state;
            S_4:    if (en)     next_state = S_5;
                    else        next_state = current_state;
            S_5:    if (en)     next_state = S_6;
                    else        next_state = current_state;
            S_6:    if (en)     next_state = S_7;
                    else        next_state = current_state;
            S_7:    if (en)     next_state = S_8;
                    else        next_state = current_state;
            S_8:    if (en)     next_state = S_0;
                    else        next_state = current_state;
            default: next_state = S_0;
        endcase
    
    // output logic
    always_comb
        case (current_state)
            S_0:    data_out = 8'b0000_0000;
            S_1:    data_out = 8'b0000_0001;
            S_2:    data_out = 8'b0000_0011;
            S_3:    data_out = 8'b0000_0111;
            S_4:    data_out = 8'b0000_1111;
            S_5:    data_out = 8'b0001_1111;
            S_6:    data_out = 8'b0011_1111;
            S_7:    data_out = 8'b0111_1111;
            S_8:    data_out = 8'b1111_1111;
            default: data_out = 8'b0;
        endcase

endmodule
