#include "CControl.h"

CControl::CControl(char command)
{
    this->command = command;
    defaultBind = command;
}

void CControl::set(char command)
{
    if(checkValidCommand(command))
        this->command = command;
}

bool CControl::checkValidCommand(char command)
{
    //check if valid
    return true;
}

CControl::~CControl()
{
}