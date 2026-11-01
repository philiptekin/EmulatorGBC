# ADR-0002: Represent CPU Registers Individually

**Status:** Proposed
**Date:** 2026-10-25

## Context

The Game Boy CPU has 8-bit registers that can be combined into 16-bit register pairs, alongside the 16-bit stack pointer (SP) and program counter (PC).

The emulator needs to support instructions that operate on individual registers, register pairs, and memory addresses.

## Decision

Represent the individual 8-bit registers explicitly. Represent SP and PC as 16-bit values. Provide helper methods to read and write the 16-bit register pairs (AF, BC, DE, HL).

## Consequences

- Instructions that operate on individual registers are straightforward to implement.
- Register-pair operations use a consistent set of helpers.
- Pair helpers centralize bit shifting and masking.
- The F register must preserve the Game Boy CPU's flag-register constraints, including keeping its lower four bits clear.
