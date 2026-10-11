# DRY principle broken

Right now there is a lot of duplicate code for each memory part. I have decided to keep that because i do not have enough knowledge of how the different memory types behave, so for now the same implementation of read, write is the same.

## PROS
The pros of this is that i can extend each memory type in the future to their appropritate separate functionality and own tests.

## CONS
The cons is that i have a lot, and i mean a lot of duplicate code, from the bus, to the tests and different memory implementations doing the same things.

## TODO
 In the future if i realise that all memory types behave the same, i want to make a template class that is called memory, and you specify what you want through that, or just use inheritance with a memory class. But either way reducing dry if i can is important.
