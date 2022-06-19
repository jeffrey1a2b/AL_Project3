# AL_Project3
## Manual
Input assembly code.
<br/>
Please input only code, **DO NOT** enter address or note.
<br />
There are sample I/O in the file "sample_output".
<br />
A .txt file with the video link.
<br/>

## Code Intro<br/>

### Variables:<br/>

**Line 15~19 Insts:** for each operator cycles.<br/>
**Line 20 Insts:** for numbers of REG<br/>
**Line 29 Insts:** to check if the current cycle did changes.<br/>
**Line 30 Insts:** for current cycle.<br/>
**Line 31 Insts:** for each input instruction.<br/>
**Line 32 Rs1, Rs2, Rd, Rd_Lable:** are for instruction input,<br/>
then store in struct Inst, last, push into Insts.<br/>
**Line 33 inst_num, insts:**<br/>
stores total instructions, currently used instruction.<br/>
**Line 42 RF:** for Regester storing(regester file).<br/>
**Line 43 rat:** are for RAT,<br/>
structure with the RS be waited, and if this RAT is empty.<br/>
**Line 50 RS:** are a structure of RS,<br/>
with RD => REG destiny,<br/>
buzy => check if this RS is buzy(not empty),<br/>
rs_num => No. of RS,<br/>
type => insturction type,<br/>
rs1, rs2=> for number of RF,<br/>
RS1, RS2=> for waited RS number,<br/>
dispatch => check if it dispatch(execution stage),<br/>
ready => check if it's ready to dispatch,<br/>
than store in RS_ADD, RS_MUL.<br/>

**Line 77 BUFFER:** are a structure of buffer,<br/>
with two cycle time, where it begin and end,<br/>
and shows which RS it come from,<br/>
also show if it's empty(busy),<br/>
divide for ADD and MUL buffers.<br/>

============================================================<br/>

### Functions:<br/>
**fetch(string):** store & decode each instruction into Insts.<br/>
**issue():** get issues.<br/>
**exe():** updateRS & check if anything ready to exe (change RS.ready).<br/>
**Wr():** check if anything need to Write back (done exe).<br/>
**print():** print out each action.<br/>
**end():** Check if program should ended.<br/>
============================================================<br/>
## END
