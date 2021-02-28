#pragma once
class CControl
{
private:

	char command, defaultBind;

	bool checkValidCommand(char command);
public:

	CControl(char command);
	~CControl();
	
	void set(char command);

	char get()		{ return command;		}
	void reset()	{ command = defaultBind;}
};