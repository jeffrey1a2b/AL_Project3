#include<iostream>
#include<string>
#include<queue>
#include<map>
#include<sstream>

using namespace std;

/*  寫定5個RS,Add*3,Mul*2  */
/*
   Write back當個Cycle已經寫回，可以立即使用。
   Write back 最快發生於結束exe的下一個Cycle。
*/

int ADD_cycles = 2;
int SUB_cycles = 2;
int ADDI_cycles = 2;
int MUL_cycles = 10;
int DIV_cycles = 20;
const int REG_num = 5;

void fetch(string);     // storing insts
void issue();
void exe();
void Wr();
void print();
bool end();

bool Change = false;
int Cycle = 1;
string IN; // each input inst
int Rs1 = 0, Rs2 = 0, Rd = 0, Im = 0;
int inst_num = 0, insts = 0;
struct Inst
{
	int address = 0;
	string type;
	int rs1 = 0, rs2 = 0, rd = 0, im = 0;
};
vector<Inst>Insts; // all insts

int RF[REG_num] = {0,2,4,6,8};
struct rat
{
	int rat_waited = 0;  // number of RS
	bool empty = true;
};
vector<rat> RAT;

struct RS
{
	int Rd = 0;
	bool buzy = false;
	int rs_num = 0;
	string type = "";
	int rs1 = 0, rs2 = 0;     // value from RF
	int RS1 = -1, RS2 = -1;    // RS number waited
	bool dispatch = false;    // in dispatch stage
	bool ready = false;       // ready or not to exe
};
vector<RS>RS_ADD;      // 3 lines
vector<RS> RS_MUL;     // 2 lines
void clean_RS(RS &in) 
{
	in.Rd = 0;
	in.buzy = false;
	in.rs_num = 0;
	in.type = "";
	in.rs1 = 0;
	in.rs2 = 0;
	in.RS1 = 0;
	in.RS2 = 0;
	in.dispatch = false;
	in.ready = false;
}

struct BUFFER          // for execute#
{
	int cycle_start = 0, cycle_end = 0; // check if done exe
	int which_RS = 0;
	bool empty = true;
};
BUFFER buffer_add;
BUFFER buffer_mul;

void fetch(string IN)
{
	stringstream ss(IN);
	string s;
	Inst temp_inst;
	temp_inst.address = inst_num;
	ss >> s;
	//cout << s;

	temp_inst.type = s;
	char C;
	int N = 0;
	if (s == "addi") //addi r2 ,r2 ,-1
	{
		ss >> C >> N;
		temp_inst.rd = N;
		//==================
		ss >> C >> C >> N;
		temp_inst.rs1 = N;
		//==================
		ss >> C >> N;
		temp_inst.im = N;
	}
	else // (s == "add","sub","mul","div")
	{
		ss >> C >> N;
		temp_inst.rd = N;
		//==================
		ss >> C >> C >> N;
		temp_inst.rs1 = N;
		//==================
		ss >> C >>C>> N;
		temp_inst.rs2 = N;
	}

	Insts.push_back(temp_inst);
}

void issue()
{
	if (insts < inst_num)
	{
		if (Insts[insts].type == "addi")
		{
			for (auto& I : RS_ADD)
			{
				if (!I.buzy)
				{
					Change = true;
					I.buzy = true;
					I.Rd = Insts[insts].rd;
					I.type = Insts[insts].type;
					I.rs1 = Insts[insts].rs1;
					I.rs2 = Insts[insts].im;
					I.RS2 = -1;
					if (RAT[I.rs1].empty)
						I.RS1 = -1, I.ready = true;
					else
						I.RS1 = RAT[I.rs1].rat_waited;
					RAT[I.Rd].rat_waited = I.rs_num;
					RAT[I.Rd].empty = false;
					insts++;
					break;
				}
			}
		}
		else if (Insts[insts].type == "add" || Insts[insts].type == "sub")
		{
			for (auto& I : RS_ADD)
			{
				if (!I.buzy)
				{
					Change = true;
					I.buzy = true;
					I.Rd = Insts[insts].rd;
					I.type = Insts[insts].type;
					I.rs1 = Insts[insts].rs1;
					I.rs2 = Insts[insts].rs2;
					if (RAT[I.rs1].empty)
					{
						I.RS1 = -1;
					}
					else
						I.RS1 = RAT[I.rs1].rat_waited;
					if (RAT[I.rs2].empty)
					{
						I.RS2 = -1;
					}
					else
						I.RS2 = RAT[I.rs2].rat_waited;
					if (I.RS1==-1 && I.RS2==-1)
						I.ready = true;
					RAT[I.Rd].rat_waited = I.rs_num;
					RAT[I.Rd].empty = false;
					insts++;
					break;
				}
			}
		}
		else if (Insts[insts].type == "mul" || Insts[insts].type == "div")
		{
			for (auto& I : RS_MUL)
			{
				if (!I.buzy)
				{
					Change = true;
					I.buzy = true;
					I.Rd = Insts[insts].rd;
					I.type = Insts[insts].type;
					I.rs1 = Insts[insts].rs1;
					I.rs2 = Insts[insts].rs2;
					if (RAT[I.rs1].empty)
					{
						I.RS1 = -1;
					}
					else
						I.RS1 = RAT[I.rs1].rat_waited;
					if (RAT[I.rs2].empty)
					{
						I.RS2 = -1;
					}
					else
						I.RS2 = RAT[I.rs2].rat_waited;
					if (I.RS1 == -1 && I.RS2 == -1)
						I.ready = true;
					RAT[I.Rd].rat_waited = I.rs_num;
					RAT[I.Rd].empty = false;
					insts++;
					break;
				}
			}
		}
	}
}

void exe()
{
	// update RS by RAT
	for (auto& I : RS_ADD)
	{
		bool flag = true;
		if (I.buzy && !I.ready)
		{
			if (I.type == "addi")
			{
				for (auto& i : RAT)
					if (!i.empty)
					{
						if (i.rat_waited == I.RS1)
						{
							I.ready = false;
							flag = false;
							break;
						}
					}
				if (flag)
					I.RS1 = -1;
			}
			else
			{
				for (auto& i : RAT)
				{
					if (!i.empty)
					{
						if (i.rat_waited == I.RS1)
						{
							I.ready = false;
							flag = false;
							break;
						}
					}
				}
				if (flag)
					I.RS1 = -1;
				flag = true;
				for (auto& i : RAT)
				{
					if (!i.empty)
					{
						if (i.rat_waited == I.RS2)
						{
							I.ready = false;
							flag = false;
							break;
						}
					}
				}
				if (flag)
					I.RS2 = -1;

				if (I.RS1 == -1 && I.RS2 == -1)
					I.ready = true;
			}
		}
	}
	for (auto& I : RS_MUL)
	{
		bool flag = true;
		if (I.buzy && !I.ready)
		{
			for (auto& i : RAT)
			{
				if (!i.empty)
				{
					if (i.rat_waited == I.RS1)
					{
						I.ready = false;
						flag = false;
						break;
					}
				}
			}
			if (flag)
				I.RS1 = -1;
			flag = true;
			for (auto& i : RAT)
			{
				if (!i.empty)
				{
					if (i.rat_waited == I.RS2)
					{
						I.ready = false;
						flag = false;
						break;
					}
				}
			}
			if (flag)
				I.RS2 = -1;
			
			if (I.RS1 == -1 && I.RS2 == -1)
				I.ready = true;
		}
	}
	// dispatch
	if (buffer_add.empty)
	{
		for (auto& I : RS_ADD)
		{
			if (I.buzy)
				if (I.ready && !I.dispatch)
				{
					Change = true;
					I.dispatch = true;
					// into buffer
					buffer_add.empty = false;
					buffer_add.which_RS = I.rs_num;
					buffer_add.cycle_start = Cycle;
					if (I.type == "add")
						buffer_add.cycle_end = buffer_add.cycle_start + ADD_cycles - 1;
					else if (I.type == "addi")
						buffer_add.cycle_end = buffer_add.cycle_start + ADDI_cycles - 1;
					else if (I.type == "sub")
						buffer_add.cycle_end = buffer_add.cycle_start + SUB_cycles - 1;
					break;
				}
		}
	}
	if (buffer_mul.empty)
	{
		for (auto& I : RS_MUL)
		{
			if (I.buzy)
				if (I.ready && !I.dispatch)
				{
					Change = true;
					I.dispatch = true;
					// into buffer
					buffer_mul.empty = false;
					buffer_mul.which_RS = I.rs_num;
					buffer_mul.cycle_start = Cycle;
					if (I.type == "mul")
						buffer_mul.cycle_end = buffer_mul.cycle_start + MUL_cycles - 1;
					else if (I.type == "div")
						buffer_mul.cycle_end = buffer_mul.cycle_start + DIV_cycles - 1;
					break;
				}
		}
	}
}

void Wr()
{
	if (!buffer_add.empty)
	{
		if (buffer_add.cycle_end < Cycle)
		{
			Change = true;
			int I = buffer_add.which_RS;
			if (RS_ADD[I].type == "addi")
			{
				RF[RS_ADD[I].Rd] = RF[RS_ADD[I].rs1] + RS_ADD[I].rs2;
			}
			else if (RS_ADD[I].type == "add")
			{
				RF[RS_ADD[I].Rd] = RF[RS_ADD[I].rs1] + RF[RS_ADD[I].rs2];
			}
			else if (RS_ADD[I].type == "sub")
			{
				RF[RS_ADD[I].Rd] = RF[RS_ADD[I].rs1] - RF[RS_ADD[I].rs2];
			}
			for (auto& I : RAT)
			{
				if (!I.empty)
					if (I.rat_waited == buffer_add.which_RS)
						I.empty = true;
			}
			clean_RS(RS_ADD[I]);
			buffer_add.empty = true;
		}
	}

	if (!buffer_mul.empty)
	{
		if (buffer_mul.cycle_end < Cycle)
		{
			Change = true;
			int I = buffer_mul.which_RS - 3;
			if (RS_MUL[I].type == "mul")
			{
				RF[RS_MUL[I].Rd] = RF[RS_MUL[I].rs1] * RF[RS_MUL[I].rs2];
			}
			else if (RS_MUL[I].type == "div")
			{
				RF[RS_MUL[I].Rd] = RF[RS_MUL[I].rs1] / RF[RS_MUL[I].rs2];
			}
			for (auto& I : RAT)
			{
				if(!I.empty)
				if (I.rat_waited == buffer_mul.which_RS)
					I.empty = true;
			}
			clean_RS(RS_MUL[I]);
			buffer_mul.empty = true;
		}
	}
	// clear current RAT, RS and buffer at last

}

void print()
{
	cout << "Cycle : " << Cycle << '\n';

	cout << "____RF___" << '\n';
	for (int i = 0; i < REG_num; i++)
		cout << "F"<<i<<" : " << RF[i] << '\n';
	cout << "\n\n";
	cout << "____RAT___" << '\n';
	for (int i = 0; i < REG_num; i++)
	{
		cout << "F" << i << " : ";
		if (!RAT[i].empty)
			cout << "RS" << RAT[i].rat_waited;
		cout << '\n';
	}
	cout << "\n\n";
	cout << "____RS_______________" << '\n';
	for (int i = 0; i <= 2; i++)
	{
		cout << "RS" << i<<"  ";
		if (RS_ADD[i].type == "addi")
		{
			cout << "+I  ";
			if (RS_ADD[i].RS1 != -1)
			{
				cout << "RS" << RS_ADD[i].RS1 << "  " << RS_ADD[i].rs2 << '\n';
			}
			else
			{
				cout << "  " << RF[RS_ADD[i].rs1] << "  " << RS_ADD[i].rs2 << '\n';
			}
		}
		else if (RS_ADD[i].buzy)
		{
			if (RS_ADD[i].type == "add")
				cout << "+   ";
			else
				cout << "-   ";
			if (RS_ADD[i].RS1 != -1)
			{
				cout << "RS" << RS_ADD[i].RS1;
			}
			else
			{
				cout << "  " << RF[RS_ADD[i].rs1];
			}
			if (RS_ADD[i].RS2 != -1)
			{
				cout << " RS" << RS_ADD[i].RS2 << '\n';
			}
			else
			{
				cout << "  " << RF[RS_ADD[i].rs2] << '\n';
			}
		}
		else
			cout << '\n';
	}
	cout << "BUFFER: ";
	if (buffer_add.empty)
		cout << "empty" << '\n';
	else
	{
		cout << "(RS" << buffer_add.which_RS << ") " << RF[RS_ADD[buffer_add.which_RS].rs1];
		int I = buffer_add.which_RS;
		if (RS_ADD[I].type == "addi")
		{
			cout<<" + "<<RS_ADD[I].rs2;
		}
		else if (RS_ADD[I].type == "add")
		{
			cout << " + " << RF[RS_ADD[I].rs2];
		}
		else if (RS_ADD[I].type == "sub")
		{
			cout << " - " << RF[RS_ADD[I].rs2];
		}
		cout << '\n';
	}


	cout << "\n\n_____________________\n";
	for (int i = 0; i <= 1; i++)
	{
		cout << "RS" << i+3 << "  ";
		if (RS_MUL[i].buzy)
		{
			if (RS_MUL[i].type == "mul")
				cout << "*   ";
			else
				cout << "/   ";
			if (RS_MUL[i].RS1 != -1)
			{
				cout << "RS" << RS_MUL[i].RS1;
			}
			else
			{
				cout << "  " << RF[RS_MUL[i].rs1];
			}
			if (RS_MUL[i].RS2 != -1)
			{
				cout << " RS" << RS_MUL[i].RS2 << '\n';
			}
			else
			{
				cout << "  " << RF[RS_MUL[i].rs2] << '\n';
			}
		}
		else cout << '\n';
	}
	cout << "BUFFER: ";
	if (buffer_mul.empty)
		cout << "empty" << '\n';
	else
	{
		cout << "(RS" << buffer_mul.which_RS << ") " << RF[RS_MUL[buffer_mul.which_RS-3].rs1];
		int I = buffer_mul.which_RS - 3;
		if (RS_MUL[I].type == "mul")
		{
			cout << " * " <<RF[RS_MUL[I].rs2];
		}
		else if (RS_MUL[I].type == "div")
		{
			cout << " / "<<RF[RS_MUL[I].rs2];
		}
		cout << '\n';
	}
	cout << "=========================\n\n";
}

bool end()
{
	if (insts < inst_num)
		return false;
	for (auto& I : RS_ADD)
	{
		if (I.buzy == true)
			return false;
	}
	for (auto& I : RS_MUL)
	{
		if (I.buzy == true)
			return false;
	}
	for (auto& I : RAT)
	{
		if (I.empty == false)
			return false;
	}

	return true;
}

int main()
{
	// Preprocessing
	RS Temp_RS;
	for (int i = 0; i <= 2; i++) // 3 RS for Add
		Temp_RS.rs_num = i, RS_ADD.push_back(Temp_RS);
		
	for (int i = 3; i <= 4; i++) // 2 RS for Mul
		Temp_RS.rs_num = i, RS_MUL.push_back(Temp_RS);
	rat Temp_RAT;
	for (int i = 0; i < 5; i++) // 5 RAT for 5 REG
		RAT.push_back(Temp_RAT);

	// Enter code
	cout << "Enter assembly code:(finish by EOF)\n";
	inst_num = 0;
	while (getline(cin, IN)) {
		fetch(IN);
		inst_num++;
	}
	cout << "Insts num: " << inst_num << '\n' <<
		"========= Start =========" << '\n';
	// Run
	while (!end())
	{
		Change = false;
		Wr(); // check if anything need to Write back (done exe)
		exe(); // updateRS & check if anything ready to exe (change RS.ready)
		issue(); //  get issue
		if (Change)
			print();
		Cycle++;
	}
	cout << "=====END=====\n";
	return 1;
}

/*
F0=0
F1=2
F2=4
F3=6
F4=8
*/
/*
addi R1 ,R0 ,3
addi R2 ,R0 ,5
mul R4 ,R1 ,R2
sub R3 ,R2 ,R4
*/
/*
addi R1 ,R0 ,3     3
addi R2 ,R0 ,5     5
mul R4 ,R1 ,R2     15
sub R3 ,R2 ,R4     -10
*/
/*
F0=0
F1=3
F2=5
F3=-10
F4=15
*/