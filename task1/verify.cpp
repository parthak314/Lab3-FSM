#include "gtest/gtest.h"
#include "Vdut.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

#include <memory>

class TestDut : public ::testing::Test
{
public:
    void SetUp() override
    {
        top = std::make_unique<Vdut>();
        tfp = std::make_unique<VerilatedVcdC>();

        Verilated::traceEverOn(true);
        top->trace(tfp.get(), 99);
        tfp->open("waveform.vcd");

        initializeInputs();
    }

    void TearDown() override
    {
        top->final();
        tfp->close();
    }

    void initializeInputs()
    {
        top->clk = 0;
        top->rst = 0;
        // top->en = vbdFlag();
        top->en = 1;
    }

    // Runs the simulation for a clock cycle, evaluates the DUT, dumps waveform.
    void runSimulation()
    {
        for (int clk = 0; clk < 2; clk++)
        {
            top->eval();
            tfp->dump(2 * ticks + clk);
            top->clk = !top->clk;
        }
        ticks++;

        if (Verilated::gotFinish())
        {
            exit(0);
        }
    }

protected:
    unsigned int ticks = 0;
    std::unique_ptr<Vdut> top;
    std::unique_ptr<VerilatedVcdC> tfp;
};

TEST_F(TestDut, InitialStateTest)
{
    top->rst = 1;
    runSimulation();
    EXPECT_EQ(top->data_out, 0b0001);
}

TEST_F(TestDut, SequenceTest)
{
    top->rst = 1;
    runSimulation();
    top->rst = 0;

    std::vector<int> expected = {
        0b0001,
        0b0010,
        0b0100,
        0b1001,
        0b0011,
        0b0110,
        0b1101,
        0b1010,
        0b0101,
        0b1011,
        0b0111,
        0b1111,
        0b1110,
        0b1100,
        0b1000,
        0b0001};

    for (int exp : expected)
    {
        EXPECT_EQ(top->data_out, exp);
        runSimulation();
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    return res;
}