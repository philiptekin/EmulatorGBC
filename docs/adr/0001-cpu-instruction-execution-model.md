# ADR-0001: Use Instruction-at-a-Time CPU Execution Initially

- **Status:** Proposed
- **Date:** 2026-10-25

## Context

The emulator needs a CPU execution model that determines how instructions are executed and how elapsed time is communicated to other hardware components.

Two main approaches are being considered:

1. Execute one complete instruction at a time.
2. Execute instructions in smaller steps, allowing other hardware components to advance between those steps.

The goal of the initial implementation is to learn CPU emulation, establish clear component responsibilities, and build a testable foundation without introducing timing complexity before it is needed.

## Options Considered

### Option 1: Instruction-at-a-time execution

The CPU fetches and executes one complete instruction, then reports the number of T-cycles it consumed. The system coordinator advances timed hardware components using that elapsed time.

**Advantages**
- Simple execution model.
- Easier to implement and debug individual instructions.
- Supports focused unit tests for instruction behaviour and timing.
- Allows the CPU to be developed before more complex hardware timing is implemented.

**Disadvantages**
- Intermediate events within an instruction are not naturally represented.
- Hardware interactions that depend on the precise timing of individual memory accesses may be inaccurate.
- A more detailed execution model may eventually be needed.

### Option 2: Fine-grained, cycle-stepped execution

The CPU performs smaller operations and coordinates elapsed time with other hardware as execution progresses.

**Advantages**
- Allows more precise coordination between CPU operations and hardware timing.
- Provides a better foundation for behaviours that depend on the timing of individual memory accesses.

**Disadvantages**
- More complex execution logic and state management.
- More difficult to implement and debug initially.
- Requires a clearer model of how CPU operations and hardware advancement are interleaved.

## Decision

The initial CPU implementation will use instruction-at-a-time execution.

`Cpu::step()` will execute one complete instruction and return the elapsed time in T-cycles.

The system coordinator will use the returned T-cycle count to advance timed hardware components after each CPU instruction.

The CPU will access memory through the bus rather than directly accessing individual memory devices.

A finer-grained execution model will not be implemented unless concrete requirements or test results demonstrate that it is needed.

## Consequences

### Positive

- The CPU can be implemented and tested incrementally.
- Instruction behaviour, register changes, memory effects, and elapsed time can be tested independently.
- The initial design avoids introducing timing machinery before its requirements are understood.

### Negative

- Hardware components advance at instruction boundaries rather than necessarily at the exact point of an event within an instruction.
- Timing-sensitive behaviours, including some interactions with the PPU, memory access restrictions, and interrupts, may not be accurately represented.
- Improving timing accuracy may require changes to the CPU execution interface and system coordination.

### Revisit this decision when

- Tests demonstrate failures caused by timing-sensitive behaviour within an instruction.
- A hardware interaction requires the PPU, timer, or another component to advance between individual CPU operations.
- The limitations of instruction-at-a-time execution prevent the desired level of accuracy.

When revisiting the decision, compare the concrete requirements against the complexity of a finer-grained execution model. If the architectural decision changes after this ADR has been accepted, record the new decision in a separate ADR that supersedes this one.
