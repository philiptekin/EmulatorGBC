Question
How should I represent the Game Boy CPU registers?

Hardware facts
The hardware has individual 8-bit registers and paired 16-bit views.

Options
8-bit canonical state with pair accessors; 16-bit pair storage with byte accessors.

Current decision
Use 8-bit register members and pair helpers initially.

Revisit when
Revisit if the representation makes instruction implementation or correctness testing materially more difficult.
