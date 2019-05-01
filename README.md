# SIR with vaccination
Original code:
https://github.com/pholme/sir

To get it running you need:

1. Make dictionary o (for complier object files)
2. Compile it by make (it has some generic optimization flags now)
3. Run it like

./sir nwk/iceland.lnk 0.6 1.5

The third argument is the vaccination coverage. It should be between 0 to 1, where 1 denotes every node gets vaccinated against the infection. 

