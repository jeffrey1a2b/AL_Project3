# AL_Project3
## Manual
Input interger N for number of entry.
Then input assembly code.
<br/>
Please input only code, **DO NOT** enter address or note.
<br />
There are sample I/O in the file "sample_output".
<br />
A .txt file with the video link.
<br/>

## Code Intro<br/>

### Variables:<br/>
**Line 9 Insts:** for each input instruction.<br/>
**Line 10,11 Rs1, Rs2, Rd, Rd_Lable:** are for instruction input,<br/>
then store in struct Inst, last, push into Insts.<br/>
**Line 12,13 entry_num, entry, inst_num, PC, next_PC:**<br/>
stores number of entry, total instructions,<br/>
currently used entry, and PC & next_PC.<br/>
**Line 15 history:** for history, then store in each entry's BTB.<br/>
**Line 16 Lables:**, a table for Labels, shows which address should PC be.<br/>
**Line 17 BTB:**, struct include one BTB with 3-bit-history, 8BCs,<br/>
number of misprediction, and wether it has been used.<br/>
Then push into BTBs.<br/>
**Line 34 REG[32]:** for Regester storing.<br/>
============================================================<br/>

### Functions:<br/>
**fetch(string):** store & decode each instruction into Insts.<br/>
**predict():** predict T/N by using BTBs.<br/>
**outcome():** execute instruction write REG than show T/N.<br/>
**print():** print out each action.<br/>
**update(char,char):** Compare prediction & outcome than update BTBs.<br/>
============================================================<br/>
## END

