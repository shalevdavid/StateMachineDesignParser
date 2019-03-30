#include <iostream>
using std::cout;
using std::endl;

#include <cstring>
#include <string>
#include <fstream>
using std::ifstream;
using std::ofstream;

///// Definitions of Text File Format /////
#define ID_COL 1
#define EVENT_COL 0
#define TYPE_COL 2
#define DISP_COL 5
///////////////////////////////////////////

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = "|";

//////////////////////////////////////////////////////////////////////////////////

enum STATES{
	STATE_DEL_NO_DISP,
	STATE_DEL_DISP,
	STATE_NEW_REC_NO_DISP,
	STATE_NEW_REC_DISP,
	STATE_UP_NO_DISP,
	STATE_UP_DISP_GUY,
	STATE_DISP_ALREADY,
	STATE_DISP_STOP_STILL_UP,
	STATE_NONE	//Fail has occurred
};

enum ACTIONS{
	ACTION_NEW_NO_DISP,
	ACTION_NEW_DISP,
	ACTION_UP_NO_DISP,
	ACTION_UP_DISP,
	ACTION_DELETE_NO_DISP,
	ACTION_DELETE_DISP,
	ACTIONS_FINISH,
	ACTIONS_NONE	//Fail has occurred
};

struct DATA{
};

// Abstract calss only for inheritance
class FSMstate {
	protected:
		
	public:
		FSMstate(){}
		virtual ~FSMstate(){}
		virtual void performState(int& newRecordsCounter, ofstream& fout) = 0;
};

// State 0
class stateDelNoDisp: public FSMstate
{
	public:
		void performState(int& newRecordsCounter, ofstream& fout)
		{
			std::cout<<"newRecordsCounter: "<<newRecordsCounter<<". state Del No Disp was Performed - state 0"<<endl;
		}
};

// State 1
class stateDelDisp: public FSMstate
{
	public:
		void performState(int& newRecordsCounter, ofstream& fout)
		{
			std::cout<<"newRecordsCounter: "<<newRecordsCounter<<". state Del Disp was Performed - state 1: Relevant For Recording"<<endl<<endl;
			fout<<"newRecordsCounter: "<<newRecordsCounter<<". state Del Disp was Performed - state 1: Relevant For Recording"<<endl<<endl;
		}
};

// State 2
class stateNewRecNoDisp: public FSMstate
{
	public:
		void performState(int& newRecordsCounter, ofstream& fout)
		{
			newRecordsCounter++;
			std::cout<<"newRecordsCounter: "<<newRecordsCounter<<". state New Rec No Disp was Performed - state 2"<<endl;
		}
};

// State 3
class stateNewRecDisp: public FSMstate
{
	public:
		void performState(int& newRecordsCounter, ofstream& fout)
		{
			newRecordsCounter++;
			std::cout<<"newRecordsCounter: "<<newRecordsCounter<<". state New Rec Disp was Performed - state 3: Relevant For Recording"<<endl;
			fout<<"newRecordsCounter: "<<newRecordsCounter<<". state New Rec Disp was Performed - state 3: Relevant For Recording"<<endl;
		}
};

// State 4
class stateUpNoDisp: public FSMstate
{
	public:
		void performState(int& newRecordsCounter, ofstream& fout)
		{
			std::cout<<"newRecordsCounter: "<<newRecordsCounter<<". state Up No Disp was Performed - state 4"<<endl;
		}
};

// State 5
class stateUpDispGuy: public FSMstate
{
	public:
		void performState(int& newRecordsCounter, ofstream& fout)
		{
			std::cout<<"newRecordsCounter: "<<newRecordsCounter<<". state Up Disp Guy was Performed - state 5: Relevant For Recording"<<endl;
			fout<<"newRecordsCounter: "<<newRecordsCounter<<". state Up Disp Guy was Performed - state 5: Relevant For Recording"<<endl;
		}
};

// State 6
class stateDispAlready: public FSMstate
{
	public:
		void performState(int& newRecordsCounter, ofstream& fout)
		{
			std::cout<<"newRecordsCounter: "<<newRecordsCounter<<". state Disp Already was Performed - state 6"<<endl;
		}
};

// State 7
class stateDispStopStillUp: public FSMstate
{
	public:
		void performState(int& newRecordsCounter, ofstream& fout)
		{
			std::cout<<"newRecordsCounter: "<<newRecordsCounter<<". state Disp Stop Still Up Performed - state 7: Relevant For Recording"<<endl<<endl;
			fout<<"newRecordsCounter: "<<newRecordsCounter<<". state Disp Stop Still Up Performed - state 7: Relevant For Recording"<<endl<<endl;
		}
};


class FSM {

	private:
		ifstream _fin;
		ofstream _fout;
		char _filePathIn[120];
		char _filePathOut[120];
		int _numLinesToSkeep;
		int _id;
		int _newRecordsCounter;
		DATA _newRecord;
		DATA _prevRecord;
		DATA _curRecord;

		int _current; // Current State in FSM
		int _next[8][6];

		FSMstate* _states[8];

	public:
		FSM(char* file_path, char* file_path_out, int id, int numLinesToSkeep);
		~FSM();
		ACTIONS GetNextAction();
		void PerformOneStep();
		void CreateLogFile();
};

FSM::FSM(char* file_path_in, char* file_path_out, int id, int numLinesToSkeep): _id(id), _numLinesToSkeep(numLinesToSkeep), _newRecordsCounter(0)
{
	strcpy(_filePathIn, file_path_in);
	strcpy(_filePathOut, file_path_out);
	
	_fin.open("C:\\Users\\user\\Desktop\\data_input.txt"); // open a file
	if (!_fin.good()) 
	{
		exit(1); // exit if file not found
	}

	_fout.open("C:\\Users\\user\\Desktop\\data_output.txt",std::ofstream::out); // open a file

	// Skeep numLinesToSkeep from start
	for(int i=0; i<numLinesToSkeep; i++)
	{
		char buf[MAX_CHARS_PER_LINE];
		_fin.getline(buf, MAX_CHARS_PER_LINE);
	}

	_states[0] = new stateDelNoDisp();
	_states[1] = new stateDelDisp();
	_states[2] = new stateNewRecNoDisp();
	_states[3] = new stateNewRecDisp();
	_states[4] = new stateUpNoDisp();
	_states[5] = new stateUpDispGuy();
	_states[6] = new stateDispAlready();
	_states[7] = new stateDispStopStillUp();

	_current = STATE_DEL_NO_DISP;
	
	_next[STATE_DEL_NO_DISP][ACTION_NEW_NO_DISP] = STATE_NEW_REC_NO_DISP; 
	_next[STATE_DEL_NO_DISP][ACTION_NEW_DISP] = STATE_NEW_REC_DISP; 
	_next[STATE_DEL_NO_DISP][ACTION_UP_NO_DISP] = STATE_DEL_NO_DISP; // Not Relevant.
	_next[STATE_DEL_NO_DISP][ACTION_UP_DISP] = STATE_DEL_NO_DISP; // Not Relevant.
	_next[STATE_DEL_NO_DISP][ACTION_DELETE_NO_DISP] = STATE_DEL_NO_DISP; // Not Relevant.
	_next[STATE_DEL_NO_DISP][ACTION_DELETE_DISP] = STATE_DEL_NO_DISP; // Not Relevant.

	_next[STATE_DEL_DISP][ACTION_NEW_NO_DISP] = STATE_NEW_REC_NO_DISP; 
	_next[STATE_DEL_DISP][ACTION_NEW_DISP] = STATE_NEW_REC_DISP; 
	_next[STATE_DEL_DISP][ACTION_UP_NO_DISP] = STATE_DEL_DISP; // Not Relevant.
	_next[STATE_DEL_DISP][ACTION_UP_DISP] = STATE_DEL_DISP; // Not Relevant.
	_next[STATE_DEL_DISP][ACTION_DELETE_NO_DISP] = STATE_DEL_DISP; // Not Relevant.
	_next[STATE_DEL_DISP][ACTION_DELETE_DISP] = STATE_DEL_DISP; // Not Relevant.

	_next[STATE_NEW_REC_NO_DISP][ACTION_NEW_NO_DISP] = STATE_NEW_REC_NO_DISP; // Not Relevant.
	_next[STATE_NEW_REC_NO_DISP][ACTION_NEW_DISP] = STATE_NEW_REC_NO_DISP; 
	_next[STATE_NEW_REC_NO_DISP][ACTION_UP_NO_DISP] = STATE_UP_NO_DISP; 
	_next[STATE_NEW_REC_NO_DISP][ACTION_UP_DISP] = STATE_UP_DISP_GUY; 
	_next[STATE_NEW_REC_NO_DISP][ACTION_DELETE_NO_DISP] = STATE_DEL_NO_DISP; 
	_next[STATE_NEW_REC_NO_DISP][ACTION_DELETE_DISP] = STATE_NEW_REC_NO_DISP; // Not Relevant. maybe an alternative of 1.

	_next[STATE_NEW_REC_DISP][ACTION_NEW_NO_DISP] = STATE_NEW_REC_DISP; // Not Relevant.
	_next[STATE_NEW_REC_DISP][ACTION_NEW_DISP] = STATE_NEW_REC_DISP; // Not Relevant.
	_next[STATE_NEW_REC_DISP][ACTION_UP_NO_DISP] = STATE_DISP_STOP_STILL_UP; 
	_next[STATE_NEW_REC_DISP][ACTION_UP_DISP] = STATE_DISP_ALREADY;
	_next[STATE_NEW_REC_DISP][ACTION_DELETE_NO_DISP] = STATE_DEL_DISP; 
	_next[STATE_NEW_REC_DISP][ACTION_DELETE_DISP] = STATE_DEL_DISP; 

	_next[STATE_UP_NO_DISP][ACTION_NEW_NO_DISP] = STATE_UP_NO_DISP; // Not Relevant.
	_next[STATE_UP_NO_DISP][ACTION_NEW_DISP] = STATE_UP_NO_DISP; // Not Relevant.
	_next[STATE_UP_NO_DISP][ACTION_UP_NO_DISP] = STATE_UP_NO_DISP;
	_next[STATE_UP_NO_DISP][ACTION_UP_DISP] = STATE_UP_DISP_GUY;
	_next[STATE_UP_NO_DISP][ACTION_DELETE_NO_DISP] = STATE_DEL_NO_DISP; 
	_next[STATE_UP_NO_DISP][ACTION_DELETE_DISP] = STATE_UP_NO_DISP; // Not Relevant. maybe an alternative of 1.

	_next[STATE_UP_DISP_GUY][ACTION_NEW_NO_DISP] = STATE_UP_DISP_GUY; // Not Relevant.
	_next[STATE_UP_DISP_GUY][ACTION_NEW_DISP] = STATE_UP_DISP_GUY; // Not Relevant.
	_next[STATE_UP_DISP_GUY][ACTION_UP_NO_DISP] = STATE_DISP_STOP_STILL_UP; 
	_next[STATE_UP_DISP_GUY][ACTION_UP_DISP] = STATE_DISP_ALREADY; 
	_next[STATE_UP_DISP_GUY][ACTION_DELETE_NO_DISP] = STATE_DEL_DISP;
	_next[STATE_UP_DISP_GUY][ACTION_DELETE_DISP] = STATE_DEL_DISP;

	_next[STATE_DISP_ALREADY][ACTION_NEW_NO_DISP] = STATE_DISP_ALREADY; // Not Relevant.
	_next[STATE_DISP_ALREADY][ACTION_NEW_DISP] = STATE_DISP_ALREADY; // Not Relevant.
	_next[STATE_DISP_ALREADY][ACTION_UP_NO_DISP] = STATE_DISP_STOP_STILL_UP;
	_next[STATE_DISP_ALREADY][ACTION_UP_DISP] = STATE_DISP_ALREADY; 
	_next[STATE_DISP_ALREADY][ACTION_DELETE_NO_DISP] = STATE_DEL_DISP;
	_next[STATE_DISP_ALREADY][ACTION_DELETE_DISP] = STATE_DEL_DISP;

	_next[STATE_DISP_STOP_STILL_UP][ACTION_NEW_NO_DISP] = STATE_DISP_STOP_STILL_UP; // Not Relevant.
	_next[STATE_DISP_STOP_STILL_UP][ACTION_NEW_DISP] = STATE_DISP_STOP_STILL_UP; // Not Relevant.
	_next[STATE_DISP_STOP_STILL_UP][ACTION_UP_NO_DISP] = STATE_UP_NO_DISP;
	_next[STATE_DISP_STOP_STILL_UP][ACTION_UP_DISP] = STATE_UP_DISP_GUY; 
	_next[STATE_DISP_STOP_STILL_UP][ACTION_DELETE_NO_DISP] = STATE_DEL_NO_DISP;
	_next[STATE_DISP_STOP_STILL_UP][ACTION_DELETE_DISP] = STATE_DISP_STOP_STILL_UP; // Not Relevant. maybe an alternative of 1.

}

FSM::~FSM()
{
	_fin.close();
	_fout.close();
}

ACTIONS FSM::GetNextAction()
{
	int curLineId=-1;

	char buf[MAX_CHARS_PER_LINE];
	_fin.getline(buf, MAX_CHARS_PER_LINE);
	
	if (_fin.eof())
	{
		return ACTIONS_FINISH;
	}

	// parse the line into blank-delimited tokens
    int n = 0; // a for-loop index
    
    // array to store memory addresses of the tokens in buf
    const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0
    
    // parse the line
    token[0] = strtok(buf, DELIMITER); // first token
    if (token[0]) // zero if line is blank
    {
      for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
      {
        token[n] = strtok(0, DELIMITER); // subsequent tokens
        if (!token[n]) break; // no more tokens
      }
    }
	
	if(token[ID_COL])
	{
		std::string str_dec2(token[ID_COL]);
		curLineId=std::stoi(str_dec2,nullptr,0);		
	}

	while (curLineId!=_id)
	{
		char buf[MAX_CHARS_PER_LINE];
		_fin.getline(buf, MAX_CHARS_PER_LINE);

		if (_fin.eof())
		{
			return ACTIONS_FINISH;
		}
	
		// parse the line into blank-delimited tokens
		n = 0; // a for-loop index
    
		// array to store memory addresses of the tokens in buf
		//const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0
    
		for(int temp=0; temp<MAX_TOKENS_PER_LINE ; temp++)
		{
			token[temp]=NULL;
		}

		// parse the line
		token[0] = strtok(buf, DELIMITER); // first token
		if (token[0]) // zero if line is blank
		{
			for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
			{
				token[n] = strtok(0, DELIMITER); // subsequent tokens
				if (!token[n]) break; // no more tokens
			}
		}
	
		// Check Validity of line
		if(token[ID_COL])
		{
			std::string str_dec2(token[ID_COL]);
			curLineId=std::stoi(str_dec2,nullptr,0);		
		}

	}

	// process (print) the tokens
    //for (int i = 0; i < n; i++) // n = #of tokens
	//{
    // cout << "Token[" << i << "] = " << token[i]<<endl;
	//}


	std::string str_dec3(token[DISP_COL]);
	int disp_f = std::stoi(str_dec3,nullptr,0);

	char str[15];
	int ret=0;

	strcpy(str, token[EVENT_COL]);

	// Check if str first three letters eqeuals "New"
	if(str[0]=='N' && str[1]== 'e' && str[2]=='w')
	{
		ret = 1;
	}

	if(ret==1 && disp_f==0)
	{
		return ACTION_NEW_NO_DISP;
	}

	if(ret==1 && disp_f==1)
	{
		return ACTION_NEW_DISP;
	}

	// Check if str first two letters eqeuals "Up"
	ret = 0;
	if(str[0]=='U' && str[1]== 'p')
	{
		ret = 1;
	}

	if(ret==1 && disp_f==0)
	{
		return ACTION_UP_NO_DISP;
	}

	if(ret==1 && disp_f==1)
	{
		return 	ACTION_UP_DISP;
	}

	// Check if str first three letters eqeuals "Del"
	ret = 0;
	if(str[0]=='D' && str[1]== 'e' && str[2]=='l')
	{
		ret = 1;
	}

	if(ret==1 && disp_f==0)
	{
		return ACTION_DELETE_NO_DISP;
	}

	if(ret==1 && disp_f==1)
	{
		return 	ACTION_DELETE_DISP;
	}
	
	return ACTIONS_NONE; //Failure has occurred

}

void FSM::PerformOneStep()
{
	ACTIONS action = GetNextAction();
	
	if (action == ACTIONS_FINISH)
	{
		return;
	}

	// We get here only if action is OK and we are not finished yet
	_current = _next[_current][action];
	_states[_current]->performState(_newRecordsCounter, _fout);
}

void FSM::CreateLogFile()
{
	int numSteps = 1000;
	for(int i=0; i < numSteps; i++)
	{
		PerformOneStep();
	}
}

int main()
{

	FSM MyFsm("C:\\Users\\user\\Desktop\\data_input.txt","C:\\Users\\user\\Desktop\\data_output.txt",1,1);

	MyFsm.CreateLogFile();

	cout<<endl;

}